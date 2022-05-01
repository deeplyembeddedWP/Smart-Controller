/*
 * iot_logging_helper_fxns.c
 *
 * Created: 7/12/2021 10:58:47 AM
 *  Author: Developer
 */
#include "stdint.h"
#include "string.h" 
#include "driver_init.h"
#include "iot_logging_helper_fxns.h"

void UART_writeString(const char *buff){
	struct io_descriptor *io;
	usart_os_get_io(&TARGET_IO, &io);
	io->write(io, (uint8_t*)buff, strlen(buff));
}

