#ifndef _DUTYCYCLE_H_
#define _DUTYCYCLE_H_

/*==================[inclusions]=============================================*/

#include "../parser.h"
#include "../pwm.h"

/*==================[macros]=================================================*/

#undef PARSER_DATA_T
#undef PARSER_RESULTS_T

#define PARSER_DATA_T               PARSER_DATA_TYPE(dutyCycle)
#define PARSER_RESULTS_T            PARSER_RESULTS_TYPE(dutyCycle)
#define PARSER_RESULTS_DUTYCYCLE_T  PARSER_RESULTS_TYPE(dutyCycle)

#define INITIALIZER_DUTYCYCLE {USER_DUTYCYCLE, STATUS_UNINITIALIZED, 0, 0, &FUNCTIONS_DUTYCYCLE}

/*==================[typedef]================================================*/

typedef struct {
    fsmStatus		state;
    uint16_t		dataLength;
} PARSER_DATA_T;

typedef MotorControlData PARSER_RESULTS_T;

/*==================[external data declaration]==============================*/

extern const ParserFunctions FUNCTIONS_DUTYCYCLE;

/*==================[external functions declaration]=========================*/

#endif // _DUTYCYCLE_H_
