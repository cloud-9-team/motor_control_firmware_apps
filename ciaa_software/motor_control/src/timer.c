/*==================[inclusions]=============================================*/

#include "timer.h"
#include "chip.h"
#include "os.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static uint32_t tickCount = 0;
static uint8_t isEnabled = 0;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


extern void timer_init(void)
{
	Chip_RIT_Init(LPC_RITIMER);
	Chip_RIT_SetTimerInterval(LPC_RITIMER, 1);
	Chip_RIT_Enable(LPC_RITIMER);
	isEnabled = 1;
}


extern void timer_stop(void)
{
	Chip_RIT_Disable(LPC_RITIMER);
	isEnabled = 0;
}


extern void timer_delay_ms(uint32_t milisecs)
{
	uint32_t ticksComienzo = tickCount;

	while ((uint32_t)(tickCount - ticksComienzo) < milisecs && isEnabled)
	{
		__WFI();
	}
}


ISR(RIT_IRQHandler)
{
	tickCount++;
	Chip_RIT_ClearInt(LPC_RITIMER);
}

