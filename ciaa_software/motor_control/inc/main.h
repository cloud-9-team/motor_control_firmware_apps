#ifndef __MAIN_H_
#define __MAIN_H_

/** \brief Control y caracterización de motores
 *
 * Este proyecto ha sido desarrollado para la cátedra Taller de Proyecto 1,
 * de la carrera Ingeniería en Computación, dictada por la Universidad
 * Nacional de La Plata.
 *
 *
 * Profesor titular:    Ing. Sager, Gerardo Enrique.\n
 * Profesor adjunto:    Ing. Juarez, José María\n
 * Ayudante diplomado:  Ing. Aróztegui, Walter José\n
 *
 * \date Segundo semestre del 2015
 * \author
 * Ailán, Julián\n
 * Bouche, Federico\n
 * Hourquebie, Lucas\n
 * Liotta, Emiliano\n
 *
 */

/** \defgroup MotorControl Motor Control
 ** @{ */


/** \brief Programa principal.
 *
 * Punto de arranque del programa. Aquí se inicializan todos los demás módulos,
 * se los configura, se registran los callbacks, cuyas funciones a llamar se
 * encuentran definidas aquí.
 *
 * Contiene la tarea de background.
 *
 */

/** \defgroup Main Programa principal
 ** @{ */

/*==================[inclusions]=============================================*/

/*==================[macros]=================================================*/

/** \brief Bitmask para acceder a bit que controla el pin ENABLE12 del puente H. */
#define ENABLE12 	0x0040		// 00000000 01000000

/** \brief Bitmask para acceder a bit que controla el pin ENABLE34 del puente H. */
#define ENABLE34    0x0080		// 00000000 10000000

/** \brief Bitmask para acceder a bit que controla el pin CH_PD del módulo WiFi. */
#define ESP8266_EN	0x0100		// 00000010	00000000

/** \brief Bitmask para acceder a bit que controla el pin RST del módulo WiFi. */
#define ESP8266_RST 0x0200		// 00000100	00000000

/** \brief Tamaño en bytes del buffer usado para la recepción de datos que fueron recibidos por el módulo WiFi. */
#define RECEIVE_BUFFER_LENGTH   	(2048)

/** \brief Cantidad de motores que se manejarán */
#define MOTOR_COUNT					(2)

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* __MAIN_H_ */
