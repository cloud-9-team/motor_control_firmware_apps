#ifndef _PARSER_H_
#define _PARSER_H_

 /** \addtogroup MotorControl
 ** @{ */

/** \brief Interfaz unificada para el manejo de los parsers.
 *
 * Provee una interfaz común y declara ciertos tipos utilizados para crear y
 * manipular parsers.
 *
 * Un parser es un módulo de software que cumple con ciertas especificaciones y
 * que tiene como finalidad recibir caracteres, uno a uno, y decidir si cumplen
 * con algún patrón o regla dados. A su vez, existe la posibilidad que al
 * detectar el patrón, también se extraigan datos de éste y sean devueltos al
 * usuario como resultados del parseo.
 *
 */

 /** \defgroup PARSER Parser
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaPOSIX_stdio.h"
#include "ciaaPOSIX_stdlib.h"

/*==================[macros]=================================================*/

#define __PARSER_RESULTS_TYPE(name)         parser_ ## name ## _results
#define __PARSER_DATA_TYPE(name)            parser_ ## name ## _data

#define PARSER_RESULTS_TYPE(name)           __PARSER_RESULTS_TYPE(name)
#define PARSER_DATA_TYPE(name)              __PARSER_DATA_TYPE(name)


/** \brief Macro para utilizar la función definida en el firmware en lugar de la estándar. */
#define free	ciaaPOSIX_free

/** \brief Macro para utilizar la función definida en el firmware en lugar de la estándar. */
#define malloc	ciaaPOSIX_malloc

/** \brief Macro para utilizar la función definida en el firmware en lugar de la estándar. */
#define memcpy	ciaaPOSIX_memcpy

/*==================[typedef]================================================*/

/** \brief Enumerativos de estados para utilizar en una máquina de estados. */
typedef enum {S0, S1, S2, S3, S4, S5} fsmStatus;

/** \brief Estados posibles de un parser */
typedef enum {
    STATUS_UNINITIALIZED, /**< Todavía no se ha llamado \p parser_init() en el parser. */
    STATUS_INITIALIZED, /**< Se ha llamado \p parser_init(), pero no se hizo nada más. */
    STATUS_INCOMPLETE, /**< El último carácter ingresado mediante \p parser_tryMatch() es válido pero no completa el patrón. */
    STATUS_NOT_MATCHES, /**< El último carácter ingresado mediante \p parser_tryMatch() no puede formar parte del patrón. */
    STATUS_COMPLETE /**< El último carácter ingresado mediante \p parser_tryMatch() ha completado exitosamente el patrón. */
} ParserStatus;

/**
 *
 * Valores reservados para el tipo de parser.
 * Si se agrega un nuevo parser, es conveniente agregar el tipo en este
 * enumerativo.
 *
 * Si por algún razón no importa el tipo, establecerlo en un valor
 * distinto a los reservados.
 *
 */
typedef enum {
    AT_MSG_IPD = 0,
    AT_MSG_CONNECTION_OPEN,
    AT_MSG_CONNECTION_CLOSE,
	AT_MSG_CONNECTION_FAILED,
    USER_DUTYCYCLE,
    LITERAL_PARSER,
    AT_MSG_RESET,
    USER_CARACTERIZAR,
    PARSER_TYPES_COUNT
} ParserType;

typedef struct Parser_struct Parser;

/** \brief Funciones que todo parser debe implementar.
 *
 * Estas funciones determinan el comportamiento individual de cada
 * parser.
 *
 */
typedef struct {
    void            (*init)(Parser* parserPtr);
    ParserStatus    (*tryMatch)(Parser* parserPtr, uint8_t newChar);
    void            (*deinit)(Parser* parserPtr);
} ParserFunctions;

struct Parser_struct {
    const ParserType        type;
    ParserStatus            status;
    void*                   data;
    void*                   results;
    const ParserFunctions*  functions;
};

/*==================[inclusions]=============================================*/

#include "at_cmd/ipd.h"
#include "at_cmd/connection_open.h"
#include "at_cmd/connection_close.h"
#include "at_cmd/connection_failed.h"
#include "at_cmd/literal_parser.h"
#include "user_cmd/dutycycle.h"
#include "at_cmd/reset_detection.h"
#include "user_cmd/caracterizar.h"

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** \brief Inicializa este módulo. Llamar antes de llamar cualquier otra función. */
extern void parser_initModule(void);

/** \brief Inicializa el parser especificado.
 *
 * Establece los valores iniciales para cada parser. Si se quiere reiniciar su
 * estado, se lo puede hacer llamando de nuevo esta función sobre el mismo.
 *
 * Es obligatorio llamar esta función antes de empezar a usar un parser.
 *
 * \param[in] parser Puntero al parser a inicializar.
 * \return Retorna un valor positivo si la inicialización fue correcta, de lo
 * contrario el valor será negativo.
 */
extern int32_t parser_init(Parser * parser);


/** \brief Obtiene el estado actual del parser.
 *
 * Permite obtener el estado actual del parser, que es resultado del último
 * carácter ingresado.
 *
 * \param[in] parser Puntero al parser del que se desea conocer su estado.
 * \return Estado actual del parser.
 *
 */
extern ParserStatus parser_getStatus(const Parser * parser);


/** \brief Ingresa un carácter para intentar formar el patrón.
 *
 * Un nuevo carácter es ingresado para analizar si forma el patrón buscado en
 * conjunto con los caracteres anteriormente ingresados.
 *
 * Esta operación siempre modifica el estado del parser.
 *
 * \param[in] parser Puntero al parser sobre el cual se desea ver si el carácter pertenece al patrón.
 * \param[in] newChar Carácter a ingresar al parser.
 * \return Estado del parser luego de haber ingresado el carácter.
 *
 */
extern ParserStatus parser_tryMatch(Parser * parser, uint8_t newChar);


/** \brief Obtiene el resultado del parseo.
 *
 * Llamar sólo si el parser tiene como estado \p STATUS_COMPLETE, de lo contrario,
 * los valores que retorne pueden ser incorrectos.
 *
 * \param[in] parser Puntero al parser cuyos resultados se desean conocer.
 * \return Resultado del parseo. Debe castearse al tipo apropiado, definido por cada
 * parser.
 *
*/
extern void* parser_getResults(const Parser * parser);


/** \brief De-inicializa un parser.
 *
 * Esta función pone al parser en un estado inválido, por lo cual si se lo quiere
 * volver a usar, deberá ser inicializado de nuevo con \p parser_init().
 *
 * \param[in] parser Puntero al parser a de-inicializar.
 */
extern void parser_deinit(Parser * parser);

/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* _PARSER_H_ */
