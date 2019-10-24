#include "Driver_Led.h"
#include "Driver_Flash.h"
#include "Driver_SysTime.h"
#include "Task.h"
#include "stdio.h"
#include "string.h"

uint16_t WS2812_DMA[WS2812_NUM * 24 + 1];
ws2812_union_t WS2812_Buff[WS2812_NUM];
ws2812_para_union_t WS2812_Para;

void State_Led_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  STATE_LED_OFF;
}

void State_Led_Func(void)
{
  static int8_t ledcnt = 0;
  STATE_LED_OFF;
  if (ledcnt == -4)
  {
    ledcnt = WS2812_Para.Struct.Ws2812_Mode * 2 + 2;
  }
  else
  {
    if (ledcnt > 0)
    {
      if (ledcnt % 2 == 0)
        STATE_LED_ON;
      else
        STATE_LED_OFF;
    }
    ledcnt--;
  }
}

void WS2812_Led_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_2);

  TIM_TimeBaseStructure.TIM_Period = 59;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = 0;
  TIM_OCInitStructure.TIM_OCNIdleState = 0;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
  TIM_Cmd(TIM1, DISABLE);
  TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);

  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (TIM1->CCR1);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)WS2812_DMA;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = WS2812_NUM * 24 + 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);

  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);
  DMA_ClearITPendingBit(DMA1_IT_TC2);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  memcpy(&WS2812_Para, (uint8_t *)0x08003C00, sizeof(WS2812_Para));
  if (WS2812_Para.Struct.Check_Data != 0x4F4B)
  {
    WS2812_Para.Struct.Check_Data = 0x4f4B;
    WS2812_Para.Struct.Ws2812_Num = 0x08;
    WS2812_Para.Struct.Ws2812_Mode = 0x00;
    WS2812_Para.Struct.Ws2812_Color = 0x00;
    WS2812_Para.Struct.Ws2812_Color_Max = 0x0A;
    WS2812_Para.Struct.Ws2812_Brightness = 0xFF;
    WS2812_Para.Struct.Ws2812_Period = 0x64;

    WS2812_Para.Struct.Color[0].Red = 0xFF;
    WS2812_Para.Struct.Color[0].Green = 0x00;
    WS2812_Para.Struct.Color[0].Blue = 0x00;

    WS2812_Para.Struct.Color[1].Red = 0xFF;
    WS2812_Para.Struct.Color[1].Green = 0xFF;
    WS2812_Para.Struct.Color[1].Blue = 0x00;

    WS2812_Para.Struct.Color[2].Red = 0x00;
    WS2812_Para.Struct.Color[2].Green = 0xFF;
    WS2812_Para.Struct.Color[2].Blue = 0xFF;

    WS2812_Para.Struct.Color[3].Red = 0xFF;
    WS2812_Para.Struct.Color[3].Green = 0x3F;
    WS2812_Para.Struct.Color[3].Blue = 0x3F;

    WS2812_Para.Struct.Color[4].Red = 0x00;
    WS2812_Para.Struct.Color[4].Green = 0xFF;
    WS2812_Para.Struct.Color[4].Blue = 0x00;

    WS2812_Para.Struct.Color[5].Red = 0x00;
    WS2812_Para.Struct.Color[5].Green = 0x00;
    WS2812_Para.Struct.Color[5].Blue = 0xFF;

    WS2812_Para.Struct.Color[6].Red = 0xFF;
    WS2812_Para.Struct.Color[6].Green = 0x61;
    WS2812_Para.Struct.Color[6].Blue = 0x00;

    WS2812_Para.Struct.Color[7].Red = 0xFF;
    WS2812_Para.Struct.Color[7].Green = 0x00;
    WS2812_Para.Struct.Color[7].Blue = 0xFF;

    WS2812_Para.Struct.Color[8].Red = 0xFF;
    WS2812_Para.Struct.Color[8].Green = 0xFF;
    WS2812_Para.Struct.Color[8].Blue = 0xFF;

    WS2812_Para.Struct.Color[9].Red = 0x00;
    WS2812_Para.Struct.Color[9].Green = 0x00;
    WS2812_Para.Struct.Color[9].Blue = 0x00;
    Flash_Write_Flag = 1;
  }
}

