#ifndef __TIMER_H_
#define __TIMER_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Permite la generación de retardos arbitrarios.
 *
 * El funcionamiento de este módulo se basa en la utilización de un periférico
 * que genera interrupciones periódicas, y la utilidad de estas interrupciones
 * reside en la capacidad de generar retardos (delay) por una cantidad de tiempo
 * dada.
 *
 * Estos retardos son utilizados en otros módulos para generar timeouts, o
 * esperar a que un dispositivo se inicialice, como lo es en el caso del módulo WiFi.
 *
 */

 /** \defgroup Timer Timer
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdlib.h"

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Configura y habilita el periférico.
 *
 * Configura el periférico Repetitive Interrupt Timer (RIT), para que genere
 * una interrupción cada un milisegundo.
 *
 */
extern void timer_init(void);


/** \brief Detiene el timer asociado.
 *
 * Detiene el RIT, deshabilitando la generación de delays.
 *
 * Si al momento de detener hay un delay pendiente, cesa su espera y permite
 * que la función que lo solicitó retome el control.
 *
 */
extern void timer_stop(void);


/** \brief Delay
 *
 * Esta función no retorna el control hasta que no haya transcurrido el tiempo
 * deseado.
 *
 * Si se detiene el timer con \p timer_stop(), esta función retorná.
 *
 * \param[in] milisecs Cantidad de milisegundos durante los cuales se esperará.
 *
 */
extern void timer_delay_ms(uint32_t milisecs);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* __TIMER_H_ */
