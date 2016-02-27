/*==================[inclusions]=============================================*/

#include "debug_logger.h"
#include "chip.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

#ifdef LOGGING_ALLOWED
/** \brief File descriptor of UART connected to the USB bridge (FT2232)
 *
 * Device path /dev/serial/uart/2
 */
static int32_t fd_uart;
#endif

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void logger_init(void)
{
#ifdef LOGGING_ALLOWED

    /* open UART connected to USB bridge (FT2232) */
	fd_uart = ciaaPOSIX_open("/dev/serial/uart/1", ciaaPOSIX_O_RDWR | ciaaPOSIX_O_NONBLOCK);

	/* change baud rate */
	ciaaPOSIX_ioctl(fd_uart, ciaaPOSIX_IOCTL_SET_BAUDRATE, (void *)ciaaBAUDRATE_115200);

	/* change FIFO TRIGGER LEVEL for uart usb */
	ciaaPOSIX_ioctl(fd_uart, ciaaPOSIX_IOCTL_SET_FIFO_TRIGGER_LEVEL, (void *)ciaaFIFO_TRIGGER_LEVEL3);

#else

	/* Disable peripheral clock */
	Chip_Clock_Disable(CLK_MX_UART2);

#endif
}



void logger_print_data(uint8_t * data, size_t size)
{
#ifdef LOGGING_ALLOWED
    ciaaPOSIX_write(fd_uart, data, size);
#endif // LOGGING_ALLOWED
}


void logger_print_string(const char * str)
{
#ifdef LOGGING_ALLOWED
    ciaaPOSIX_write(fd_uart, str, ciaaPOSIX_strlen(str));
#endif // LOGGING_ALLOWED
}

ssize_t logger_read_input(void * buf, size_t size)
{
#ifdef LOGGING_ALLOWED
    return ciaaPOSIX_read(fd_uart, buf, size);
#endif // LOGGING_ALLOWED
}
