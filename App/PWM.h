#ifndef PWM_H
#define PWM_H

#include "ch32v00x.h"

void PWM_Init(uint16_t prescaler, uint16_t period);
void PWM_SetDuty(uint16_t channel, uint16_t duty);
void PWM_SetFromVoltage_0_10(uint16_t channel, uint16_t voltage_mv);
void PWM_SetFromCurrent_4_20(uint16_t channel, uint16_t current_ua);

#endif /* PWM_H */
