#ifndef _CONNECTION_FAILED_H_
#define _CONNECTION_FAILED_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T                       PARSER_DATA_TYPE(connectionFailed)
#define PARSER_RESULTS_T                    PARSER_RESULTS_TYPE(connectionFailed)
#define PARSER_RESULTS_CONNECTIONFAILED_T   PARSER_RESULTS_TYPE(connectionFailed)

#define INITIALIZER_AT_CONNECTIONFAILED {AT_MSG_CONNECTION_FAILED, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_AT_CONNECTIONFAILED}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    uint8_t         readPos;
} PARSER_DATA_T;

typedef struct {
    uint8_t connectionID;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_AT_CONNECTIONFAILED;

/*==================[external functions declaration]=========================*/

#endif // _CONNECTION_FAILED_H_
