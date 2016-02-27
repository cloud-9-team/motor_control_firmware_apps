#ifndef ESP8266_H
#define ESP8266_H

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Maneja la interfaz con el módulo WiFi.
 *
 * Este módulo tiene el objetivo de abstraer al programador de los detalles
 * correspondientes a la comunicación con el módulo WiFi, tales como tiempos
 * a respetar y detalles de la sintaxis de los comandos.
 *
 * Por una parte, procesa los mensajes recibidos del módulo WiFi y notifica
 * al usuario, mediante callbacks, distintos eventos ocurridos.
 *
 * Por otra parte, provee mecanismos para enviar los comandos AT de manera
 * confiable, con capacidad para reintentar el envío, y también notificación
 * del estado final de los estos.
 *
 * Este módulo utiliza internamente una tarea llamada WiFiDataReceivedTask,
 * la cual debe tener prioridad alta y una alarma asociada ya que es periódica
 * con período de 20 milisegundos.
 *
 * El OIL para esta tarea debe ser:
 *
 \verbatim
  TASK WiFiDataReceiveTask {
    PRIORITY = 20;
    ACTIVATION = 1;
    STACK = 1024;
    TYPE = BASIC;
    SCHEDULE = FULL;
  }

  ALARM ActivateWiFiDataReceiveTask {
    COUNTER = SoftwareCounter;
    ACTION = ACTIVATETASK {
        TASK = WiFiDataReceiveTask;
    }
  }
 \endverbatim
 *
 * Para más información acerca de los comandos empleados, consultar la
 * documentación oficial provista por el fabricante.
 * \see http://bbs.espressif.com/viewtopic.php?f=51&t=1022
 *
 */

 /** \defgroup ESP8266 ESP8266
 ** @{ */

/*==================[inclusions]=============================================*/

#include "parser.h"

/*==================[macros]=================================================*/

/** \brief Máxima cantidad de conexiones simultáneas aceptadas por el módulo. */
#define MAX_MULTIPLE_CONNECTIONS	(5)

 /** \defgroup ComandosAT Comandos AT
 ** @{ */

/** \brief Indica que el mensaje a enviar es una cadena de caracteres.
 *
 * Macro que debe ser usada en el campo \p length de la estructura AT_CIPSEND_DATA
 * para indicar que no se conoce la longitud de los datos referenciados por el
 * campo \p content, pero sí se sabe que los datos finalizan con el carácter nulo.
 *
 * Esto facilita el envío de datos para el usuario, ya que no se preocupa por
 * saber la longitud de lo que está enviando, pero si debe enviar datos que
 * contengan caracteres nulos, ahí sí deberá proporcionar la longitud.
 *
 */
#define AT_CIPSEND_ZERO_TERMINATED_CONTENT  (0)

/*==================[typedef]================================================*/

/** \brief Comandos AT actualmente disponibles para ser enviados.
 *
 * Éstos son los comandos AT que pueden ser encolados para ser enviados.
 *
 * Todos los comandos que pueden ser encolados presentan la capacidad para ser
 * reenviados en caso de que el dispositivo receptor se encuentre ocupado.
 * También este módulo permite notificar mediante un callback el envío efectivo
 * de cada comando encolado.
 *
 * No se detallará qué hace cada comando, para ello recurrir a la documentación
 * oficial.
 *
 * \see http://bbs.espressif.com/viewtopic.php?f=51&t=1022
 *
 */
typedef enum {
	AT_RST = 0,
	AT_CWMODE,
	AT_CWSAP,
	AT_CWSAP_CUR,
	AT_CWSAP_DEF,
	AT_CIPMUX,
	AT_CIPSERVER,
	AT_CIPSEND,
	AT_CIPSENDEX,
	AT_CIPSENDBUF,
	AT_COMMAND_SIZE
} AT_Command;


/** \brief Tipo de operación a realizar por un comando AT.
 *
 * Operación que debe ir asociada con cada comando AT. Por ejemplo, indican
 * si hay que leer el valor actual del comando, establecerlo, ver el rango
 * de valores posibles, o directamente ejecutarlo.
 *
 */
