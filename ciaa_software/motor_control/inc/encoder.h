#ifndef __ENCODER_H_
#define __ENCODER_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Administra las entradas conectadas a los encoders y captura sus datos.
 *
 * Este módulo configura los pines conectados a los encoders como entradas y
 * mide las interrupciones que ocurren en cada uno de ellos. Teniendo una base de
 * tiempo, es posible traducir la cantidad de interrupciones en unidades de
 * velocidad, útiles para el usuario.
 *
 * El proceso de captura de interrupciones es periódico, siendo a grandes rasgos
 * así:
 *
 * 1. Cuenta interrupciones durante un tiempo especificado.
 * 2. Al transcurrir ese tiempo, notifica a quien hace uso del módulo, y le
 * permite obtener la cantidad de interrupciones contabilizadas en el período.
 * 3. Luego, se reinicia la cuenta, y vuelve a comenzar el período, regresando a 1.
 *
 * Este módulo utiliza internamente una tarea llamada EncoderTask,
 * la cual debe tener prioridad media y una alarma asociada ya que es periódica
 * con período que varía pero su máximo está determinado por el valor máximo del
 * contador de la alarma.
 *
 * El código OIL para esta tarea debe ser:
 *
 \verbatim
  TASK EncoderTask {
    PRIORITY = 10;
    ACTIVATION = 1;
    STACK = 1024;
    TYPE = BASIC;
    SCHEDULE = FULL;
  }

  ALARM ActivateEncoderTask {
    COUNTER = SoftwareCounter;
    ACTION = ACTIVATETASK {
        TASK = EncoderTask;
    }
  }
 \endverbatim
 *
 */

 /** \defgroup Encoder Encoder
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdio.h"  /* <= device handler header */

/*==================[macros]=================================================*/

/** \brief Cantidad de encoders conectados. */
#define ENCODER_COUNT   (2)

/*==================[typedef]================================================*/

/** \brief Tipo de función llamada por el módulo para notificar que transcurrió un período de conteo.
 *
 * Este módulo llamará a la función con esta firma, que haya sido asociada con el callback,
 * para notificar que un período de conteo ha transcurrido, siendo el período el configurado por el
 * usuario.
 *
 */
typedef void (*callBackTimeElapsedFunction_type)(void);


/** \brief Unidades para los datos que podría utilizar este módulo. Actualmente, sólo se usa SPEED_TYPE_INTERRUPTS. */
typedef enum {
    SPEED_TYPE_RPM          = 0,
    SPEED_TYPE_INTERRUPTS   = 1
} SpeedType;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Configuración inicial del módulo.
 *
 * Antes de usar cualquier otra función de este módulo, se debe llamar a esta
 * función.
 *
 * Lo que hace es configurar como entrada los pines donde están conectados los encoders,
 * activar interrupciones para cada uno de estos pines, que son disparadas por el flanco
 * descendente de la señal de entrada.
 *
 */
extern void encoder_init(void);


/** \brief Inicia el conteo periódico de interrupciones.
 *
 * Esta función hace que el módulo empiece a contar interrupciones hasta que transcurra el
 * tiempo especificado en el parámetro. Una vez que suceda esto, el valor de la cantidad
 * de interrupciones pasará a estar disponible mediante \p encoder_getLastCount(), y se
 * notificará al usuario, si es que registró el callback, acerca de este suceso.
 *
 * Una vez finalizado el proceso anterior, la cuenta se reinicia y se repite el ciclo.
 * Hasta que se tenga la nueva cuenta de interrupciones, la cantidad total correspondiente
 * al período anterior seguirá siendo accesible mediante \p encoder_getLastCount().
 *
 * Si anteriormente se ha llamado \p encoder_beginCount(), un nuevo llamado de esta función
 * forzará al reinicio del conteo anterior, e inmediatamente comenzará la nueva cuenta, con
 * el nuevo período escogido.
 *
 * \param[in] periodMS Tiempo durante el cual se medirán interrupciones antes de notificar
 * al usuario. El valor máximo que puede tomar depende del valor máximo del contador
 * asociado a la alarma que activa la tarea periódica.
 *
 */
extern void encoder_beginCount(uint16_t periodMS);


/** \brief Registra una función para la notificación TimeElapsed.
 *
 * La función registrada será llamada al ocurrir el evento.
 *
 * \see callbackTimeElapsedFunction_type
 *
 * \param[in] fcnPtr puntero a la función a llamar en caso de que ocurra tal evento.
 *
 */
extern void encoder_setTimeElapsedCallback(callBackTimeElapsedFunction_type fcnPtr);


/** \brief Obtiene la cantidad de interrupciones generadas por el encoder contadas durante un período.
 *
 * \param[in] encoderID Identificador de encoder del cual se desea obtener la cantidad
 * de interrupciones computadas. Si hay dos encoders, los IDs posibles serán 0 y 1.
 * \return Cantidad de interrupciones contadas en un período de tiempo para el encoder
 * especificado.
 *
 */
extern uint16_t encoder_getLastCount(uint8_t encoderID);


/** \brief Reinicia el conteo actual de interrupciones para todos los encoders. */
extern void encoder_resetCount(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* __ENCODER_H_ */
