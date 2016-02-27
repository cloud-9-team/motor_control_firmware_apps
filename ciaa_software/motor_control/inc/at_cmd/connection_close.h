#ifndef _CONNECTION_CLOSE_H_
#define _CONNECTION_CLOSE_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T                       PARSER_DATA_TYPE(connectionClose)
#define PARSER_RESULTS_T                    PARSER_RESULTS_TYPE(connectionClose)
#define PARSER_RESULTS_CONNECTIONCLOSE_T    PARSER_RESULTS_TYPE(connectionClose)

#define INITIALIZER_AT_CONNECTIONCLOSE {AT_MSG_CONNECTION_CLOSE, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_AT_CONNECTIONCLOSE}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    uint8_t         readPos;
} PARSER_DATA_T;

typedef struct {
    uint8_t connectionID;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_AT_CONNECTIONCLOSE;

/*==================[external functions declaration]=========================*/

#endif // _CONNECTION_CLOSE_H_
