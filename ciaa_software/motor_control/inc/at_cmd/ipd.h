#ifndef _IPD_H_
#define _IPD_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T					PARSER_DATA_TYPE(IPD)
#define PARSER_RESULTS_T                PARSER_RESULTS_TYPE(IPD)
#define PARSER_RESULTS_IPD_T			PARSER_RESULTS_TYPE(IPD)

#define INITIALIZER_AT_IPD {AT_MSG_IPD, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_AT_IPD}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    uint16_t        readPos;
    uint16_t        writePos;
} PARSER_DATA_T;

typedef struct {
    uint8_t *   buffer;
    uint16_t    bufferLength;
    uint16_t    payloadLength;
    uint8_t     connectionID;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_AT_IPD;

/*==================[external functions declaration]=========================*/

extern uint8_t parser_ipd_isDataBeingSaved(Parser* parserPtr);
extern void parser_ipd_setBuffer(Parser* parserPtr, uint8_t * buf, uint16_t sz);

#endif // _IPD_H_