void WS2812_Led_Func(void)
{
  uint16_t i = 0;
  uint8_t red, green, blue;
  static uint8_t counter = 0;
  static uint16_t num = 0;
  static uint8_t light_mode_last = 0;
  static int8_t dir = 10;

  if (light_mode_last != WS2812_Para.Struct.Ws2812_Mode)
    counter = 0;

  switch (WS2812_Para.Struct.Ws2812_Mode)
  {
  case MODE_MONOCHROME:
    for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
    {
      WS2812_Buff[i].RGB.Red = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Red * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
      WS2812_Buff[i].RGB.Green = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Green * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
      WS2812_Buff[i].RGB.Blue = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Blue * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
    }
    break;
  case MODE_FLICKER:
    if (light_mode_last != WS2812_Para.Struct.Ws2812_Mode)
    {
      Task_Set_Period(TASK_ID_WS2812, 200);
      counter = 0;
    }
    if (counter % 2)
    {
      for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
      {
        WS2812_Buff[i].RGB.Red = 0;
        WS2812_Buff[i].RGB.Green = 0;
        WS2812_Buff[i].RGB.Blue = 0;
      }
    }
    else
    {
      for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
      {
        WS2812_Buff[i].RGB.Red = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Red * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
        WS2812_Buff[i].RGB.Green = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Green * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
        WS2812_Buff[i].RGB.Blue = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Blue * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
      }
    }
    counter++;
    break;
  case MODE_SCROLLER:
    if (light_mode_last != WS2812_Para.Struct.Ws2812_Mode)
    {
      Task_Set_Period(TASK_ID_WS2812, 25);
      counter = 0;
    }
    for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
    {
      WS2812_Buff[i].RGB.Red = 0;
      WS2812_Buff[i].RGB.Green = 0;
      WS2812_Buff[i].RGB.Blue = 0;
    }
    counter++;
    if (counter == WS2812_Para.Struct.Ws2812_Num)
      counter = 0;
    WS2812_Buff[counter].RGB.Red = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Red * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
    WS2812_Buff[counter].RGB.Green = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Green * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
    WS2812_Buff[counter].RGB.Blue = ((uint16_t)WS2812_Para.Struct.Color[WS2812_Para.Struct.Ws2812_Color].Blue * WS2812_Para.Struct.Ws2812_Brightness) >> 8;
    break;
  case MODE_RAINBOW:
    if (light_mode_last != WS2812_Para.Struct.Ws2812_Mode)
    {
      Task_Set_Period(TASK_ID_WS2812, 500);
      counter = 0;
    }
    for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
    {
      num += 1536/(WS2812_Para.Struct.Ws2812_Num-1);
      num %= 1536;
      rainbow(&red, &green, &blue, num, WS2812_Para.Struct.Ws2812_Brightness);
      WS2812_Buff[i].RGB.Red = red;
      WS2812_Buff[i].RGB.Green = green;
      WS2812_Buff[i].RGB.Blue = blue;
    }
    break;

  case MODE_BREATHING:
    if (light_mode_last != WS2812_Para.Struct.Ws2812_Mode)
    {
      Task_Set_Period(TASK_ID_WS2812, (1000/WS2812_Para.Struct.Ws2812_Brightness));
      counter = 0;
    }
    if (counter >= WS2812_Para.Struct.Ws2812_Brightness)
      dir = -1;
    else if (counter == 0)
    {
      dir = 1;
      num += 1536 - 1536 / (WS2812_Para.Struct.Ws2812_Num+1);
    }
    counter += dir;
    for (i = 0; i < WS2812_Para.Struct.Ws2812_Num; i++)
    {
      rainbow(&red, &green, &blue, num, counter);
      WS2812_Buff[i].RGB.Red = red;
      WS2812_Buff[i].RGB.Green = green;
      WS2812_Buff[i].RGB.Blue = blue;
      num += 1536 / WS2812_Para.Struct.Ws2812_Num;
      num = num % 1536;
    }
    break;
  }
  light_mode_last = WS2812_Para.Struct.Ws2812_Mode;
  i = 0;
  for (uint16_t j = 0; j < WS2812_Para.Struct.Ws2812_Num; j++)
  {
    for (uint8_t k = 0; k < 3; k++)
    {
      uint8_t tmp = WS2812_Buff[j].Byte[k];
      for (uint8_t l = 0; l < 8; l++)
      {
        if (tmp & 0x80)
          WS2812_DMA[i++] = 38;
        else
          WS2812_DMA[i++] = 19;
        tmp <<= 1;
      }
    }
  }
  WS2812_DMA[i] = 0;
  DMA1_Channel2->CMAR = (uint32_t)&WS2812_DMA;
  DMA1_Channel2->CNDTR = WS2812_Para.Struct.Ws2812_Num * 24 + 1;
  TIM1->CNT = 0;
  TIM1->CR1 |= TIM_CR1_CEN;
  DMA1_Channel2->CCR |= DMA_CCR_EN;
}

void rainbow(uint8_t *red, uint8_t *green, uint8_t *blue, uint16_t num, uint8_t brightness)
{
  uint8_t step, tmp;
  num = num % 1536;
  step = num / 256;
  tmp = (num % 256) * brightness * 0.003921569f;
  switch (step)
  {
  case 0:
    *red = brightness;
    *green = tmp;
    *blue = 0;
    break;
  case 1:
    *red = brightness - tmp;
    *green = brightness;
    *blue = 0;
    break;
  case 2:
    *red = 0;
    *green = brightness;
    *blue = tmp;
    break;
  case 3:
    *red = 0;
    *green = brightness - tmp;
    *blue = brightness;
    break;
  case 4:
    *red = tmp;
    *green = 0;
    *blue = brightness;
    break;
  case 5:
    *red = brightness;
    *green = 0;
    *blue = brightness - tmp;
    break;
  }
}

void DMA1_Channel2_3_IRQHandler()
{
  if ((DMA1->ISR & DMA1_IT_TC2) != (uint32_t)RESET)
  {
    DMA1->IFCR = DMA1_IT_TC2;
    DMA1_Channel2->CCR &= (uint16_t)(~DMA_CCR_EN);
    TIM1->CR1 &= (uint16_t)~TIM_CR1_CEN;
  }
}
