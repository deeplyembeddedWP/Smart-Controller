/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

#include "driver_init.h"
#include <peripheral_clk_config.h>
#include <utils.h>
#include <hal_init.h>

/* The priority of the peripheral should be between the low and high interrupt priority set by chosen RTOS,
 * Otherwise, some of the RTOS APIs may fail to work inside interrupts
 * In case of FreeRTOS,the Lowest Interrupt priority is set by configLIBRARY_LOWEST_INTERRUPT_PRIORITY and
 * Maximum interrupt priority by configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, So Interrupt Priority of the peripheral
 * should be between configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY and configLIBRARY_LOWEST_INTERRUPT_PRIORITY
 */
#define PERIPHERAL_INTERRUPT_PRIORITY (configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1)

struct usart_os_descriptor SERCOM_1_UART;
uint8_t                    SERCOM_1_UART_buffer[SERCOM_1_UART_BUFFER_SIZE];

struct usart_os_descriptor TARGET_IO;
uint8_t                    TARGET_IO_buffer[TARGET_IO_BUFFER_SIZE];

void SERCOM_1_UART_PORT_init(void)
{

	gpio_set_pin_function(PC22, PINMUX_PC22C_SERCOM1_PAD0);

	gpio_set_pin_function(PC23, PINMUX_PC23C_SERCOM1_PAD1);
}

void SERCOM_1_UART_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_CORE, CONF_GCLK_SERCOM1_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM1_GCLK_ID_SLOW, CONF_GCLK_SERCOM1_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBAMASK_SERCOM1_bit(MCLK);
}

void SERCOM_1_UART_init(void)
{
	SERCOM_1_UART_CLOCK_init();
	uint32_t irq = SERCOM1_0_IRQn;
	for (uint32_t i = 0; i < 4; i++) {
		NVIC_SetPriority((IRQn_Type)irq, PERIPHERAL_INTERRUPT_PRIORITY);
		irq++;
	}
	usart_os_init(&SERCOM_1_UART, SERCOM1, SERCOM_1_UART_buffer, SERCOM_1_UART_BUFFER_SIZE, (void *)NULL);
	usart_os_enable(&SERCOM_1_UART);
	SERCOM_1_UART_PORT_init();
}

void TARGET_IO_PORT_init(void)
{

	gpio_set_pin_function(PB25, PINMUX_PB25D_SERCOM2_PAD0);

	gpio_set_pin_function(PB24, PINMUX_PB24D_SERCOM2_PAD1);
}

void TARGET_IO_CLOCK_init(void)
{
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_CORE, CONF_GCLK_SERCOM2_CORE_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));
	hri_gclk_write_PCHCTRL_reg(GCLK, SERCOM2_GCLK_ID_SLOW, CONF_GCLK_SERCOM2_SLOW_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos));

	hri_mclk_set_APBBMASK_SERCOM2_bit(MCLK);
}

void TARGET_IO_init(void)
{

	TARGET_IO_CLOCK_init();
	uint32_t irq = SERCOM2_0_IRQn;
	for (uint32_t i = 0; i < 4; i++) {
		NVIC_SetPriority((IRQn_Type)irq, PERIPHERAL_INTERRUPT_PRIORITY);
		irq++;
	}
	usart_os_init(&TARGET_IO, SERCOM2, TARGET_IO_buffer, TARGET_IO_BUFFER_SIZE, (void *)NULL);
	usart_os_enable(&TARGET_IO);
	TARGET_IO_PORT_init();
}

void system_init(void)
{
	init_mcu();

	SERCOM_1_UART_init();

	TARGET_IO_init();
}
