/*==================[inclusions]=============================================*/

#include "connection_close.h"
#include "../parser_helper.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void init(Parser* parserPtr);
static ParserStatus tryMatch(Parser* parserPtr, uint8_t newChar);
static ParserStatus tryMatch_internal(	PARSER_DATA_T * internalData,
								PARSER_RESULTS_T * results,
								uint8_t newChar);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

const ParserFunctions FUNCTIONS_AT_CONNECTIONCLOSE =
{
    &init,
    &tryMatch,
    &parser_default_deinit
};

/*==================[internal functions definition]==========================*/

static void init(Parser* parserPtr)
{
    PARSER_DATA_T * p;

    if (parserPtr->data == 0)
        parserPtr->data = (void*) malloc(sizeof(PARSER_DATA_T));

    if (parserPtr->results == 0)
        parserPtr->results = (void*) malloc(sizeof(PARSER_RESULTS_T));

    p = parserPtr->data;
    p->state = S0;
    p->readPos = 0;

    parserPtr->status = STATUS_INITIALIZED;
}


static ParserStatus tryMatch(Parser* parserPtr, uint8_t newChar){
    parserPtr->status = tryMatch_internal(parserPtr->data, parserPtr->results, newChar);
    if (parserPtr->status == STATUS_NOT_MATCHES)
    {
        parserPtr->status = tryMatch_internal(parserPtr->data, parserPtr->results, newChar);
    }
    return parserPtr->status;
}

static ParserStatus tryMatch_internal(PARSER_DATA_T * internalData,
                                      PARSER_RESULTS_T * results,
                                      uint8_t newChar)
{
    ParserStatus ret = STATUS_NOT_MATCHES;

    switch(internalData->state){
        case S0: /* Matcheo de ID de conexión, de 0 a 4 */
            if (newChar >= '0' && newChar <= '4'){
                results->connectionID = newChar - '0';
                internalData->state = S1;
                ret = STATUS_INCOMPLETE;
            }
            break;
        case S1: /* Matcheo de la cadena ",CLOSED" que debe sucederse */
            if (newChar == ",CLOSED"[internalData->readPos]){
                internalData->readPos++;
                if (",CLOSED"[internalData->readPos] == '\0'){
                    ret = STATUS_COMPLETE;
                }
                else{
                    ret = STATUS_INCOMPLETE;
                }
            }
            break;
        default:
        	break;
    }

	if (ret == STATUS_NOT_MATCHES || ret == STATUS_COMPLETE){
        internalData->state = S0;
        internalData->readPos = 0;
    }

    return ret;
}

/*==================[external functions definition]==========================*/