typedef enum {
    AT_TYPE_TEST    = 1 << 0,
    AT_TYPE_QUERY   = 1 << 1,
    AT_TYPE_SET     = 1 << 2,
    AT_TYPE_EXECUTE = 1 << 3
} AT_Type;


/** \brief Modos posibles para el comando AT+CWMODE.
 *
 * Modos posibles para el comando AT+CWMODE.
 *
 * Los valores mínimo y máximo son útiles para verificar que el valor
 * que toma una variable de este tipo sea válido.
 *
 */
typedef enum {
    AT_CWMODE_MODE_MIN        = 1, /**< Mínimo valor válido */
    AT_CWMODE_STATION         = 1, /**< Estación */
    AT_CWMODE_SOFTAP          = 2, /**< Soft Access Point */
    AT_CWMODE_SOFTAP_STATION  = 3, /**< Soft Access Point y Estación */
    AT_CWMODE_MODE_MAX        = 3  /**< Máximo valor válido */
} AT_CWMODE_MODE;


/** \brief Acciones a realizar con los datos al encolar un comando AT+CIPSEND.
 *
 * AT+CIPSEND, en cualquiera de sus variantes, se usa para enviar datos. Al
 * encolar un comando de estos, hay que tomar una decisión respecto a los
 * datos a enviar: copiarlos a un buffer interno de este módulo, o al
 * momento de enviarlos tomarlos de una posición de memoria indicada por el
 * usuario.
 *
 */
typedef enum {
    AT_CIPSEND_CONTENT_DONT_COPY    = 0,
    AT_CIPSEND_CONTENT_COPYTOBUFFER = 1
} AT_CIPSEND_CONTENT;


/** \brief Parámetros para las variantes de AT+CIPSEND.
 *
 * Parámetros para los comandos AT+CIPSEND, AT+CIPSENDBUF y AT+CIPSENDEX.
 *
 */
typedef struct {
    char *              content; /**< Puntero a los datos a enviar. */
    uint16_t            length; /**< Cantidad de datos a enviar. \see AT_CIPSEND_ZERO_TERMINATED_CONTENT */
    AT_CIPSEND_CONTENT  copyContentToBuffer; /**< \see AT_CIPSEND_CONTENT */
    uint8_t             connectionID; /**< Número de conexión a la cual se le enviarán los datos. */
} AT_CIPSEND_DATA;


/** \brief Parámetro para el comando AT+CIPMUX */
typedef enum {
    AT_CIPMUX_SINGLE_CONNECTION     = 0,
    AT_CIPMUX_MULTIPLE_CONNECTION   = 1
} AT_CIPMUX_MODE;


/** \brief Acción a llevar a cabo por el comando AT+CIPSERVER (crear o borrar servidor). */
typedef enum {
    AT_CIPSERVER_DELETE = 0,
    AT_CIPSERVER_CREATE
} AT_CIPSERVER_MODE;


/** \brief Parámetros para el comando AT+CIPSERVER. */
typedef struct {
    AT_CIPSERVER_MODE   mode;
    uint16_t            port;
} AT_CIPSERVER_DATA;


/** \brief Tipo de encriptación de la conexión WiFi.
 *
 * Se definen los distintos tipos de encriptación, usados en todos los
 * comandos que requieran este dato, por ejemplo AT+CWSAP.
 *
 */
typedef enum {
    AT_SAP_ENCRYPTION_OPEN          = 0,
    AT_SAP_ENCRYPTION_WPA_PSK       = 2,
    AT_SAP_ENCRYPTION_WPA2_PSK      = 3,
    AT_SAP_ENCRYPTION_WPA_WPA2_PSK  = 4
} AT_SAP_ENCRYPTION;


/** \brief Parámetros para el comando AT+CWSAP. */
typedef struct {
    char *              ssid; /**< Cadena de caracteres terminada en '\0' que indica el SSID del AP. */
    char *              pwd; /**< Cadena de caracteres terminada en '\0' que indica la contraseña del AP. */
    uint8_t             chl; /**< Canal para la WLAN, de 1 a 14 (802.11b/g/n). */
    AT_SAP_ENCRYPTION   ecn; /**< Encriptación a emplear en la comunicación. */
} AT_CWSAP_DATA;


/** @} doxygen end group definition */


