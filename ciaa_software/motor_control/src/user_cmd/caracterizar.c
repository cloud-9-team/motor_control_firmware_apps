/*==================[inclusions]=============================================*/

#include "caracterizar.h"
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

const ParserFunctions FUNCTIONS_USER_CARACTERIZAR =
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
	case S0: /* Leer caracteres que comienzan en el mensaje */
		if (newChar == "$CARACTERIZAR="[internalData->readPos])
		{
			internalData->readPos++;
			ret = STATUS_INCOMPLETE;
			if ("$CARACTERIZAR="[internalData->readPos] == '\0')
			{
				internalData->state = S1;
			}
		}
		break;
	case S1: /* Leo el ID del motor, un sólo carácter */
		if (newChar >= '0' && newChar <= '9')
		{
			results->idMotor = newChar - '0';
			internalData->state = S2;
			ret = STATUS_INCOMPLETE;
		}
		break;
	case S2: /* Matcheo de ',' que sucede al ID del motor */
		if (newChar == ',')
		{
			internalData->state = S3;
			results->tiempo = 0;
			ret = STATUS_INCOMPLETE;
		}
		break;
	case S3: /* Matcheo del parámetro TIEMPO */
		if (newChar == '$' && results->tiempo > 0)
		{
			ret = STATUS_COMPLETE;
		}
		else if (newChar >= '0' && newChar <= '9'){
			results->tiempo = (results->tiempo * 10) + (newChar - '0');
			ret = STATUS_INCOMPLETE;
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

