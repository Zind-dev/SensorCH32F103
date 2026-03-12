#include "PWM.h"

void PWM_Init(uint16_t prescaler, uint16_t period)
{
    GPIO_InitTypeDef        GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_OCInitTypeDef       TIM_OCInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    /* TIM1 CH1 -> PD2 (default mapping) */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Time base */
    TIM_TimeBaseStructure.TIM_Period            = period - 1;
    TIM_TimeBaseStructure.TIM_Prescaler         = prescaler - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* PWM mode 1 on channel 1 */
    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse        = 0;
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

void PWM_SetDuty(uint16_t channel, uint16_t duty)
{
    switch (channel)
    {
        case TIM_Channel_1: TIM_SetCompare1(TIM1, duty); break;
        case TIM_Channel_2: TIM_SetCompare2(TIM1, duty); break;
        case TIM_Channel_3: TIM_SetCompare3(TIM1, duty); break;
        case TIM_Channel_4: TIM_SetCompare4(TIM1, duty); break;
        default: break;
    }
}
