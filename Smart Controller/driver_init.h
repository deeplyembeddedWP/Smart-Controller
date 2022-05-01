/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */
#ifndef DRIVER_INIT_INCLUDED
#define DRIVER_INIT_INCLUDED

#include "atmel_start_pins.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <hal_atomic.h>
#include <hal_delay.h>
#include <hal_gpio.h>
#include <hal_init.h>
#include <hal_io.h>
#include <hal_sleep.h>

#include <hal_usart_os.h>

#include <hal_usart_os.h>

#define SERCOM_1_UART_BUFFER_SIZE 16

#define TARGET_IO_BUFFER_SIZE 16

extern struct usart_os_descriptor SERCOM_1_UART;
extern uint8_t                    SERCOM_1_UART_buffer[];

extern struct usart_os_descriptor TARGET_IO;
extern uint8_t                    TARGET_IO_buffer[];

void SERCOM_1_UART_PORT_init(void);
void SERCOM_1_UART_CLOCK_init(void);
void SERCOM_1_UART_init(void);

void TARGET_IO_PORT_init(void);
void TARGET_IO_CLOCK_init(void);
void TARGET_IO_init(void);

/**
 * \brief Perform system initialization, initialize pins and clocks for
 * peripherals
 */
void system_init(void);

#ifdef __cplusplus
}
#endif
#endif // DRIVER_INIT_INCLUDED
