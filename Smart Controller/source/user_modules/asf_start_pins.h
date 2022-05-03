/*
 * asf_start_pins.h
 *
 * Created: 3/05/2022 9:07:58 pm
 *  Author: VinayD
 */ 


#ifndef ASF_START_PINS_H_
#define ASF_START_PINS_H_

#include <hal_gpio.h>

// SAME54 has 14 pin functions

#define GPIO_PIN_FUNCTION_A 0
#define GPIO_PIN_FUNCTION_B 1
#define GPIO_PIN_FUNCTION_C 2
#define GPIO_PIN_FUNCTION_D 3
#define GPIO_PIN_FUNCTION_E 4
#define GPIO_PIN_FUNCTION_F 5
#define GPIO_PIN_FUNCTION_G 6
#define GPIO_PIN_FUNCTION_H 7
#define GPIO_PIN_FUNCTION_I 8
#define GPIO_PIN_FUNCTION_J 9
#define GPIO_PIN_FUNCTION_K 10
#define GPIO_PIN_FUNCTION_L 11
#define GPIO_PIN_FUNCTION_M 12
#define GPIO_PIN_FUNCTION_N 13

#define PB24 GPIO(GPIO_PORTB, 24)
#define PB25 GPIO(GPIO_PORTB, 25)
#define PC22 GPIO(GPIO_PORTC, 22)
#define PC23 GPIO(GPIO_PORTC, 23)

#define PB22 GPIO(GPIO_PORTB, 22)


#endif /* ASF_START_PINS_H_ */