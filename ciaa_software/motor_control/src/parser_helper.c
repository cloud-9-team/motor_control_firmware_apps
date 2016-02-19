/*==================[inclusions]=============================================*/

#include "parser_helper.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

extern void parser_default_deinit(Parser* parserPtr)
{
    free(parserPtr->data);
    free(parserPtr->results);

    parserPtr->status = STATUS_UNINITIALIZED;
}
