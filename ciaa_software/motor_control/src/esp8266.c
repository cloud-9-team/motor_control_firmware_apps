/*==================[inclusions]=============================================*/

#include "chip.h"
#include "os.h"               /* <= operating system header */
#include "ciaaPOSIX_stdio.h"  /* <= device handler header */
#include "ciaaPOSIX_string.h" /* <= string header */
#include "ciaak.h"            /* <= ciaa kernel header */
#include "esp8266.h"         /* <= own header */
#include "StringUtils.h"
#include "debug_logger.h"
#include "ciaaLibs_CircBufExt.h"
#include "timer.h"

/*==================[macros and definitions]=================================*/

/** \brief Máxima cantidad de comandos a encolar. Su valor debe ser potencia de 2. */
#define MAX_QUEUED_COMMANDS     (16)

/** \brief Tamaño del buffer interno para guardar datos a enviar. Su valor debe ser potencia de 2. */
#define MAX_SENDBUFFER_SIZE     (2048)

/** \brief Cantidad de parsers de uso libre disponibles. */
#define COMMAND_PARSERS_SIZE	(5)

#define AT_Command_toString(cmd) AT_Command_string[(cmd)]
#define isCommandTypeValid(command, type) ((valid_types[(command)] & (uint8_t)(type)) != 0)


#define internalBuffer_getWrittenLength(dataInfo) (dataInfo)
#define internalBuffer_wasDataWritten(dataInfo) (internalBuffer_getWrittenLength(dataInfo) > 0)
#define internalBuffer_writeData(buf, length) ciaaLibs_circBufPut(&circBuffer, (buf), (length))
#define internalBuffer_writeString(str) internalBuffer_writeData((str), ciaaPOSIX_strlen(str))
#define internalBuffer_deleteFrontData(dataInfo) ciaaLibs_circBufUpdateHead(&circBuffer, internalBuffer_getWrittenLength(dataInfo))
#define internalBuffer_sendData(dataInfo) ciaaLibs_circBufWriteTo(&circBuffer, fd_uart, (dataInfo))


#define isCommandWaitFunctionDefined(command) (waitFunctions[(command)] != NULL)

/*==================[internal data declaration]==============================*/

typedef uint16_t InternalBufferedDataInfo;

typedef struct {
	char * buffer;
	uint16_t length;
} ExternalBufferedDataInfo;

typedef enum {
	CONTENT_EMPTY = 0,
	CONTENT_INTERNAL = 1,
	CONTENT_EXTERNAL = 2
} ContentType;

typedef struct {
	AT_Command                  command;
	AT_Type                     type;
	InternalBufferedDataInfo    paramsData;
	ContentType                 contentInfo;
	union{
		InternalBufferedDataInfo internal;
		ExternalBufferedDataInfo external;
	} content;
} QueuedCommand;

typedef int32_t (*paramsToString_type)(QueuedCommand* commandData, void* parameters);

typedef enum
{
	WAIT_RESULT_TIMEOUT	= -1,
	WAIT_RESULT_BUSY = 0,
	WAIT_RESULT_OK = 1,
	WAIT_RESULT_ERROR = 2
} WaitResult;

typedef WaitResult (*waitFunction_type)(void);

/*==================[internal functions declaration]=========================*/

static inline const char * AT_Type_toString(const AT_Type type);

/* Funciones para armar los argumentos de los comandos a enviar */
static int32_t paramsToString_cwmode(QueuedCommand* commandData, void* parameters);
static int32_t paramsToString_cipsend(QueuedCommand* commandData, void* parameters);
static int32_t paramsToString_cwsap(QueuedCommand* commandData, void* parameters);
static int32_t paramsToString_cipmux(QueuedCommand* commandData, void* parameters);
static int32_t paramsToString_cipserver(QueuedCommand* commandData, void* parameters);

/* Funciones de ayuda para mantener la cola de comandos */
static int32_t queue_cmd_push(QueuedCommand newCommand);
static QueuedCommand queue_cmd_pop(void);
static uint8_t queue_cmd_isEmpty(void);

