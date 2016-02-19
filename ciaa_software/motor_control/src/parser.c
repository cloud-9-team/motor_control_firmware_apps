/*==================[inclusions]=============================================*/

#include "parser.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

extern void parser_initModule(void){

}

extern int32_t parser_init(Parser * parser)
{
    if (parser != 0 && parser->functions != 0)
    {
        parser->functions->init(parser);
        if (parser->data != 0 && parser->results != 0)
            return 1;
    }

    parser->status = STATUS_UNINITIALIZED;

    return -1;
}

extern ParserStatus parser_getStatus(const Parser * parser)
{
    return parser->status;
}

extern ParserStatus parser_tryMatch(Parser * parser, uint8_t newChar)
{
    return parser->functions->tryMatch(parser, newChar);
}

extern void* parser_getResults(const Parser * parser)
{
    return parser->results;
}

extern void parser_deinit(Parser * parser)
{
    parser->functions->deinit(parser);
}
