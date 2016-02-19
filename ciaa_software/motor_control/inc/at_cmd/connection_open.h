#ifndef _CONNECTION_OPEN_H_
#define _CONNECTION_OPEN_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T                       PARSER_DATA_TYPE(connectionOpen)
#define PARSER_RESULTS_T                    PARSER_RESULTS_TYPE(connectionOpen)
#define PARSER_RESULTS_CONNECTIONOPEN_T     PARSER_RESULTS_TYPE(connectionOpen)

#define INITIALIZER_AT_CONNECTIONOPEN {AT_MSG_CONNECTION_OPEN, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_AT_CONNECTIONOPEN}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    uint8_t         readPos;
} PARSER_DATA_T;

typedef struct {
    uint8_t connectionID;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_AT_CONNECTIONOPEN;

/*==================[external functions declaration]=========================*/

#endif // _CONNECTION_OPEN_H_
