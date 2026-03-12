#include "Measure.h"

/* Sensor transfer-function constants (linear: value = SCALE * mV + OFFSET).
 * Adjust these to match your actual sensors.                               */
#define HUMIDITY_SCALE_NUM       1000   /* 0.1 %  per mV  (numerator)   */
#define HUMIDITY_SCALE_DEN       33     /*                 (denominator) */
#define HUMIDITY_OFFSET          0      /* 0.1 % offset                  */

#define TEMPERATURE_SCALE_NUM    1250   /* 0.1 °C per mV  (numerator)   */
#define TEMPERATURE_SCALE_DEN    33     /*                 (denominator) */
#define TEMPERATURE_OFFSET       (-500) /* 0.1 °C offset  (-50.0 °C)    */

void Measure_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    ADC_DeInit(ADC1);

    ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode        = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode  = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel         = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;
}

uint16_t Measure_ReadChannel(Measure_Channel_t ch)
{
    ADC_RegularChannelConfig(ADC1, (uint8_t)ch, 1, ADC_SampleTime_241Cycles);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;

    return ADC_GetConversionValue(ADC1);
}

uint16_t Measure_ReadAvg(Measure_Channel_t ch)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < MEASURE_AVG_SAMPLES; i++)
    {
        sum += Measure_ReadChannel(ch);
    }
    return (uint16_t)(sum / MEASURE_AVG_SAMPLES);
}

uint16_t Measure_AdcToMillivolts(uint16_t adc_val)
{
    return (uint16_t)((uint32_t)adc_val * MEASURE_VREF_MV / MEASURE_ADC_MAX);
}

int16_t Measure_ConvertHumidity(uint16_t adc_val)
{
    uint16_t mv = Measure_AdcToMillivolts(adc_val);
    return (int16_t)(((int32_t)mv * HUMIDITY_SCALE_NUM / HUMIDITY_SCALE_DEN)
                     + HUMIDITY_OFFSET);
}

int16_t Measure_ConvertTemperature(uint16_t adc_val)
{
    uint16_t mv = Measure_AdcToMillivolts(adc_val);
    return (int16_t)(((int32_t)mv * TEMPERATURE_SCALE_NUM / TEMPERATURE_SCALE_DEN)
                     + TEMPERATURE_OFFSET);
}

void Measure_GetResults(Measure_Result_t *result)
{
    uint16_t raw_hum  = Measure_ReadAvg(MEASURE_CH_HUMIDITY);
    uint16_t raw_temp = Measure_ReadAvg(MEASURE_CH_TEMPERATURE);

    result->humidity_pct_x10  = Measure_ConvertHumidity(raw_hum);
    result->temperature_c_x10 = Measure_ConvertTemperature(raw_temp);
}
