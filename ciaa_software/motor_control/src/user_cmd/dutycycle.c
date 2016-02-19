/*==================[inclusions]=============================================*/

#include "dutycycle.h"
#include "../parser_helper.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

static void init(Parser* parserPtr);
static ParserStatus tryMatch(Parser* parserPtr, uint8_t newChar);
static ParserStatus tryMatch_internal(	PARSER_DATA_T * internalData,
								PARSER_RESULTS_T * results,
								uint8_t newChar);
static uint8_t parseReceivedDutyCycle(PARSER_RESULTS_T * results);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

const ParserFunctions FUNCTIONS_DUTYCYCLE =
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
    p->dataLength = 0;

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
		case S0: /* Carácter separador entre el valor anterior y
					el Ciclo de Trabajo */
			if (newChar == '%'){
				internalData->state = S1;
				ret = STATUS_INCOMPLETE;
			}
			break;
		case S1: /* Lee la longitud de la cantidad de caracteres que
					contiene el mensaje siguiente: de 1 a 9 caracteres */
			if (newChar >= '1' && newChar <= '9'){
				internalData->dataLength = newChar - '0';
				internalData->state = S2;

				ret = STATUS_INCOMPLETE;
			}
			break;
		case S2: /* Lee el número de motor: de 0 a 9. */
			if (newChar >= '0' && newChar <= '9'){

				results->motorID = newChar - '0';
				results->dutyCycle = 0;

				internalData->dataLength--;
				internalData->state = S3;

				ret = STATUS_INCOMPLETE;
			}
			break;

		case S3: /* Se leen caracteres hasta leer la cantidad
					anteriormente especificada. Estos representan
					el ciclo de trabajo.
					Formato: de 0 a 200, de 100 a 200 se incrementa
					en un sentido, de 100 a 0 en el otro. */
			if (newChar >= '0' && newChar <= '9' && internalData->dataLength > 0){

				results->dutyCycle = (results->dutyCycle * 10) + newChar - '0';

				internalData->dataLength--;
				if (internalData->dataLength == 0){

					if (parseReceivedDutyCycle(results)){
						ret = STATUS_COMPLETE;
						internalData->state = S0;
					}
					else
						ret = STATUS_NOT_MATCHES;

				}
				else
					ret = STATUS_INCOMPLETE;
			}
			break;
		default:
			break;
	}

	if (ret == STATUS_NOT_MATCHES || ret == STATUS_COMPLETE)
    {
		internalData->state = S0;
	}

    return ret;
}

static uint8_t parseReceivedDutyCycle(PARSER_RESULTS_T * results){
	if (results->dutyCycle >= 0 && results->dutyCycle <= 200){
		if (results->dutyCycle < 100){
			results->direction = DIR_BACKWARD;
			results->dutyCycle = 100 - results->dutyCycle;
		}
		else{
			results->direction = DIR_FORWARD;
			results->dutyCycle -= 100;
		}

		return 1;
	}

	return 0;
}

/*==================[external functions definition]==========================*/
