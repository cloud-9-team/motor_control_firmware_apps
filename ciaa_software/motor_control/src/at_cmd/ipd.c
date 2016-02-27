/*==================[inclusions]=============================================*/

#include "ipd.h"
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

const ParserFunctions FUNCTIONS_AT_IPD =
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
	p->writePos = 0;

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
		if (newChar == "+IPD,"[internalData->readPos]){
			internalData->readPos++;
			ret = STATUS_INCOMPLETE;
			if ("+IPD,"[internalData->readPos] == '\0'){
				internalData->state = S1; /* TODO Si CIPMUX es 0, saltar a S3 */
			}
		}
		break;
	case S1: /* Matcheo de ID de conexión, de 0 a 4 */
		if (newChar >= '0' && newChar <= '4'){
			results->connectionID = newChar - '0';
			internalData->state = S2;
			ret = STATUS_INCOMPLETE;
		}
		break;
	case S2: /* Matcheo de ',' que sucede al ID de conexión */
		if (newChar == ','){
			internalData->state = S3;
			results->payloadLength = 0;
			ret = STATUS_INCOMPLETE;
		}
		break;
	case S3: /* Matcheo de longitud del mensaje (payload) */
		if (newChar == ':' && results->payloadLength > 0){
			internalData->state = S4;
			ret = STATUS_INCOMPLETE;
		}
		else if (newChar >= '0' && newChar <= '9'){
			results->payloadLength = (results->payloadLength * 10) + (newChar - '0');
			ret = STATUS_INCOMPLETE;
		}
		break;
	case S4: /* Lectura del payload */
		/* Escribe sólo si hay lugar en el buffer. */
		/* Si se llena, se continua leyendo caracteres, sin almacenarlos, hasta leer
		 * la cantidad especificada por la longitud del mensaje (payloadLength).
		 * El usuario debe comprobar si el buffer tuvo suficiente espacio para
		 * guardar todo el mensaje.
		 */
		if (internalData->writePos < results->bufferLength){
			results->buffer[internalData->writePos] = newChar;
		}

		internalData->writePos++;
		if (results->payloadLength == internalData->writePos){
			ret = STATUS_COMPLETE;
		}
		else{
			ret = STATUS_INCOMPLETE;
		}

		break;
	default:
		break;
	}


	if (ret == STATUS_NOT_MATCHES || ret == STATUS_COMPLETE){
		internalData->state = S0;
		internalData->readPos = 0;
		internalData->writePos = 0;
	}

	return ret;
}

/*==================[external functions definition]==========================*/

extern uint8_t parser_ipd_isDataBeingSaved(Parser* parserPtr)
{
	return ((parserPtr->status == STATUS_INCOMPLETE) && (((PARSER_DATA_T*)parserPtr->data)->state == S4));
}


extern void parser_ipd_setBuffer(Parser* parserPtr, uint8_t * buf, uint16_t sz)
{
	((PARSER_RESULTS_T*)parserPtr->results)->bufferLength = sz;
    ((PARSER_RESULTS_T*)parserPtr->results)->buffer = buf;
}
