#include "wdt.h"

#include <stm32f10x_iwdg.h>



void wdt_init(uint32_t ms)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	IWDG_SetPrescaler(IWDG_Prescaler_256);		// 40000/256=156.25/sec
	uint32_t v=(uint32_t)( (float)ms / 1000.0 * 156.25 + 0.5);
	if (v > 0xfff) v=0xfff;
	IWDG_SetReload(v);
	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	
	IWDG_ReloadCounter();
	IWDG_Enable();
}


void wdt_reset(void)
{
	IWDG_ReloadCounter();
}
