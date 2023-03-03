#include "leds.h"

#include <stm32f10x_gpio.h>


uint8_t red=0, green=0;


void leds_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void leds_int(void)
{
	if (red)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		red--;
	} else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
	}
	
	if (green)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		green--;
	} else
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
}