/* Funciones de espera que serán utilizadas */
static int8_t waitForAny(const char ** str, uint8_t size);
static WaitResult rst_wait(void);
static WaitResult wait_OK_busy_error(void);
static WaitResult wait_cipsend(void);


/*==================[internal data definition]===============================*/

static const char * AT_Command_string[AT_COMMAND_SIZE] = {
		"AT+RST",
		"AT+CWMODE",
		"AT+CWSAP",
		"AT+CWSAP_CUR",
		"AT+CWSAP_DEF",
		"AT+CIPMUX",
		"AT+CIPSERVER",
		"AT+CIPSEND",
		"AT+CIPSENDEX",
		"AT+CIPSENDBUF"
};

static const char * AT_Type_string[4] = {
		"=?",   /* Test */
		"?",    /* Query */
		"=",    /* Set */
		""      /* Execute */
};

/** \brief Tipos de operaciones válidas para cada uno de los comandos.
 *
 * Permite si una operación es válida para un comando dado.
 *
 */
static const uint8_t valid_types[AT_COMMAND_SIZE] = {
		AT_TYPE_EXECUTE,                            /* <= AT+RST */
		AT_TYPE_QUERY | AT_TYPE_SET | AT_TYPE_TEST, /* <= AT+CWMODE */
		AT_TYPE_QUERY | AT_TYPE_SET,                /* <= AT+CWSAP */
		AT_TYPE_QUERY | AT_TYPE_SET,                /* <= AT+CWSAP_CUR */
		AT_TYPE_QUERY | AT_TYPE_SET,                /* <= AT+CWSAP_DEF */
		AT_TYPE_QUERY | AT_TYPE_SET,                /* <= AT+CIPMUX */
		AT_TYPE_QUERY | AT_TYPE_SET,                /* <= AT+CIPSERVER */
		AT_TYPE_SET,                                /* <= AT+CIPSEND */
		AT_TYPE_SET,                                /* <= AT+CIPSENDEX */
		AT_TYPE_SET,                                /* <= AT+CIPSENDBUF */
};

/** \brief Asociación de funciones para armar los argumentos con cada comando.
 *
 * Al momento de encolar un comando, si se trata de un comando tipo SET, es
 * necesario generar una string con los parámetros, para enviar al módulo WiFi.
 * Por ejemplo, el comando \p AT+CWMODE encolado con tipo SET, requiere un argumento
 * que indica el modo de funcionamiento de módulo WiFi (ver \p AT_CWMODE_MODE).
 * La cadena final a enviar será "AT+CWMODE=1", donde 1 representa el modo elegido.
 * Ese "1" es el parámetro, y es lo que estas funciones se encargan de generar.
 *
 * Para acceder a la función debe utilizarse su enumerativo como índice, por
 * ejemplo \p waitFunctions[AT_CWMODE].
 *
 */
static const paramsToString_type paramsToString[AT_COMMAND_SIZE] = {
		0,
		&paramsToString_cwmode,
		&paramsToString_cwsap,
		&paramsToString_cwsap,
		&paramsToString_cwsap,
		&paramsToString_cipmux,
		&paramsToString_cipserver,
		&paramsToString_cipsend,
		&paramsToString_cipsend,
		&paramsToString_cipsend
};

/** \brief Asociación de comando con máximo número de reintentos.
 *
 * Para obtener el máximo número de reintentos de un comando, debe
 * utilizarse su enumerativo como índice, por ejemplo \p maxRetryNumber[AT_RST].
 *
 */
static const uint8_t maxRetryNumber[AT_COMMAND_SIZE] =
{
		5,  /* <= AT+RST */
		1,  /* <= AT+CWMODE */
		3,  /* <= AT+CWSAP */
		3,  /* <= AT+CWSAP_CUR */
		3,  /* <= AT+CWSAP_DEF */
		1,  /* <= AT+CIPMUX */
		1,  /* <= AT+CIPSERVER */
		2,  /* <= AT+CIPSEND */
		2,  /* <= AT+CIPSENDEX */
		1,  /* <= AT+CIPSENDBUF */
};

