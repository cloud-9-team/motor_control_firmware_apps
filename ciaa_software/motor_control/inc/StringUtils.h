#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Proporciona funciones de soporte para manipular cadenas de caracteres. */

 /** \defgroup StringUtils String Utils
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdio.h"

/*==================[macros]=================================================*/

/** \brief Máximo valor para una variable de tipo unsigned char */
#define UCHAR_MAX   (255)

/** \brief Máximo valor para una variable de tipo unsigned short */
#define USHORT_MAX  (65535)

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Conversión de número en punto fijo a string.
 *
 * Convierte el número en punto fijo pasado por argumentos en una cadena de
 * caracteres representativa. Esta cadena se escribe en un buffer.
 *
 * \param[in] valor Número en punto fijo a convertir a string.
 * \param[in] exp Determina la posición del separador de parte entera y	decimal.
 * Equivale a dividir valor por 10^(exp).
 * \param[in] minCantDigitos Mínima cantidad de dígitos a devolver. Si no se
 * alcanza esta cantidad, se agregan ceros a la izquierda.
 * \param[out] str Buffer donde se guardará la cadena resultante.
 * Debe tener espacio suficiente para todos los	números, un carácter
 * separador de parte entera/decimal y el carácter nulo.
 * \return Posición en el buffer luego de escribir la cadena. Apunta al
 * carácter	nulo.
 *
 */
extern unsigned char * fixedPointToString(unsigned short valor, unsigned char exp,	unsigned char minCantDigitos, unsigned char * str);


/** \brief Conversión de número entero decimal a string.
 *
 * Convierte el número decimal pasado por argumentos en una cadena de
 * caracteres representativa. Esta cadena se escribe en un buffer.
 *
 * \param[in] valor Número decimal a convertir a string.
 * \param[in] minCantDigitos Mínima cantidad de dígitos a devolver. Si no se
 * alcanza esta cantidad, se agregan ceros a la izquierda.
 * \param[out] str Cadena de caracteres donde se guardará la cadena resultante.
 * Debe tener espacio suficiente para todos los	números y el carácter nulo.
 * \return Posición en el buffer luego de escribir la cadena. Apunta al
 * carácter	nulo.
 *
 */
extern unsigned char * uintToString(unsigned int valor, unsigned char minCantDigitos, unsigned char * str);


/** \brief Conversión de número hexadecimal a string.
 *
 * Convierte el número hexadecimal pasado por argumentos en una cadena de
 * caracteres representativa. Esta cadena se escribe en un buffer.
 *
 * La cadena de caracteres resultante tendrá el prefijo "0x".
 *
 * \param[in] valor Número en base 16 a convertir a string.
 * \param[in] minCantDigitos Mínima cantidad de dígitos a devolver. Si no se
 * alcanza esta cantidad, se agregan ceros a la izquierda.
 * \param[out] str Buffer donde se guardará la cadena resultante.
 * Debe tener espacio suficiente para todos los	números, el prefijo y el carácter nulo.
 * \return Posición en el buffer luego de escribir la cadena. Apunta al carácter nulo.
 *
 */
extern unsigned char * hexIntToString(unsigned int valor, unsigned char minCantDigitos, unsigned char * str);


/** \brief Comprueba si un carácter se encuentra contenido en una cadena de caracteres.
 *
 * \param[in] str Cadena de caracteres terminada con el carácter nulo donde se buscará \p c.
 * \param[in] c Carácter a buscar.
 * \return Si el carácter se encuentra, devuelve 1, de lo contrario, 0.
 *
 */
extern unsigned char strContainsChar(const unsigned char * str, unsigned char c);


/** \brief
 *
 * The \p strlcpy() function copy strings It is designed to be safer, more	consistent,
 * and less error prone replacements for \p strncpy. Unlike these function, \p strlcpy()
 * take the full size of the buffer (not just the length) and guarantee to
 * NUL-terminate the result (as long as \p size is larger than 0). Note that a byte
 * for the NUL should be included in \p size. Also note that \p strlcpy() only operate on
 * true 'C' strings. This means that \p src must be NUL-terminated.
 *
 * The \p strlcpy() function copies up to \p size - 1 characters from the NUL-terminated
 * string \p src to \p dst, NUL-terminating	the result.
 *
 * \see https://www.freebsd.org/cgi/man.cgi?query=strlcpy&sektion=3
 *
 * \param[out] dest Destination buffer.
 * \param[in] src Source string.
 * \param[in] size Size of the buffer.
 * \return The \p strlcpy() function return the total length of the string it tried to
 * create, that means the length of \p src.
 *
 */
extern size_t strlcpy(char *dest, const char *src, size_t size);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* STRINGUTILS_H_ */
