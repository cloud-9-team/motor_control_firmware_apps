#ifndef _CARACTERIZAR_H_
#define _CARACTERIZAR_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T					PARSER_DATA_TYPE(caracterizar)
#define PARSER_RESULTS_T                PARSER_RESULTS_TYPE(caracterizar)
#define PARSER_RESULTS_CARACTERIZAR_T	PARSER_RESULTS_TYPE(caracterizar)

#define INITIALIZER_CARACTERIZAR {USER_CARACTERIZAR, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_USER_CARACTERIZAR}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus       state;
    uint16_t        readPos;
} PARSER_DATA_T;

typedef struct {
    uint16_t    tiempo;
    uint8_t     idMotor;
} PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_USER_CARACTERIZAR;

/*==================[external functions declaration]=========================*/


#endif // _CARACTERIZAR_H_