/** \brief Asociación de funciones de espera con cada comando.
 *
 * Las funciones de espera son llamadas luego de enviar un comando. El código
 * que ejecutan debe ser el necesario para obtener una conclusión respecto al
 * envío del comando, es decir, que permita aseverar que un comando ha sido
 * enviado exitosamente o no. Por lo general, esperan a que se reciba una string
 * estilo "OK", "ERROR", etc. Lógicamente, deben tener alguna secuencia para
 * determinar si se ha esperado demasiado tiempo, así se evita retrasar otros
 * comandos en la cola.
 *
 * Para acceder a la función de espera de un comando, debe utilizarse su
 * enumerativo como índice, por ejemplo \p waitFunctions[AT_RST].
 *
 */
static const waitFunction_type waitFunctions[AT_COMMAND_SIZE] = {
		&rst_wait,  /* <= AT+RST */
		&wait_OK_busy_error,    /* <= AT+CWMODE */
		&wait_OK_busy_error,    /* <= AT+CWSAP */
		&wait_OK_busy_error,    /* <= AT+CWSAP_CUR */
		&wait_OK_busy_error,    /* <= AT+CWSAP_DEF */
		&wait_OK_busy_error,    /* <= AT+CIPMUX */
		&wait_OK_busy_error,    /* <= AT+CIPSERVER */
		&wait_cipsend,          /* <= AT+CIPSEND */
		&wait_cipsend,          /* <= AT+CIPSENDEX */
		&wait_cipsend           /* <= AT+CIPSENDBUF */
};

/* Variables para mantener el estado de la cola de comandos */
static QueuedCommand command_queue[MAX_QUEUED_COMMANDS];
static uint8_t queue_front = 0;
static uint8_t queue_count = 0;

/* Buffer circular para guardar los datos a enviar de los comandos */
static char internalBuffer_data[MAX_SENDBUFFER_SIZE];
static ciaaLibs_CircBufType circBuffer;

/* Variables para mantener los parsers de uso libre */
static Parser* cmd_parsers[COMMAND_PARSERS_SIZE];
static uint8_t cmd_parsers_length = 0;

/* Funciones de callback */
static callbackCommandSentFunction_type callbackCommandSent = NULL;
static callbackConnectionChangedFunction_type callbackConnectionChanged = NULL;
static callbackDataReceivedFunction_type callbackDataReceived = NULL;
static callbackResetDetectedFunction_type callbackResetDetected = NULL;


/** \brief File descriptor of the RS232 uart
 *
 * Device path /dev/serial/uart/2
 */
static int32_t fd_uart;

/* Parsers a utilizar dentro de WiFiDataReceivedTask */
static Parser parserIPD = INITIALIZER_AT_IPD;
static Parser parserConnectionClose = INITIALIZER_AT_CONNECTIONCLOSE;
static Parser parserConnectionOpen = INITIALIZER_AT_CONNECTIONOPEN;
static Parser parserConnectionFailed = INITIALIZER_AT_CONNECTIONFAILED;
static Parser parserResetDetection = INITIALIZER_AT_RESET_DETECTION;

/** \brief Parsers para capturar cadenas de caracteres. */
static Parser parserLiteral[3] =
{
		INITIALIZER_LITERAL_PARSER,
		INITIALIZER_LITERAL_PARSER,
		INITIALIZER_LITERAL_PARSER
};

/** \brief Mantiene el estado de las conexiones. */
static ConnectionStatus connectionStatus[MAX_MULTIPLE_CONNECTIONS];


/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static inline const char * AT_Type_toString(const AT_Type type){
	switch(type){
	case AT_TYPE_TEST:
		return AT_Type_string[0];
		break;
	case AT_TYPE_QUERY:
		return AT_Type_string[1];
		break;
	case AT_TYPE_SET:
		return AT_Type_string[2];
		break;
	default:
		return AT_Type_string[3];
	}
}

static inline void deleteCommandDataFromBuffer(QueuedCommand* cmd)
{
	/* Elimino el comando del buffer, el contenido (si está presente) también */
	internalBuffer_deleteFrontData(cmd->paramsData);

	if (cmd->contentInfo == CONTENT_INTERNAL)
	{
		internalBuffer_deleteFrontData(cmd->content.internal);
	}
}

