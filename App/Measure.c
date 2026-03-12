#include "Measure.h"

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

uint16_t Measure_ReadHumidity(void)
{
    return Measure_ReadChannel(MEASURE_CH_HUMIDITY);
}

uint16_t Measure_ReadTemperature(void)
{
    return Measure_ReadChannel(MEASURE_CH_TEMPERATURE);
}
