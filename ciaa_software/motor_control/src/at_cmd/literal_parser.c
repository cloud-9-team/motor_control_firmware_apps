/*==================[inclusions]=============================================*/

#include "literal_parser.h"
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

const ParserFunctions FUNCTIONS_LITERAL_PARSER =
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
    p->string = 0;
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

	if (newChar == internalData->string[internalData->readPos]){
		internalData->readPos++;
		if (internalData->string[internalData->readPos] == '\0'){
			ret = STATUS_COMPLETE;
			internalData->readPos = 0;
		}
		else{
			ret = STATUS_INCOMPLETE;
		}
	}
	else if (internalData->readPos != 0)
	{
		internalData->readPos = 0;
	}

	return ret;
}


/*==================[external functions definition]==========================*/

extern void literalParser_setStringToMatch(Parser * parserPtr, const char * str)
{
	((PARSER_DATA_T*)parserPtr->data)->string = str;
	((PARSER_DATA_T*)parserPtr->data)->readPos = 0;
}