/*==================[start of command queue functions]=======================*/

static int32_t queue_cmd_push(QueuedCommand newCommand)
{
	int32_t ret = -1;

	if (queue_count < MAX_QUEUED_COMMANDS){
		command_queue[(queue_front + queue_count) & (MAX_QUEUED_COMMANDS - 1)] = newCommand;
		queue_count++;
		ret = 1;
	}

	return ret;
}

static QueuedCommand queue_cmd_pop(void){
	QueuedCommand ret;

	if (queue_count > 0){
		ret = command_queue[queue_front];
		queue_front = (uint8_t)((queue_front + 1) & (MAX_QUEUED_COMMANDS - 1));
		queue_count--;
	}

	return ret;
}

static uint8_t queue_cmd_isEmpty(void){
	if (queue_count == 0){
		return 1;
	}else{
		return 0;
	}
}

/** Al recibir cualquier dato por la UART conectada al módulo ESP8266,
 * se intentará matchear esos datos con los parsers que sea agregados aquí.
 *
 * Se le enviarán caracteres hasta que haya match, o sea eliminado de la
 * lista con la función \p cmd_parsers_clear().
 */
static inline void cmd_parsers_add(Parser* parser)
{
	if (cmd_parsers_length < COMMAND_PARSERS_SIZE && parser != NULL)
	{
		cmd_parsers[cmd_parsers_length++] = parser;
	}
}

static inline void cmd_parsers_clear(void)
{
	cmd_parsers_length = 0;
}

/*==================[end of command queue functions]=========================*/

/*==================[start of parameters functions]==========================*/

static int32_t paramsToString_cwmode(QueuedCommand* cmd, void* parameters){
	AT_CWMODE_MODE mode = (AT_CWMODE_MODE)parameters;
	char buf;

	if (mode >= AT_CWMODE_MODE_MIN && mode <= AT_CWMODE_MODE_MAX){
		buf = '0' + mode;
		cmd->paramsData = internalBuffer_writeData(&buf, 1);
		if (internalBuffer_wasDataWritten(cmd->paramsData)){
			return 1;
		}
	}
	return -1;
}

static int32_t paramsToString_cipmux(QueuedCommand* cmd, void* parameters){
	AT_CIPMUX_MODE mode = (AT_CIPMUX_MODE)parameters;
	char buf;

	if (mode <= AT_CIPMUX_MULTIPLE_CONNECTION){
		buf = '0' + mode;
		cmd->paramsData = internalBuffer_writeData(&buf, 1);
		if (internalBuffer_wasDataWritten(cmd->paramsData)){
			return 1;
		}
	}

	return -1;
}

static int32_t paramsToString_cipserver(QueuedCommand* cmd, void* parameters){
	AT_CIPSERVER_DATA* data = (AT_CIPSERVER_DATA*)parameters;
	char paramStr[8] = "N,XXXXX";

	if (data->mode <= AT_CIPSERVER_CREATE) /* Si el modo tiene un valor correcto */
	{
		paramStr[0] = '0' + data->mode;
		if (data->port == 0) /* Si el puerto es inválido */
		{
			/* Se usa el puerto por defecto del ESP8266 */
			paramStr[1] = '\0';
		}
		else
		{
			uintToString(data->port, 1, (unsigned char *)&paramStr[2]);
		}

		cmd->paramsData = internalBuffer_writeString(paramStr);
		if (internalBuffer_wasDataWritten(cmd->paramsData))
		{
			return internalBuffer_getWrittenLength(cmd->paramsData);
		}
	}

	return -1;
}

