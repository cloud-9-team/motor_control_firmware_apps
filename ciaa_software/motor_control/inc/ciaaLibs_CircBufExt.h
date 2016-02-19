#ifndef _CIAALIBS_CIRCBUF_EXT_H_
#define _CIAALIBS_CIRCBUF_EXT_H_
/** \brief Circular Buffer Library header extended
 **
 ** This library extends the circular buffer provided.
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Libs CIAA Libraries
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaLibs_CircBuf.h"

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/** \brief realiza la resta en aritmética modular
 **
 ** Esta función realiza la siguiente operación: (a - b) % mod.
 **
 ** Función auxiliar para la función ciaaLibs_circBufDeleteLastN().
 **
 ** \param[in] a minuendo de la resta
 ** \param[in] b sustraendo de la resta
 ** \param[in] mod módulo bajo el cual se llevará a cabo la operación
 **/
#define mod_subtract(a, b, mod)     \
   ( ( (a) >= (b) ) ?               \
     ( (a) - (b) ) :                \
     ( mod - (b) + (a) ) )

/** \brief borra los últimos nbytes del buffer
 **
 ** Esta función borra los últimos nbytes agregados al buffer,
 ** actualizando el tail adecuadamente.
 **
 ** \param[inout] cbuf puntero al buffer circular que será actualizado
 ** \param[in] nbytes cantidad de bytes a eliminar al final del buffer
 **/
#define ciaaLibs_circBufDeleteLastN(cbuf, nbytes)       \
   ( (cbuf)->tail = mod_subtract((cbuf)->tail, (nbytes), (cbuf)->size + 1) )

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief lee datos del buffer circular y los escribe en un dispositivo
 **
 ** Lee la cantidad especificada de bytes del buffer circular, los escribe en
 ** el dispositivo deseado en base a su descriptor, haciendo uso de la función
 ** ciaaPOSIX_write() de POSIX.
 **
 ** NO borra los bytes leídos del buffer, es decir, no modifica el head.
 **
 ** \param[inout] cbuf pointer to the circular buffer
 ** \param[in]    filDes descriptor del dispositivo donde se realizará la escritura
 ** \param[in]    nbytes cantidad de bytes a leer
 ** \returns cantidad de bytes leídos
 **/
extern size_t ciaaLibs_circBufWriteTo(ciaaLibs_CircBufType * cbuf, int32_t filDes, size_t nbytes);

/*==================[cplusplus]==============================================*/
#ifdef __cplusplus
}
#endif
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _CIAALIBS_CIRCBUF_EXT_H_ */

