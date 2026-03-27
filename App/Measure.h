#ifndef MEASURE_H
#define MEASURE_H

#include "ch32v00x.h"

#define MEASURE_AVG_SAMPLES   8
#define MEASURE_ADC_MAX       1023
#define MEASURE_VREF_MV       3300

typedef enum {
    MEASURE_CH_HUMIDITY    = ADC_Channel_0,
    MEASURE_CH_TEMPERATURE = ADC_Channel_1,
    MEASURE_CH_COUNT       = 2
} Measure_Channel_t;

typedef struct {
    int16_t  humidity_pct_x10;     /* humidity in 0.1 % units  (e.g. 655 = 65.5 %) */
    int16_t  temperature_c_x10;    /* temperature in 0.1 °C units (e.g. 237 = 23.7 °C) */
} Measure_Result_t;

void     Measure_Init(void);
uint16_t Measure_ReadChannel(Measure_Channel_t ch);
uint16_t Measure_ReadAvg(Measure_Channel_t ch);
uint16_t Measure_AdcToMillivolts(uint16_t adc_val);
int16_t  Measure_ConvertHumidity(uint16_t adc_val);
int16_t  Measure_ConvertTemperature(uint16_t adc_val);
void     Measure_GetResults(Measure_Result_t *result);

#endif /* MEASURE_H */
