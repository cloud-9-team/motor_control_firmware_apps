#ifndef _LITERAL_PARSER_H_
#define _LITERAL_PARSER_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T                       PARSER_DATA_TYPE(literalParser)
#define PARSER_RESULTS_T                    PARSER_RESULTS_TYPE(literalParser)
#define PARSER_RESULTS_LITERALPARSER_T		PARSER_RESULTS_TYPE(literalParser)

#define INITIALIZER_LITERAL_PARSER {LITERAL_PARSER, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_LITERAL_PARSER}

/*==================[typedef]================================================*/

typedef struct {
	char * 			string;
    uint8_t         readPos;
} PARSER_DATA_T;

typedef struct {
    uint8_t dummy;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_LITERAL_PARSER;

/*==================[external functions declaration]=========================*/

extern void literalParser_setStringToMatch(Parser* parserPtr, const char * str);

#endif // _LITERAL_PARSER_H_
