/*==================[inclusions]=============================================*/

#include "pwm.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/** \brief File descriptor for PWM 0
 *
 * Device path /dev/dio/pwm/0
 */
static int32_t fd_pwm0;

/** \brief File descriptor for PWM 1
 *
 * Device path /dev/dio/pwm/1
 */
static int32_t fd_pwm1;

/** \brief File descriptor for PWM 2
 *
 * Device path /dev/dio/pwm/2
 */
static int32_t fd_pwm2;

/** \brief File descriptor for PWM 3
 *
 * Device path /dev/dio/pwm/3
 */
static int32_t fd_pwm3;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

extern void ciaaPWM_init(void)
{
	uint8_t min_duty_cycle = MIN_DUTY_CYCLE;

	/* Opening of PWM channels and initialization */
	fd_pwm0 = ciaaPOSIX_open("/dev/dio/pwm/0", SCT_PWM_PIN_1A);
	fd_pwm1 = ciaaPOSIX_open("/dev/dio/pwm/1", SCT_PWM_PIN_4A);
	fd_pwm2 = ciaaPOSIX_open("/dev/dio/pwm/2", SCT_PWM_PIN_2A);
	fd_pwm3 = ciaaPOSIX_open("/dev/dio/pwm/3", SCT_PWM_PIN_3A);

    /* Setting initial dutycycle value */
	ciaaPOSIX_write(fd_pwm0, &min_duty_cycle, 1);
	ciaaPOSIX_write(fd_pwm1, &min_duty_cycle, 1);
	ciaaPOSIX_write(fd_pwm2, &min_duty_cycle, 1);
	ciaaPOSIX_write(fd_pwm3, &min_duty_cycle, 1);

}


extern void ciaaPWM_updateMotor(MotorControlData data)
{
	uint8_t zero = 0;

	switch(data.motorID){
	case 0:
		if (data.direction == DIR_FORWARD){
			ciaaPOSIX_write(fd_pwm2, &zero, 1);
			ciaaPOSIX_write(fd_pwm0, &data.dutyCycle, 1); /* T_COL1 para adelante */
		}
		else{
			ciaaPOSIX_write(fd_pwm0, &zero, 1);
			ciaaPOSIX_write(fd_pwm2, &data.dutyCycle, 1); /* T_FIL3 para atrás */
		}
		break;

	case 1:
		if (data.direction == DIR_FORWARD){
			ciaaPOSIX_write(fd_pwm1, &zero, 1);
			ciaaPOSIX_write(fd_pwm3, &data.dutyCycle, 1); /* T_FIL2 para adelante */
		}
		else{
			ciaaPOSIX_write(fd_pwm3, &zero, 1);
			ciaaPOSIX_write(fd_pwm1, &data.dutyCycle, 1); /* T_COL0 para atrás */
		}
		break;

	default:
		/* Invalid motor number */
		break;
	}
}