static int32_t paramsToString_cipsend(QueuedCommand* cmd, void* parameters){
	AT_CIPSEND_DATA* data = (AT_CIPSEND_DATA*)parameters;
	int32_t cantChars;
	char paramStr[7] = "N,XXXX";

	if (data->content == 0)
	{
		return -1;
	}

	if (data->length == AT_CIPSEND_ZERO_TERMINATED_CONTENT)
	{
		data->length = ciaaPOSIX_strlen(data->content);
	}

	if (data->length > 0)
	{
		paramStr[0] = '0' + data->connectionID;
		uintToString(data->length, 1, (unsigned char *)&paramStr[2]);
		cmd->paramsData = internalBuffer_writeString(paramStr);
		if (internalBuffer_wasDataWritten(cmd->paramsData))
		{
			cantChars = internalBuffer_getWrittenLength(cmd->paramsData);
			if (data->copyContentToBuffer == AT_CIPSEND_CONTENT_COPYTOBUFFER)
			{
				cmd->content.internal = internalBuffer_writeData(data->content, data->length);
				cmd->contentInfo = CONTENT_INTERNAL;
				if (!internalBuffer_wasDataWritten(cmd->content.internal))
				{
					/* No hay lugar en el buffer */
					/* Elimino la cadena de los parámetros */
					ciaaLibs_circBufDeleteLastN(&circBuffer, internalBuffer_getWrittenLength(cmd->paramsData));
					return -1;
				}

				cantChars += internalBuffer_getWrittenLength(cmd->content.internal);
			}
			else if (data->copyContentToBuffer == AT_CIPSEND_CONTENT_DONT_COPY)
			{
				cmd->content.external.buffer = data->content;
				cmd->content.external.length = data->length;
				cmd->contentInfo = CONTENT_EXTERNAL;
			}

			return cantChars;
		}
	}

	return -1;
}

static int32_t paramsToString_cwsap(QueuedCommand* cmd, void* parameters){
	AT_CWSAP_DATA* data = (AT_CWSAP_DATA*)parameters;
	char paramStr[107];
	char * ptr = paramStr;
	int cant;

	/* Formato para Set: AT+CWSAP=<ssid>,<pwd>,<chl>,<ecn>
       ssid: string, SSID del soft AP del ESP8266, 32 chars máx.
       pwd: string, contraseña del soft AP, de 8 a 64 chars.
       chl: int, channel id, de 1 a 14 para 802.11b/g/n.
       ecn: int, encriptación empleada en la comunicación, posibles valores: 0, 2, 3, 4.
       Cantidad máxima de caracteres necesaria: 32 + 64 + 2 + 1 + 4 (comillas) + 3 (comas) + '\0' = 107
	 */

	if (data->ssid == 0 || data->pwd == 0 || data->chl < 1 || data->chl > 14)
	{
		return -1;
	}

	*ptr++ = '"';
	cant = strlcpy(&paramStr[1], data->ssid, 32 + 1);
	if (cant <= 32)
	{
		ptr += cant;
		*ptr++ = '"';
		*ptr++ = ',';
		*ptr++ = '"';
		cant = strlcpy(ptr, data->pwd, 64 + 1);

		/* Si no hay encriptación no hace falta contraseña */
		if ((data->ecn == AT_SAP_ENCRYPTION_OPEN) || (cant >= 8 && cant <= 64))
		{
			ptr += cant;
			*ptr++ = '"';
			*ptr++ = ',';
			ptr = (char *)uintToString(data->chl, 1, (unsigned char *)ptr);
			*ptr++ = ',';
			ptr = (char *)uintToString(data->ecn, 1, (unsigned char *)ptr);

			cmd->paramsData = internalBuffer_writeString(paramStr);
			if (internalBuffer_wasDataWritten(cmd->paramsData))
			{
				return internalBuffer_getWrittenLength(cmd->paramsData);
			}
		}
	}

	return -1;
}

/*==================[end of parameters functions]============================*/

/*==================[start of wait functions]================================*/

static int8_t waitForAny(const char ** str, uint8_t size)
{
	uint16_t timeoutMS = 1500;
	uint8_t i;
	int8_t ret = WAIT_RESULT_TIMEOUT;


	for (i = 0; i < size; i++)
	{
		/* config parser */
		parser_init(&parserLiteral[i]);
		literalParser_setStringToMatch(&parserLiteral[i], str[i]);

		/* activate parser */
		cmd_parsers_add(&parserLiteral[i]);
	}


	/* wait for the parsers OR timeout */
	while (--timeoutMS > 0 && ret < 0)
	{
		for (i = 0; i < size; i++)
		{
			if (parser_getStatus(&parserLiteral[i]) == STATUS_COMPLETE)
			{
				ret = i;
				break;
			}
		}

		if (ret < 0)
		{
			timer_delay_ms(1);
		}
	}

	cmd_parsers_clear();

	return ret;
}

