#ifndef PWM_H
#define PWM_H

#include "ch32v00x.h"

void PWM_Init(uint16_t prescaler, uint16_t period);
void PWM_SetDuty(uint16_t channel, uint16_t duty);

#endif /* PWM_H */
