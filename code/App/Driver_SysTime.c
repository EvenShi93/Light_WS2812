#include "Driver_SysTime.h"
#include "Task.h"

void Delay_Ms(uint16_t ms)
{
  uint32_t tmp = Now_Time_Us + ms * 1000;
  while (Now_Time_Us < tmp)
    ;
}
void System_Time_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_DeInit(TIM2);

  TIM_TimeBaseStructure.TIM_Period = 0xffffffff;
  TIM_TimeBaseStructure.TIM_Prescaler = 47;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_Cmd(TIM2, ENABLE);
}

void Task_Time_Init(void)
{
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  SysTick_Config(48000);
}

void SysTick_Handler(void)
{
  Task_Counter();
}