static WaitResult rst_wait(void)
{
	char * waitString[1] = {"\r\nready"};
	int8_t ret;

	ret = waitForAny(waitString, 1);
	return ((ret < 0) ? WAIT_RESULT_TIMEOUT : WAIT_RESULT_OK);
}

static WaitResult wait_OK_busy_error(void)
{
	char * waitString[3] = {"busy p...", "\r\nOK", "\r\nERROR"};
	return waitForAny(waitString, 3);
}

static WaitResult wait_cipsend(void)
{
	char * waitString[3] = {"busy p...", "OK\r\n>", "\r\nERROR"};
	return waitForAny(waitString, 3);
}

/*==================[end of wait functions]==================================*/

/*==================[external functions definition]==========================*/

void esp8266_init(void)
{

	/* parsers initialization */
	parser_init(&parserIPD);
	parser_init(&parserConnectionOpen);
	parser_init(&parserConnectionClose);
	parser_init(&parserConnectionFailed);
	parser_init(&parserResetDetection);

	/* open UART connected to RS232 connector */
	fd_uart = ciaaPOSIX_open("/dev/serial/uart/2", ciaaPOSIX_O_RDWR | ciaaPOSIX_O_NONBLOCK);

	/* change baud rate */
	ciaaPOSIX_ioctl(fd_uart, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_115200);

	/* inicialización de buffer interno */
	ciaaLibs_circBufInit(&circBuffer, internalBuffer_data, MAX_SENDBUFFER_SIZE);

	/* initialize timer for delay */
	timer_init();

	/* delay for WiFi module initialization */
	timer_delay_ms(900);

	/* set alarm for receiving task */
	SetRelAlarm(ActivateWiFiDataReceiveTask, 10, 20);
}


int32_t esp8266_queueCommand(AT_Command command, AT_Type type, void* parameters){
	int32_t ret = 0;
	QueuedCommand newCommand = {0};

	/* Verifico que el comando sea uno de los definidos, y que el tipo sea compatible con este */
	if (command >= AT_COMMAND_SIZE || !isCommandTypeValid(command, type)){
		return -1;
	}

	newCommand.command = command;
	newCommand.type = type;
	newCommand.contentInfo = CONTENT_EMPTY;

	if (type == AT_TYPE_SET){ /* Sólo los comandos de tipo SET necesitan parámetros al ser llamados */
		ret = paramsToString[command](&newCommand, parameters);
	}

	if (ret >= 0)
	{
		ret = queue_cmd_push(newCommand);
	}

	return ret;
}


