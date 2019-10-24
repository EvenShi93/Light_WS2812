#ifndef __DRIVER_BUTTON_H__
#define __DRIVER_BUTTON_H__

#include "stm32f0xx.h"

typedef enum
{
  BUTTON_NULL = 0,
  BUTTON_SHOT,
  BUTTON_LONG,
  BUTTON_PRE_LONG,
} button_value;

extern button_value Button_Value;

void Button_Init(void);
void Button_Func(void);

#endif
