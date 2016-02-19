#ifndef _RESET_DETECTION_H_
#define _RESET_DETECTION_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T                       PARSER_DATA_TYPE(resetDetection)
#define PARSER_RESULTS_T                    PARSER_RESULTS_TYPE(resetDetection)
#define PARSER_RESULTS_RESET_DETECTION_T    PARSER_RESULTS_TYPE(resetDetection)

#define INITIALIZER_AT_RESET_DETECTION {AT_MSG_RESET, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_AT_RESET_DETECTION}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    Parser          strParser;
    uint16_t        skippedChars;
} PARSER_DATA_T;

typedef struct {
    uint8_t dummy;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_AT_RESET_DETECTION;

/*==================[external functions declaration]=========================*/

#endif // _RESET_DETECTION_H_