void esp8266_doWork(void)
{
	QueuedCommand cmd;
	uint8_t haveToRetry, retry, buf[16];
	char * waitStrings[3] = {0};
	WaitResult result;

	while(!queue_cmd_isEmpty())
	{
		cmd = queue_cmd_pop();

		haveToRetry = 1;
		for (retry = 1; retry <= maxRetryNumber[cmd.command] && haveToRetry; retry++)
		{
			logger_print_string("\r\n");

			/* Envío: <COMANDO><TIPO><PARAMETROS>, por ejemplo AT+CIPSERVER=1,8080, donde COMANDO:AT+CIPSERVER, TIPO:= y PARAMETROS:1,8080 */
			ciaaPOSIX_write(fd_uart, AT_Command_toString(cmd.command), ciaaPOSIX_strlen(AT_Command_toString(cmd.command)));
			ciaaPOSIX_write(fd_uart, AT_Type_toString(cmd.type), ciaaPOSIX_strlen(AT_Type_toString(cmd.type)));

			internalBuffer_sendData(cmd.paramsData);

			/* Terminador de comando */
			ciaaPOSIX_write(fd_uart, "\r\n", 2);

			if (isCommandWaitFunctionDefined(cmd.command))
			{
				result = waitFunctions[cmd.command]();
				if (result == WAIT_RESULT_BUSY || result == WAIT_RESULT_TIMEOUT)
				{
					logger_print_string("Retry...");
					/* Hubo error al esperar (timeout, etc), por lo cual reintento si es posible */
					continue;
				}
			}

			/* En teoría el comando ha sido enviado correctamente.
               Borro la información de los parámetros del buffer. */
			internalBuffer_deleteFrontData(cmd.paramsData);

			/* Si hay datos adicionales a enviar correspondientes al comando... */
			if (cmd.contentInfo != CONTENT_EMPTY)
			{
				if (cmd.contentInfo == CONTENT_INTERNAL)
				{
					internalBuffer_sendData(cmd.content.internal);
					internalBuffer_deleteFrontData(cmd.content.internal);
				}
				else /* cmd.contentInfo == CONTENT_EXTERNAL */
                {
					ciaaPOSIX_write(fd_uart, cmd.content.external.buffer, cmd.content.external.length);
				}

				ciaaPOSIX_strcpy((char *)uintToString((cmd.contentInfo == CONTENT_INTERNAL) ? internalBuffer_getWrittenLength(cmd.content.internal) : cmd.content.external.length,
						1,
						(unsigned char *)ciaaPOSIX_strcpy((char *)buf, "Recv ")),
						" byte");

				waitStrings[0] = "ERROR"; /* If connection cannot be established, or it’s not a TCP connection, or buffer full, or some other error occurred, returns ERROR */
				waitStrings[1] = (char*) buf; /* Recv xxxxx byte */
				waitForAny(waitStrings, 2);
			}

			if (callbackCommandSent != NULL)
			{
				callbackCommandSent(cmd.command);
			}

			/* Terminó correctamente la ejecución del comando, no reintento más */
			haveToRetry = 0;
			if (!isCommandWaitFunctionDefined(cmd.command))
			{
				/* Si el comando no tiene función de espera para confirmar la finalización de su ejecución,
				 * se asume un delay.
				 */
				timer_delay_ms(200);
			}

		}

		if (haveToRetry)
		{
			/* Hay que reintentar, pero se acabaron los intentos... */
			logger_print_string("Reset limit excedeed");
			deleteCommandDataFromBuffer(&cmd);
		}
	}
}


void esp8266_registerCommandSentCallback(callbackCommandSentFunction_type fcn)
{
	callbackCommandSent = fcn;
}


void esp8266_registerDataReceivedCallback(callbackDataReceivedFunction_type fcn)
{
	callbackDataReceived = fcn;
}


void esp8266_registerConnectionChangedCallback(callbackConnectionChangedFunction_type fcn)
{
	callbackConnectionChanged = fcn;
}


void esp8266_registerResetDetectedCallback(callbackResetDetectedFunction_type fcn)
{
	callbackResetDetected = fcn;
}


void esp8266_setReceiveBuffer(uint8_t * buf, uint16_t size)
{
	if (parser_getStatus(&parserIPD) != STATUS_UNINITIALIZED)
	{
		parser_ipd_setBuffer(&parserIPD, buf, size);
	}
}


ssize_t esp8266_writeRawData(void * buf, size_t size)
{
	return ciaaPOSIX_write(fd_uart, buf, size);
}


ConnectionStatus esp8266_getConnectionStatus(uint8_t connectionID)
{
	return (connectionID < MAX_MULTIPLE_CONNECTIONS ? connectionStatus[connectionID] : CONNECTION_STATUS_CLOSE);
}


/** \brief Tarea de recepción de datos de la UART conectada al módulo WiFi
 *
 * Recibe datos enviados por el módulo WiFi, los cuales son procesados para generar
 * las notificaciones antes los distintos sucesos que pueden ocurrir.
 *
 */
