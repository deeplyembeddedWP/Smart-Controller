/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

//#include <stdio.h>
#include "driver_examples.h"
#include "driver_init.h"
#include "utils.h"
#include "cellular_config.h"

/**
 * Example task of using SERCOM_1_UART to echo using the IO abstraction.
 */
void SERCOM_1_UART_example_task(void *p){
	struct io_descriptor *io;

	(void)p;

	usart_os_get_io(&SERCOM_1_UART, &io);
	
	while(1){
		//printf("%s\r\n", buff);
		//io->write(io, (uint8_t *)buff, 30);
		vTaskDelay(100);	
	}
}

/**
 * Example task of using TARGET_IO to echo using the IO abstraction.
 */
void TARGET_IO_example_task(void *p){

	for (;;) {
		vTaskDelay(1000);
		UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		configPRINTF(("[TARGET_IO_example_task], Stack: %lu", uxHighWaterMark*4));
	}
}
