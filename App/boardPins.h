#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "ch32v00x.h"

/* USART1 pins (default mapping, no remap) */
#define BOARD_USART1_TX_PORT        GPIOD
#define BOARD_USART1_TX_PIN         GPIO_Pin_5
#define BOARD_USART1_RX_PORT        GPIOD
#define BOARD_USART1_RX_PIN         GPIO_Pin_6

/* RS485 direction pin (DE/RE active HIGH = TX) */
#define BOARD_RS485_DE_PORT         GPIOC
#define BOARD_RS485_DE_PIN          GPIO_Pin_1

/* TIM1 CH1 PWM output */
#define BOARD_PWM_PORT              GPIOD
#define BOARD_PWM_PIN               GPIO_Pin_2

/* ADC channels (PA0 = humidity, PA1 = temperature) */
#define BOARD_ADC_HUMIDITY_PORT     GPIOA
#define BOARD_ADC_HUMIDITY_PIN      GPIO_Pin_0
#define BOARD_ADC_HUMIDITY_CH       ADC_Channel_0
#define BOARD_ADC_TEMPERATURE_PORT  GPIOA
#define BOARD_ADC_TEMPERATURE_PIN   GPIO_Pin_1
#define BOARD_ADC_TEMPERATURE_CH    ADC_Channel_1

#endif /* BOARD_PINS_H */
