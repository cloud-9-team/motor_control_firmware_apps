/*==================[inclusions]=============================================*/

#include "StringUtils.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

static const char hex_charset[] = "0123456789ABCDEF";

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** \brief Cuenta la cantidad de dígitos de un entero, interpretándolo como decimal.
 *
 * \param[in] number Entero cuya cantidad de dígitos será contada.
 * \return Cantidad de dígitos.
 *
 */
static unsigned char digitCount(unsigned int number){
	unsigned char count = 0;

	do {
		count++;
		number /= 10;
	} while (number != 0);

	return count;
}


/** \brief Cuenta la cantidad de dígitos de un entero, interpretándolo como hexadecimal.
 *
 * \param[in] number Número hexadecimal cuya cantidad de dígitos será contada.
 * \return Cantidad de dígitos.
 *
 */
static unsigned char hexDigitCount(unsigned int number)
{
	unsigned char count = 0;

	do {
		count++;
		number >>= 4;
	} while (number != 0);

	return count;
}

/*==================[external functions definition]==========================*/

unsigned char * fixedPointToString(unsigned short valor,
								   unsigned char exp,
								   unsigned char minCantDigitos,
								   unsigned char * str){
    unsigned char * p;

    p = uintToString(valor, minCantDigitos, str);
	p++;
    str = p;
    for (minCantDigitos = 0; minCantDigitos <= exp; minCantDigitos++) *(--str + 1) = *str;
    *str = '.';
    return p;
}


unsigned char * uintToString(unsigned int valor, unsigned char minCantDigitos, unsigned char * str){
    unsigned char cantDigitos = 1;
    char i;

    cantDigitos = digitCount(valor);

    if (minCantDigitos > cantDigitos)
        cantDigitos = minCantDigitos;

    for (i = cantDigitos; i > 0; i--){
        str[i-1] = '0' + valor % 10;
        valor /= 10;
    }

    str += cantDigitos;
    *str = '\0';

    return str;
}


unsigned char * hexIntToString(unsigned int valor, unsigned char minCantDigitos, unsigned char * str){
    unsigned char cantDigitos = 1;
    char i;

    *str++ = '0';
    *str++ = 'x';

    cantDigitos = hexDigitCount(valor);

    if (minCantDigitos > cantDigitos)
        cantDigitos = minCantDigitos;

    for (i = cantDigitos; i > 0; i--){
        str[i-1] = hex_charset[valor & 15];
        valor >>= 4;
    }

    str += cantDigitos;
    *str = '\0';

    return str;
}


unsigned char strContainsChar(const unsigned char * str, unsigned char c){
	while (*str != '\0'){
		if (*str == c)
			return 1;
		str++;
	}
	return 0;
}


size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t i;

	/* Debo copiar a dest como mucho size-1 caracteres de src, o hasta llegar a su fin */
	size--;
	for (i = 0; i < size && src[i] != '\0'; i++)
	{
		dest[i] = src[i];
	}

	/* Siempre se finaliza la cadena con el carácter nulo */
	dest[i] = '\0';

	/* Cuento los caracteres que restan de src, sin copiarlos a dest */
	while(src[i] != '\0')
		i++;

	/* Retorno la longitud de la cadena src */
	return i;
}

/*==================[end of file]============================================*/
