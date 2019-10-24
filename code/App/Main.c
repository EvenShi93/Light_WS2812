#include "Driver_Button.h"
#include "Driver_Flash.h"
#include "Driver_Led.h"
#include "Driver_Pwmc.h"
#include "Driver_SysTime.h"
#include "Task.h"

int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOFEN;
	System_Time_Init();
	State_Led_Init();
	WS2812_Led_Init();
	Button_Init();
	Pwm_Capture_Init();
	Task_Time_Init();
	Task_Init(TASK_ID_WS2812, -1, 20, WS2812_Led_Func);
	Task_Init(TASK_ID_LED, -1, 200, State_Led_Func);
	Task_Init(TASK_ID_PWMC, -1, 20, Pwm_Capture_Func);
	Task_Init(TASK_ID_BUTTON, -1, 20, Button_Func);
	Task_Init(TASK_ID_FLASH, -1, 2000, Flash_Data_Save);
	while (1)
	{
		Task_Running();
	}
}
