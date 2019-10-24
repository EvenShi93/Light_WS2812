#ifndef __DRIVER_LED_H__
#define __DRIVER_LED_H__

#include "stm32f0xx.h"

#define STATE_LED_FLICKER GPIOA->ODR ^= 1 << 4
#define STATE_LED_OFF GPIOA->BRR = 1 << 4
#define STATE_LED_ON GPIOA->BSRR = 1 << 4

#define WS2812_NUM 32

typedef enum
{
	MODE_MONOCHROME = 0,
	MODE_FLICKER,
	MODE_SCROLLER,
	MODE_RAINBOW,
	MODE_BREATHING,
	MODE_NUM,
} light_mode;

struct ws2812_t
{
	uint8_t Green;
	uint8_t Red;
	uint8_t Blue;
};

typedef union {
	uint8_t Byte[3];
	struct ws2812_t RGB;
} ws2812_union_t;

typedef union {
	struct
	{
		uint16_t Check_Data;
		uint8_t Ws2812_Num;				 //WS2812的灯数
		uint8_t Ws2812_Mode;			 //WS2812的当前模式
		uint8_t Ws2812_Color;			 //WS2812的当前色号
		uint8_t Ws2812_Color_Max;	//WS2812的最多色号
		uint8_t Ws2812_Brightness; //WS2812的亮度设置
		uint8_t Ws2812_Period;		 //WS2812的执行周期
		struct ws2812_t Color[64]; //色号存储表
	} Struct;
	uint32_t Word[50]; //色号存储表
} ws2812_para_union_t;

extern ws2812_union_t WS2812_Buff[WS2812_NUM];
extern ws2812_para_union_t WS2812_Para;

void State_Led_Init(void);
void State_Led_Func(void);
void WS2812_Led_Init(void);
void WS2812_Led_Func(void);
void rainbow(uint8_t *red, uint8_t *green, uint8_t *blue, uint16_t num, uint8_t brightness);

#endif
