#include "timer.h"

#include <stm32f10x_systick.h>

#include <leds.h>
#include <can.h>


volatile uint32_t __ms_count=0;


void SysTick_Handler(void)
{
	__ms_count++;
	
	leds_int();
	can_int();
}


void timer_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	SysTick_SetReload(24000000/1000);
	SysTick_ITConfig(ENABLE);
	SysTick_CounterCmd(SysTick_Counter_Enable);
}