/** \brief Tipo de función llamada por el módulo para notificar comando enviado.
 *
 * Este módulo llamará a la función con esta firma, que haya sido asociada con el callback,
 * para notificar que un comando encolado ha sido enviado exitosamente.
 *
 * \param[in] cmd tipo de comando que ha sido enviado.
 *
 */
typedef void (*callbackCommandSentFunction_type)(AT_Command cmd);


/** \brief Estructura que concentra la información de los datos recibidos a través de una conexión abierta. */
typedef PARSER_RESULTS_IPD_T ReceivedDataInfo;

/** \brief Tipo de función llamada por el módulo para notificar la recepción de datos.
 *
 * Este módulo llamará a la función con esta firma, que haya sido asociada con el callback,
 * para notificar la recepción de datos en el módulo WiFi enviados por una estación conectada a la misma red.
 *
 * \param[in] info información que caracteriza los datos recibidos.
 *
 */
typedef void (*callbackDataReceivedFunction_type)(ReceivedDataInfo info);


typedef enum
{
    CONNECTION_STATUS_CLOSE = 0,
    CONNECTION_STATUS_OPEN = 1
} ConnectionStatus;


/** \brief Parámetro del callback ConnectionChanged */
typedef struct
{
    ConnectionStatus newStatus;
    uint8_t connectionID;
} ConnectionInfo;


/** \brief Tipo de función llamada por el módulo para notificar un cambio en alguna conexión.
 *
 * Este módulo llamará a la función con esta firma, que haya sido asociada con el callback,
 * para notificar el cambio en alguna de las conexiones, como podría ser que una conexión que
 * estaba cerrada ahora esté abierta.
 *
 * Sólo tiene sentido si se utiliza el protocolo TCP.
 *
 * \param[in] info información para conocer qué conexión cambió y cuál es su nuevo estado.
 *
 */
typedef void (*callbackConnectionChangedFunction_type)(ConnectionInfo info);


/** \brief Tipo de función llamada por el módulo para notificar un reinicio del módulo WiFi.
 *
 * Este módulo llamará a la función con esta firma, que haya sido asociada con el callback,
 * para notificar la detección de un reinicio en el módulo WiFi, lo que implicará la pérdida
 * de cualquier conexión abierta.
 *
 */
typedef void (*callbackResetDetectedFunction_type)(void);

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Inicializa el módulo ESP8266.
 *
 * Antes de hacer uso de este módulo, es imperativo llamar esta función para
 * inicializar datos y estructuras internas, como así iniciar las tareas que
 * correspondan.
 *
 */
extern void esp8266_init(void);


/** \brief Realiza tareas pendientes del módulo.
 *
 * Para el correcto funcionamiento de este módulo, esta función debe ser llamada dentro del
 * bucle infinito de la tarea de background del programa que hace uso de este.
 * Realiza todo lo relacionado con el envío de los comandos encolados, esperas necesarias
 * para éstos, y los reintentos en los casos que corresponda.
 *
 * Ejemplo de uso:
 * \code{.c}
 * void BackgroundTask()
 * {
 *   // Otro código...
 *   while (1)
 *   {
 *      esp8266_doWork();
 *      // SLEEP function
 *   }
 * }
 * \endcode
 *
 */
extern void esp8266_doWork(void);


/** \brief Encola un comando para ser enviado al módulo WiFi.
 *
 * Agrega a la cola de comandos a enviar el comando, con los parámetros especificados.
 *
 * Cuando se envía un comando encolado, el sistema espera a que el módulo WiFi emita una
 * respuesta dentro de un período de tiempo. Si la respuesta confirma el envío exitoso del
 * comando, se invoca el callback CommandSent. Si informa un error en el comando, se lo
 * descarta. Si informa que no puede procesar el comando porque está ocupado, o directamente
 * no emite ninguna respuesta, se procederá a reenviar el comando, siendo la cantidad máxima
 * de reenvíos previamente establecida. Si se supera la cantidad máxima de reenvíos sin tener
 * éxito, se considera al comando erróneo y se lo descarta.
 *
 * Ejemplos de uso:
 * \code{.c}
 * AT_CIPSERVER_DATA cipserver_data = {AT_CIPSERVER_CREATE, 8080};
 * esp8266_queueCommand(AT_CIPSERVER, AT_TYPE_SET, &cipserver_data);
 *
 * esp8266_queueCommand(AT_CWMODE, AT_TYPE_SET, (void*)AT_CWMODE_SOFTAP);
 * \endcode
 *
 * \param[in] command Comando a enviar.
 * \param[in] type Tipo de operación a llevar a cabo con el comando elegido.
 * \param[inout] parameters
 * Si el parámetro es un tipo de dato simple (un enumerativo, un entero, etc), entonces se
 * debe enviar dicho valor casteado a \p (void*).
 * Si el parámetro es un tipo de dato compuesto, tal como una estructura, se debe enviar
 * un puntero a ésta, con los valores ya configurados. Esta función puede modificar los
 * valores originales del parámetro.
 * \return Si retorna un valor negativo ocurrió un error al encolar el comando, de
 * lo contrario, la operación fue exitosa.
 *
 */
