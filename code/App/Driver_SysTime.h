#ifndef __DRIVER_SYSTIME_H__
#define __DRIVER_SYSTIME_H__

#include "stm32f0xx.h"

#define Now_Time_Ms TIM2->CNT * 0.001
#define Now_Time_Us TIM2->CNT

void Delay_Ms(uint16_t ms);
void System_Time_Init(void);
void Task_Time_Init(void);

#endif
