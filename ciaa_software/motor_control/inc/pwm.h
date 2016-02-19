#ifndef __PWM_H_
#define __PWM_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Maneja las salidas PWM.
 *
 * Administra los distintos dispositivos PWM. En total se utilizan cuatro, dos
 * por motor, cada uno de éstos para desplazarse en un sentido distinto.
 * Expone la posibilidad de modificar el ciclo de trabajo de las señales, a
 * partir del número de motor, la dirección del movimiento y el valor del ciclo
 * de trabajo en sí.
 *
 */

 /** \defgroup PWM PWM
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdio.h"  /* <= device handler header */

/*==================[macros]=================================================*/

/** \brief COUT13 controls 1A pin in L293D -> T_COL1 (P7_4) */
#define SCT_PWM_PIN_1A		(13)

/** \brief COUT3  controls 2A pin in L293D -> T_FIL3 (P4_3) */
#define SCT_PWM_PIN_2A		(3)

/** \brief COUT0  controls 3A pin in L293D -> T_FIL2 (P4_2) */
#define SCT_PWM_PIN_3A		(0)

/** \brief COUT10 controls 4A pin in L293D -> T_COL0 (P1_5) */
#define SCT_PWM_PIN_4A		(10)

/** \brief Valor de ciclo de trabajo establecido para cada salida PWM al inicializar el módulo. */
#define MIN_DUTY_CYCLE		(0)

/*==================[typedef]================================================*/

/** \brief Sentido de movimiento del motor. */
typedef enum {DIR_FORWARD, DIR_BACKWARD} MotorDirection;


/** \brief Parámetros necesarios para controlar un motor.
 *
 * Todos estos elementos son necesarios para escoger cuáles señales PWM hay que
 * manejar en determinado momento.
 *
 */
typedef struct {
    uint8_t			motorID;
	uint8_t			dutyCycle;
	MotorDirection	direction;
} MotorControlData;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Inicializa el módulo PWM.
 *
 * Esta función realiza las tareas de configuración de los pines a usar de
 * salida para las señales PWM.
 *
 * Si se emplea este módulo, entonces esta función debe ser llamada antes que
 * cualquier otra (de este módulo).
 *
 */
extern void ciaaPWM_init(void);


/** \brief Actualiza el estado de un motor.
 *
 * Establece el estado del motor especificado de acuerdo a la dirección y
 * ciclo de trabajo deseados.
 *
 * El ciclo de trabajo directamente afectará el ciclo de trabajo de la señal
 * PWM, mientras que la dirección permite escoger cuál de las dos señales
 * PWM por motor debe habilitarse, y cuál otra deshabilitarse.
 *
 * \param[in] data Información del estado buscado para el motor.
 *
 */
extern void ciaaPWM_updateMotor(MotorControlData data);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* __PWM_H_ */
