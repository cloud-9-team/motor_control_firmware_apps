/*==================[inclusions]=============================================*/

#include "encoder.h"
#include "chip.h"
#include "os.h"               /* <= operating system header */

/*==================[macros and definitions]=================================*/

#define GPIO_INTERRUPT	(LPC_GPIO_PIN_INT)

/*==================[internal data declaration]==============================*/

typedef struct {
	uint16_t	lastCount;
	uint16_t	currentCount;
	uint8_t		nextIndex;
} encoderSampleData;

typedef struct {
	uint8_t	interruptChannel;
	uint8_t portNumber;
	uint8_t pinNumber;
	uint8_t	gpio_portNumber;
	uint8_t	gpio_pinNumber;
	uint8_t diskSlotsNumber;
} encoderConfigData;


/*==================[internal functions declaration]=========================*/

static inline void encoder_configInterrupt(uint8_t intChannel, uint8_t portNum, uint8_t pinNum);

/*==================[internal data definition]===============================*/

static encoderSampleData encoder[ENCODER_COUNT];

static const encoderConfigData encoder0 = {
		0,			// Interrupt channel
		6, 1,		// P6_1
		3, 0,		// GPIO0 -> GPIO3[0]
		4			// Number of slots in encoder disk
};

static const encoderConfigData encoder1 = {
		1,			// Interrupt channel
		7, 5,		// P7_5
		3, 13,		// TEC_COL2 -> GPIO3[13]
		4			// Number of slots in encoder disk
};

static const encoderConfigData * encoderConfig[ENCODER_COUNT] = {
		&encoder0,
		&encoder1
};

static callBackTimeElapsedFunction_type timeElapsed_callback = 0;

/** \brief Indica si la alarma asociada a la tarea EncoderTask está habilitada o no */
static uint8_t isAlarmRunning = 0;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static inline void encoder_configInterrupt(uint8_t intChannel, uint8_t portNum, uint8_t pinNum){
	Chip_SCU_GPIOIntPinSel(intChannel, portNum, pinNum);
	Chip_PININT_SetPinModeEdge(GPIO_INTERRUPT, PININTCH(intChannel));
	Chip_PININT_EnableIntLow(GPIO_INTERRUPT, PININTCH(intChannel));
}

/*==================[external functions definition]==========================*/

extern void encoder_init(void)
{
	uint8_t encoderID;

	Chip_PININT_Init(GPIO_INTERRUPT);

	for (encoderID = 0; encoderID < ENCODER_COUNT; encoderID++)
	{
	    /* Seteo la funcionalidad GPIO de cada pin */
		Chip_SCU_PinMux(encoderConfig[encoderID]->portNumber, encoderConfig[encoderID]->pinNumber, MD_PUP|MD_EZI|MD_ZI, FUNC0);

		/* Lo configuro como entrada */
		Chip_GPIO_SetDir(LPC_GPIO_PORT, encoderConfig[encoderID]->gpio_portNumber, (1 << encoderConfig[encoderID]->gpio_pinNumber), 0);

        /* Activo interrupción GPIO, cada una en un canal diferente */
		encoder_configInterrupt(encoderConfig[encoderID]->interruptChannel,
								encoderConfig[encoderID]->gpio_portNumber,
								encoderConfig[encoderID]->gpio_pinNumber);
	}

}


extern void encoder_beginCount(uint16_t periodMS)
{

	if (isAlarmRunning)
	{
		CancelAlarm(ActivateEncoderTask);
		isAlarmRunning = 0;
	}

	encoder_resetCount();

	if (!isAlarmRunning)
	{
		SetRelAlarm(ActivateEncoderTask, periodMS, periodMS);
		isAlarmRunning = 1;
	}

}


extern void encoder_setTimeElapsedCallback(callBackTimeElapsedFunction_type fcnPtr)
{
	timeElapsed_callback = fcnPtr;
}


extern uint16_t encoder_getLastCount(uint8_t encoderID)
{
	return (encoderID < ENCODER_COUNT ? encoder[encoderID].lastCount : 0);
}


extern void encoder_resetCount(void)
{
	uint8_t encoderID;

	for (encoderID = 0; encoderID < ENCODER_COUNT; encoderID++){
		encoder[encoderID].currentCount = 0;
	}
}


/** \brief EncoderTask
 *
 * Tarea que efectiviza el conteo actual de interrupciones como el total del
 * período, reinicia la cuenta para el próximo período de conteo, y notifica
 * al usuario la disponibilidad de los datos del último período.
 *
 */
TASK(EncoderTask)
{
	uint8_t encoderID;

	for (encoderID = 0; encoderID < ENCODER_COUNT; encoderID++){
		encoder[encoderID].lastCount = encoder[encoderID].currentCount;
		encoder[encoderID].currentCount = 0;
	}

    /* Si hay un callback registrado, lo llamo */
	if (timeElapsed_callback != 0)
	{
		timeElapsed_callback();
	}

	TerminateTask();
}


ISR(GPIO0_IRQHandler)
{
	encoder[0].currentCount++;
	Chip_PININT_ClearIntStatus(GPIO_INTERRUPT, PININTCH(encoderConfig[0]->interruptChannel));
}


ISR(GPIO1_IRQHandler)
{
	encoder[1].currentCount++;
	Chip_PININT_ClearIntStatus(GPIO_INTERRUPT, PININTCH(encoderConfig[1]->interruptChannel));
}