extern int32_t esp8266_queueCommand(AT_Command command, AT_Type type, void* parameters);


/** \brief Especifica el buffer a utilizar para la recepción de datos de usuario.
 *
 * Brinda una manera de especificar qué buffer será usado para almacenar datos que
 * haya recibido el módulo WiFi por parte de un usuario conectado a la red.
 *
 * El buffer NO se maneja de forma circular, por lo tanto, ante la llegada de un conjunto
 * de datos nuevos, éstos se comenzarán a guardar en la primera posición, sobreescribiendo
 * lo que antes estuvo guardado allí. Entonces, cuando llega un mensaje, los datos deben
 * ser procesados inmediatamente por el usuario, o copiados a otro buffer.
 *
 * \param[in] buf buffer con los caracteres a enviar.
 * \param[in] size cantidad de caracteres a enviar.
 *
 */
extern void esp8266_setReceiveBuffer(uint8_t * buf, uint16_t size);


/** \brief Envía datos directamente al módulo WiFi serialmente.
 *
 * Permite enviar datos al dispositivo inmediatamente, dejando de lado la cola de
 * comandos y demás estructuras. Los datos son enviados tal como son dados.
 *
 * Usar esta funcionalidad sólo si no se usa la cola de comandos, caso contrario
 * puede influir en su funcionamiento y el comportamiento del sistema estará
 * indefinido.
 *
 * \param[in] buf buffer con los caracteres a enviar.
 * \param[in] size cantidad de caracteres a enviar.
 * \return Cantidad de caracteres enviados. Si ocurrió algún error, retorna
 * un valor negativo.
 *
 */
extern ssize_t esp8266_writeRawData(void * buf, size_t size);


/** \brief Permite obtener el estado de las conexiones disponibles.
 *
 * \param[in] connectionID identificador de conexión cuyo estado será consultado.
 * \return Estado de la conexión.
 *
 */
extern ConnectionStatus esp8266_getConnectionStatus(uint8_t connectionID);


/** \brief Registra una función para la notificación CommandSent.
 *
 * \see callbackCommandSentFunction_type
 *
 * \param[in] fcn puntero a la función a llamar en caso de que ocurra tal evento.
 *
 */
extern void esp8266_registerCommandSentCallback(callbackCommandSentFunction_type fcn);


/** \brief Registra una función para la notificación DataReceived.
 *
 * \see callbackDataReceivedFunction_type
 *
 * \param[in] fcn puntero a la función a llamar en caso de que ocurra tal evento.
 *
 */
extern void esp8266_registerDataReceivedCallback(callbackDataReceivedFunction_type fcn);


/** \brief Registra una función para la notificación ConnectionChanged.
 *
 * \see callbackConnectionChangedFunction_type
 *
 * \param[in] fcn puntero a la función a llamar en caso de que ocurra tal evento.
 *
 */
extern void esp8266_registerConnectionChangedCallback(callbackConnectionChangedFunction_type fcn);


/** \brief Registra una función para la notificación ResetDetected.
 *
 * \see callbackResetDetectedFunction_type
 *
 * \param[in] fcn puntero a la función a llamar en caso de que ocurra tal evento.
 *
 */
extern void esp8266_registerResetDetectedCallback(callbackResetDetectedFunction_type fcn);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif // ESP8266_H
