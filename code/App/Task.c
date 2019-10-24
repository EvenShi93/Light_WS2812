#include "Task.h"

TASK Tasks[TASK_ID_NUM];

void Task_Init(uint8_t task_id, int16_t times, int16_t period, void (*function_name)())
{
  Tasks[task_id].Times = times;
  Tasks[task_id].Period = period;
  Tasks[task_id].Period_cnt = Tasks[task_id].Period;
  Tasks[task_id].fp = Task;
  Tasks[task_id].Function_Name = function_name;
}

void Task_Default(uint8_t task_id)
{
  Tasks[task_id].fp = 0;
}

void Task_Recovery(uint8_t task_id)
{
  Tasks[task_id].fp = Task;
}

uint8_t Task_Get_Buzy(uint8_t task_id)
{
  if (Tasks[task_id].fp == 0)
    return 0;
  else
    return 1;
}

void Task_Set_Period(uint8_t task_id, int16_t period)
{
  Tasks[task_id].Period = period;
  if (period == 0)
  {
    Tasks[task_id].Period = 30000;
  }
}

void Task(uint8_t task_id)
{
  if (Tasks[task_id].Times != 0)
  {
    Tasks[task_id].Period_cnt = Tasks[task_id].Period;
    if (Tasks[task_id].Function_Name != 0)
    {
      Tasks[task_id].Function_Name();
    }
  }
  if (Tasks[task_id].Times > 0)
  {
    Tasks[task_id].Times--;
    if (Tasks[task_id].Times == 0)
    {
      Tasks[task_id].fp = 0;
    }
  }
}

void Task_Running(void)
{
  uint8_t i;
  for (i = 0; i < TASK_ID_NUM; i++)
  {
    if (Tasks[i].fp != 0)
    {
      if (Tasks[i].Period_cnt == 0)
      {
        Tasks[i].Period_cnt = -1;
        Tasks[i].fp(i);
      }
    }
  }
}

void Task_Counter(void)
{
  uint8_t i;
  for (i = 0; i < TASK_ID_NUM; i++)
  {
    if (Tasks[i].Period_cnt > 0)
    {
      Tasks[i].Period_cnt--;
    }
  }
}
