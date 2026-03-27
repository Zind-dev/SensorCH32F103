#include "Measure.h"

/* Sensor transfer-function constants (linear: value = SCALE * mV + OFFSET).
 * Adjust these to match your actual sensors.                               */
#define HUMIDITY_SCALE_NUM       1000   /* 0.1 %  per mV  (numerator)   */
#define HUMIDITY_SCALE_DEN       33     /*                 (denominator) */
#define HUMIDITY_OFFSET          0      /* 0.1 % offset                  */

/* PT1000: ADC millivolts → resistor voltage (mV)                          *
 *   V_r = V_adc * R_VOLT_SCALE_NUM / R_VOLT_SCALE_DEN + R_VOLT_OFFSET    *
 * Stub: unity transform. Adjust for your front-end circuit.               */
#define R_VOLT_SCALE_NUM        1       /* numerator   (stub: 1:1)    */
#define R_VOLT_SCALE_DEN        1       /* denominator (stub: 1:1)    */
#define R_VOLT_OFFSET           0       /* mV offset   (stub: 0)      */

/* PT1000: Resistor voltage (mV) → temperature (0.1 °C)                    *
 *   T_x10 = (V_r - PT1000_V0_MV) * PT1000_T_NUM / PT1000_T_DEN           *
 * Linear approx: R=R0(1+aT), V_r=I*R, a~3.851e-3/°C                      *
 * Stub: assumes 1 mA excitation → V0=1000 mV at 0 °C                     */
#define PT1000_V0_MV            1000    /* voltage at 0 °C (mV)       */
#define PT1000_T_NUM            10000   /* = 10 / a  (numerator)      */
#define PT1000_T_DEN            3851    /* = 1 / a   (denominator)    */

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
    /* ADC value → millivolts */
    uint16_t mv_adc = Measure_AdcToMillivolts(adc_val);

    /* ADC millivolts → voltage across PT1000 (mV) */
    int32_t mv_r = (int32_t)mv_adc * R_VOLT_SCALE_NUM / R_VOLT_SCALE_DEN
                   + R_VOLT_OFFSET;

    /* PT1000 voltage → temperature (0.1 °C) */
    int32_t temp_x10 = (mv_r - PT1000_V0_MV) * PT1000_T_NUM / PT1000_T_DEN;

    return (int16_t)temp_x10;
}

void Measure_GetResults(Measure_Result_t *result)
{
    uint16_t raw_hum  = Measure_ReadAvg(MEASURE_CH_HUMIDITY);
    uint16_t raw_temp = Measure_ReadAvg(MEASURE_CH_TEMPERATURE);

    result->humidity_pct_x10  = Measure_ConvertHumidity(raw_hum);
    result->temperature_c_x10 = Measure_ConvertTemperature(raw_temp);
}
