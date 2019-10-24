#ifndef __DRIVER_FLASH_H__
#define __DRIVER_FLASH_H__

#include "stm32f0xx.h"

#define FLASH_PAGE_SIZE ((uint32_t)0x00000400)

extern uint8_t Flash_Write_Flag;

void Erase_Flash(uint32_t addr, uint8_t num_page);
void ProgramFlashWord(uint32_t addr, uint32_t data);
void Forbid_INT(void);
void Allow_INT(void);
void Flash_Data_Save(void);

#endif
