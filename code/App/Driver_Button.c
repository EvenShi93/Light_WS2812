#include "Driver_Button.h"
#include "Driver_Led.h"
#include "Driver_SysTime.h"
#include "Driver_Flash.h"

button_value Button_Value = BUTTON_NULL;

void Button_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
}

void Button_Func(void)
{
  static uint8_t Button = 0xFF;
  static uint32_t Button_Time = 0;

  if ((GPIOF->IDR & GPIO_Pin_0) == 0)
  {
    if (Button & 0x01)
    {
      Button_Time = Now_Time_Ms + 1200;
      Button &= 0xFE;
    }
    else if (Now_Time_Ms > Button_Time)
    {
      Button_Time = 0xFFFFFFFF;
      Button_Value = BUTTON_LONG;
      WS2812_Para.Struct.Ws2812_Mode = (WS2812_Para.Struct.Ws2812_Mode + 1) % MODE_NUM;
      Flash_Write_Flag = 1;
    }
  }
  else
  {
    if (((Button & 0x01) == 0) && (Button_Value != BUTTON_LONG))
    {
      Button_Value = BUTTON_SHOT;
      WS2812_Para.Struct.Ws2812_Color = (WS2812_Para.Struct.Ws2812_Color + 1) % WS2812_Para.Struct.Ws2812_Color_Max;
      Flash_Write_Flag = 1;
    }
    else
      Button_Value = BUTTON_NULL;
    Button |= 0x01;
  }
}
