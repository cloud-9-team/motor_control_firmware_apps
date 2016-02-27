#ifndef _DEBUG_LOGGER_H_
#define _DEBUG_LOGGER_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Control de USB UART para debug.
 *
 * La EDU-CIAA nos proporciona a través de la conexión USB el acceso a una UART.
 * Ésta, para propósitos de debugging, es realmente útil, pero una vez que el
 * desarrollo está finalizado, y no se posee acceso a una computadora para leer
 * su salida, es conveniente deshabilitarla para ahorrar aunque sea un poco en
 * el consumo.
 *
 * Este módulo provee funciones para escribir y leer de dicha UART.
 * Cuando una macro está definida (\p LOGGING_ALLOWED), su funcionamiento es
 * normal. Si dicha macro no está definida, entonces la UART será deshabilitada,
 * y además el cuerpo de todas las funciones que expone no será compilado,
 * por lo cual un compilador inteligente optimizará los llamados a esas
 * funciones, eliminándolos.
 *
 */

 /** \defgroup DebugLogger Debug Logger
 ** @{ */
/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_string.h"

/*==================[macros]=================================================*/

#ifndef LOGGING_ALLOWED
    /** \brief Macro que define si se realiza el loggeo o se lo deshabilita. */
    #define LOGGING_ALLOWED
#endif // LOGGING_ALLOWED

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Inicializa este módulo.
 *
 * Esta función debe ser llamada antes de utilizar cualquier característica
 * ofrecida.
 *
 * Si \p LOGGING_ALLOWED no está definida, deshabilitará el dispositivo asociado.
 *
 */
extern void logger_init(void);


/** \brief Escribe datos en la salida.
 *
 * \param[in] data Buffer del que se tomarán los elementos a enviar.
 * \param[in] size Cantidad de elementos a escribir del buffer.
 *
 */
extern void logger_print_data(uint8_t * data, size_t size);


/** \brief Escribe una cadena de caracteres en la salida.
 *
 * \param[in] str Cadena de caracteres a escribir, terminada con el carácter nulo.
 *
 */
extern void logger_print_string(const char * str);


/** \brief Lee datos de la entrada.
 *
 * \param buf Buffer donde se guardarán los datos leídos.
 * \param size Tamaño del buffer, cantidad de elementos que puede almacenar.
 * \return Si es negativo, ocurrió un error al leer. Si es positivo, indica la
 * cantidad de caracteres leídos de la entrada.
 *
 */
extern ssize_t logger_read_input(void * buf, size_t size);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* _DEBUG_LOGGER_H_ */
