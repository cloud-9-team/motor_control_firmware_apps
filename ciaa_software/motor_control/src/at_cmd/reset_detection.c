/*==================[inclusions]=============================================*/

#include "reset_detection.h"
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

const ParserFunctions FUNCTIONS_AT_RESET_DETECTION =
{
    &init,
    &tryMatch,
    &parser_default_deinit
};

/*==================[internal functions definition]==========================*/

static void init(Parser* parserPtr)
{
    PARSER_DATA_T * p;
    Parser dummy = INITIALIZER_LITERAL_PARSER;

    if (parserPtr->data == 0)
        parserPtr->data = (void*) malloc(sizeof(PARSER_DATA_T));

    if (parserPtr->results == 0)
        parserPtr->results = (void*) malloc(sizeof(PARSER_RESULTS_T));

    p = parserPtr->data;

    /* Inicialización de parser de la siguiente manera porque fue alocado dinámicamente */
    memcpy(&p->strParser, &dummy, sizeof(Parser));

    parser_init(&p->strParser);
    literalParser_setStringToMatch(&p->strParser, "rst cause:");

    p->state = S0;

    parserPtr->status = STATUS_INITIALIZED;
}


static ParserStatus tryMatch(Parser* parserPtr, uint8_t newChar){
    parserPtr->status = tryMatch_internal(parserPtr->data, parserPtr->results, newChar);
    return parserPtr->status;
}

static ParserStatus tryMatch_internal(PARSER_DATA_T * internalData,
                                      PARSER_RESULTS_T * results,
                                      uint8_t newChar)
{
    ParserStatus ret = STATUS_NOT_MATCHES;

    switch(internalData->state){
        case S0: /* Matcheo de la cadena "rst cause:" que se asume al comienzo del mensaje de reset */
            if (parser_tryMatch(&internalData->strParser, newChar) == STATUS_COMPLETE)
            {
                literalParser_setStringToMatch(&internalData->strParser, "\r\nready");
                internalData->state = S1;
                internalData->skippedChars = 0;
                ret = STATUS_INCOMPLETE;
            }
            break;
        case S1: /* Matcheo de la cadena "\r\nready" que debe sucederse */
            if (parser_tryMatch(&internalData->strParser, newChar) == STATUS_COMPLETE)
            {
                ret = STATUS_COMPLETE;
            }
            else
            {
                /* Va a intentar encontrar la cadena anterior durante 500 caracteres como máximo,
                   una vez superada esa cantidad, se asume que no se trataba de un mensaje de reset */
                internalData->skippedChars++;
                if (internalData->skippedChars >= 500)
                {
                    ret = STATUS_NOT_MATCHES;
                }
                else
                {
                    ret = STATUS_INCOMPLETE;
                }
            }
            break;
        default:
        	break;
    }

	if ((ret == STATUS_NOT_MATCHES || ret == STATUS_COMPLETE) && internalData->state != S0){
        internalData->state = S0;
        literalParser_setStringToMatch(&internalData->strParser, "rst cause:");
    }

    return ret;
}

/*==================[external functions definition]==========================*/
