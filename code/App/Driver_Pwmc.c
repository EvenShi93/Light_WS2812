#include "Driver_Pwmc.h"
#include "Driver_Flash.h"
#include "Driver_Led.h"
#include "Driver_SysTime.h"

uint16_t Pwm_Value;

void Pwm_Capture_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period = 22000;
	TIM_TimeBaseStructure.TIM_Prescaler = 47;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x5;
	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);

	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
}

void Pwm_Capture_Func(void)
{
	static uint8_t Pwm_state = 0;
	static uint32_t Pwm_LastTime = 0;
	switch (Pwm_state)
	{
	case 0:
		if (Pwm_Value < 1400)
		{
			Pwm_state = 1;
			Pwm_LastTime = Now_Time_Ms;
		}
		break;
	case 1:
		if (Pwm_Value > 1600)
		{
			Pwm_state = 2;
			Pwm_LastTime = Now_Time_Ms;
		}
		else if (Now_Time_Ms > Pwm_LastTime + 500)
			Pwm_state = 0;
		break;

	case 2:
		if (Pwm_Value < 1400)
		{
			Pwm_state = 3;
			Pwm_LastTime = Now_Time_Ms;
		}
		else if (Now_Time_Ms > Pwm_LastTime + 500)
			Pwm_state = 0;
		break;

	case 3:
		if (Pwm_Value > 1600)
		{
			Pwm_state = 4;
			Pwm_LastTime = Now_Time_Ms;
		}
		else if (Now_Time_Ms > Pwm_LastTime + 500)
		{
			WS2812_Para.Struct.Ws2812_Color = (WS2812_Para.Struct.Ws2812_Color + 1) % WS2812_Para.Struct.Ws2812_Color_Max;
			Flash_Write_Flag = 1;
			Pwm_state = 0;
		}
		break;

	case 4:
		if (Pwm_Value < 1400)
		{
			WS2812_Para.Struct.Ws2812_Mode = (WS2812_Para.Struct.Ws2812_Mode + 1) % MODE_NUM;
			Flash_Write_Flag = 1;
			Pwm_LastTime = Now_Time_Ms;
			Pwm_state = 0;
		}
		else if (Now_Time_Ms > Pwm_LastTime + 500)
			Pwm_state = 0;
		break;

	default:
		Pwm_state = 0;
		break;
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)
	{
		TIM3->SR = (uint16_t)~TIM_IT_CC1;
		Pwm_Value = TIM3->CCR2;
	}
}
