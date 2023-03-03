#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_nvic.h>

#include <wdt.h>
#include <leds.h>
#include <uart.h>
#include <can.h>
#include <timer.h>



static void cpu_init(void)
{
#define SCB_CCR ((volatile unsigned long *)(0xE000ED14))
	*SCB_CCR = *SCB_CCR | 0x200; /* Set STKALIGN in NVIC */

	// 1. Cloking the controller from internal HSI RC (8 MHz)
	RCC_HSICmd(ENABLE);
	// wait until the HSI is ready
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
	// 2. Enable ext. high frequency OSC
	RCC_HSEConfig(RCC_HSE_ON);
	// wait until the HSE is ready
	while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);
	// 3. Init PLL
	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3); // 24MHz
	RCC_PLLCmd(ENABLE);
	// wait until the PLL is ready
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	// 4. Set system clock divders
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_Div1);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div1);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	// Flash 1 wait state
	*(vu32 *)0x40022000 = 0x12;
	// 5. Clock system from PLL
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	// Включаем порты
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	
	// Настраиваем NVIC
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}


extern void main_program(void);


int main(void)
{
	wdt_init(2000);
	
	cpu_init();
	leds_init();
	uart_init(460800);
	can_init();
	timer_init();
	
	main_program();
	return 0;
}
