/* Copyright 2014, Mariano Cerdeiro
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \brief Circular Buffer Library sources
 **
 ** This library provides a circular buffer
 **
 **/

/** \addtogroup CIAA_Firmware CIAA Firmware
 ** @{ */
/** \addtogroup Libs CIAA Libraries
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaLibs_CircBufExt.h"
#include "ciaaPOSIX_stdio.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

extern size_t ciaaLibs_circBufWriteTo(ciaaLibs_CircBufType * cbuf, int32_t filDes, size_t nbytes)
{
   size_t rawCount;

   /* the tail of the circular buffer my be changed, therefore it has to be
    * read only once */
   size_t tail = cbuf->tail;

   /* if the users tries to read to much data, only available data will be
    * provided */
   if (nbytes > ciaaLibs_circBufCount(cbuf, tail))
   {
      nbytes = ciaaLibs_circBufCount(cbuf, tail);
   }

   /* check if data to be read */
   if (nbytes > 0)
   {
      rawCount = ciaaLibs_circBufRawCount(cbuf, tail);

      /* check if all data can be read without wrapping */
      if (nbytes <= rawCount)
      {
         ciaaPOSIX_write(filDes, ciaaLibs_circBufReadPos(cbuf), nbytes);
      }
      else
      {
         ciaaPOSIX_write(filDes, ciaaLibs_circBufReadPos(cbuf), rawCount);
         ciaaPOSIX_write(filDes, (void*)(&cbuf->buf[0]), nbytes-rawCount);
      }

   }

   return nbytes;
} /* end ciaaLibs_circBufWriteTo */

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

