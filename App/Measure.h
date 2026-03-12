#ifndef MEASURE_H
#define MEASURE_H

#include "ch32v00x.h"

typedef enum {
    MEASURE_CH_HUMIDITY    = ADC_Channel_0,
    MEASURE_CH_TEMPERATURE = ADC_Channel_1,
    MEASURE_CH_COUNT       = 2
} Measure_Channel_t;

void     Measure_Init(void);
uint16_t Measure_ReadChannel(Measure_Channel_t ch);
uint16_t Measure_ReadHumidity(void);
uint16_t Measure_ReadTemperature(void);

#endif /* MEASURE_H */
