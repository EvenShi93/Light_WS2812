#include "Driver_Flash.h"
#include "Driver_Led.h"
#include "Driver_SysTime.h"
#include "string.h"

uint8_t Flash_Write_Flag;

void Erase_Flash(uint32_t addr, uint8_t num_page)
{
  uint8_t i = 0;
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  for (i = 0; i < num_page; i++)
  {
    FLASH_ErasePage(addr + (FLASH_PAGE_SIZE * i));
  }
}

void ProgramFlashWord(uint32_t addr, uint32_t data)
{
  if (FLASH_ProgramWord(addr, data) != FLASH_COMPLETE)
  {
    while (1)
    {
      STATE_LED_FLICKER;
      Delay_Ms(200);
    }
  }
}

void Forbid_INT(void)
{
  __set_PRIMASK(1);
  Delay_Ms(1);
}

void Allow_INT(void)
{
  Delay_Ms(1);
  __set_PRIMASK(0);
}

void Flash_Data_Save(void)
{
  if (Flash_Write_Flag)
  {
    Flash_Write_Flag = 0;
    Forbid_INT();
    FLASH_Unlock();
    Erase_Flash(0x08003C00, 1);
    for (uint16_t i = 0; i < 50; i++)
    {
      ProgramFlashWord(0x08003C00 + 4 * i, WS2812_Para.Word[i]);
    }
    FLASH_Lock();
    Allow_INT();
  }
}