TASK(WiFiDataReceiveTask)
{
	int8_t buf[254];
	int8_t newChar;
	int32_t ret;
	uint8_t i, j;
	ConnectionInfo newInfo = {CONNECTION_STATUS_CLOSE, MAX_MULTIPLE_CONNECTIONS};
	ReceivedDataInfo rcvData;

	ret = ciaaPOSIX_read(fd_uart, buf, sizeof(buf));
	if(ret > 0)
	{
		/* Echo in serial terminal */
		logger_print_data(buf, ret);

        /* Envío cada carácter a los parsers encargados de encontrar patrones útiles */
		for (i = 0; i < ret; i++){
			newChar = buf[i];

			if (parser_tryMatch(&parserIPD, newChar) == STATUS_COMPLETE){
				if (callbackDataReceived != NULL)
				{
					ciaaPOSIX_memcpy(&rcvData, (PARSER_RESULTS_IPD_T *) parser_getResults(&parserIPD), sizeof(ReceivedDataInfo));
					callbackDataReceived(rcvData);
				}
			}

			if (!parser_ipd_isDataBeingSaved(&parserIPD))
			{
				/* Si no se está guardando en el buffer significa que no hay match
				 * o todavía no se completó la parte que precede al contenido mismo.
				 * Por tanto, es correcto intentar matchear los demás casos.
				 * Una vez que se comienza a procesar el contenido de IPD, se ignora
				 * al resto de los parsers ya que se supone que el contenido no es
				 * interferido por otros mensajes. */

                /* Envío el carácter a los parsers de uso libre... */
				for (j = 0; j < cmd_parsers_length; j++)
				{
				    /* ... sólo si todavía no han encontrado el patrón */
					if (parser_getStatus(cmd_parsers[j]) != STATUS_COMPLETE)
					{
						parser_tryMatch(cmd_parsers[j], newChar);
					}
				}

                /* Intento detectar si se abrió una conexión */
				if (parser_tryMatch(&parserConnectionOpen, newChar) == STATUS_COMPLETE)
				{
					newInfo.connectionID = ((PARSER_RESULTS_CONNECTIONOPEN_T *)parser_getResults(&parserConnectionOpen))->connectionID;
					newInfo.newStatus = CONNECTION_STATUS_OPEN;

					connectionStatus[newInfo.connectionID] = CONNECTION_STATUS_OPEN;
				}

                /* Intento detectar si se cerró una conexión */
				if (parser_tryMatch(&parserConnectionClose, newChar) == STATUS_COMPLETE)
				{
					newInfo.connectionID = ((PARSER_RESULTS_CONNECTIONCLOSE_T *)parser_getResults(&parserConnectionClose))->connectionID;
					newInfo.newStatus = CONNECTION_STATUS_CLOSE;

					connectionStatus[newInfo.connectionID] = CONNECTION_STATUS_CLOSE;
				}

                /* Intento detectar si una conexión falló, y por consiguiente, se cerró. */
				if (parser_tryMatch(&parserConnectionFailed, newChar) == STATUS_COMPLETE)
				{
					newInfo.connectionID = ((PARSER_RESULTS_CONNECTIONFAILED_T *)parser_getResults(&parserConnectionFailed))->connectionID;
					newInfo.newStatus = CONNECTION_STATUS_CLOSE;

					connectionStatus[newInfo.connectionID] = CONNECTION_STATUS_CLOSE;
				}

				if (newInfo.connectionID < MAX_MULTIPLE_CONNECTIONS && callbackConnectionChanged != NULL)
				{
					callbackConnectionChanged(newInfo);
				}

                /* Intento detectar un reset del módulo WiFi */
				if (parser_tryMatch(&parserResetDetection, newChar) == STATUS_COMPLETE)
				{
					for (j = 0; j < MAX_MULTIPLE_CONNECTIONS; j++)
					{
					    /* Detecté un reset, por lo cual todas las conexiones han sido cerradas */
						if (connectionStatus[j] != CONNECTION_STATUS_CLOSE)
						{
							connectionStatus[j] = CONNECTION_STATUS_CLOSE;

                            /* Armo los datos para notificar si es que está el callback habilitado */
							if (callbackConnectionChanged != NULL)
							{
							    newInfo.connectionID = j;
                                newInfo.newStatus = CONNECTION_STATUS_CLOSE;
								callbackConnectionChanged(newInfo);
							}

						}

					}

					if (callbackResetDetected != NULL)
						callbackResetDetected();
				}

			}

		}

	}

	TerminateTask();
}

/*==================[end of file]============================================*/
