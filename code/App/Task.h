#ifndef __TASK_H__
#define __TASK_H__

#include "stm32f0xx.h"

typedef enum
{
  TASK_ID_WS2812 = 0,
  TASK_ID_LED,
  TASK_ID_PWMC,
  TASK_ID_BUTTON,
  TASK_ID_FLASH,
  TASK_ID_NUM,
} TASKKIND;

typedef struct
{
  int16_t Times;
  int16_t Period_cnt;
  int16_t Period;
  void (*fp)(uint8_t Task_Id);
  void (*Function_Name)(void);
} TASK;

void Task_Init(uint8_t task_id, int16_t times, int16_t period, void (*function_name)());
void Task_Default(uint8_t task_id);
void Task_Recovery(uint8_t task_id);
uint8_t Task_Get_Buzy(uint8_t task_id);
void Task_Set_Period(uint8_t task_id, int16_t period);
void Task(uint8_t task_id);
void Task_Running(void);
void Task_Counter(void);

#endif
