//*****************************************************************************
//   +--+	   
//   | ++----+   
//   +-++	|  
//	 |	 |  
//   +-+--+  |   
//   | +--+--+  
//   +----+	Copyright (c) 2009 Code Red Technologies Ltd. 
//
// Microcontroller Startup code for use with Red Suite
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 
//
//*****************************************************************************
#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
//
// Forward declaration of the default handlers.
//
//*****************************************************************************
void Reset_Handler(void);
void ResetISR(void) ALIAS(Reset_Handler);

void NMI_Handler(void) ALIAS(IntDefaultHandler);
void HardFault_Handler(void) ALIAS(IntDefaultHandler);
void MemManage_Handler(void) ALIAS(IntDefaultHandler);
void BusFault_Handler(void) ALIAS(IntDefaultHandler);
void UsageFault_Handler(void) ALIAS(IntDefaultHandler);
void DebugMon_Handler(void) ALIAS(IntDefaultHandler);
void SysTick_Handler(void) ALIAS(IntDefaultHandler);
void PendSV_Handler(void) ALIAS(IntDefaultHandler);
void SVC_Handler(void) ALIAS(IntDefaultHandler);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take 
// precedence over these weak definitions
//
//*****************************************************************************
void WDT_IRQHandler(void) ALIAS(IntDefaultHandler);
void WWDG_IRQHandler(void) ALIAS(IntDefaultHandler);
void PVD_IRQHandler(void) ALIAS(IntDefaultHandler);
void TAMPER_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) ALIAS(IntDefaultHandler);
void FLASH_IRQHandler(void) ALIAS(IntDefaultHandler);
void RCC_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI0_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI1_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI2_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI3_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI4_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel1_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel2_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel3_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel4_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel5_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel6_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMAChannel7_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_HP_CAN_TX_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_LP_CAN_RX0_IRQHandler(void) ALIAS(IntDefaultHandler);
void CAN_RX1_IRQHandler(void) ALIAS(IntDefaultHandler);
void CAN_SCE_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI9_5_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_BRK_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_UP_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_TRG_CCUP_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM1_CC_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM2_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM3_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM4_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_EV_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_ER_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C2_EV_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C2_ER_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI1_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI2_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART1_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART2_IRQHandler(void) ALIAS(IntDefaultHandler);
void USART3_IRQHandler(void) ALIAS(IntDefaultHandler);
void EXTI15_10_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTCAlarm_IRQHandler(void) ALIAS(IntDefaultHandler);
void USBWakeUp_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIM5_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI3_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART4_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART5_IRQHandler(void) ALIAS(IntDefaultHandler);




//*****************************************************************************
//
// The entry point for the C++ library startup
//
//*****************************************************************************
extern WEAK void __libc_init_array(void);

//*****************************************************************************
//
// The entry point for the application.
// __main() is the entry point for redlib based applications
// main() is the entry point for newlib based applications
//
//*****************************************************************************
extern WEAK void __main(void);
extern WEAK void main(void);
//*****************************************************************************
//
// External declaration for the pointer to the stack top from the Linker Script
//
//*****************************************************************************
extern char _vStackTop;

//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
		// Core Level - CM3
		(void *)&_vStackTop,					// The initial stack pointer
		Reset_Handler,							// The reset handler
		NMI_Handler,							// The NMI handler
		HardFault_Handler,						// The hard fault handler
		MemManage_Handler,						// The MPU fault handler
		BusFault_Handler,						// The bus fault handler
		UsageFault_Handler,						// The usage fault handler
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		0,										// Reserved
		SVC_Handler,							// SVCall handler
		DebugMon_Handler,						// Debug monitor handler
		0,										// Reserved
		PendSV_Handler,							// The PendSV handler
		SysTick_Handler,						// The SysTick handler

		// Chip Level - STM32F103
		WWDG_IRQHandler,						// 0
		PVD_IRQHandler,							// 1
		TAMPER_IRQHandler,						// 2
		RTC_IRQHandler,							// 3
		FLASH_IRQHandler,						// 4
		RCC_IRQHandler,							// 5
		EXTI0_IRQHandler,						// 6
		EXTI1_IRQHandler,						// 7
		EXTI2_IRQHandler,						// 8
		EXTI3_IRQHandler,						// 9
		EXTI4_IRQHandler,						// 10
		DMAChannel1_IRQHandler,					// 11
		DMAChannel2_IRQHandler,					// 12
		DMAChannel3_IRQHandler,					// 13
		DMAChannel4_IRQHandler,					// 14
		DMAChannel5_IRQHandler,					// 15
		DMAChannel6_IRQHandler,					// 16
		DMAChannel7_IRQHandler,					// 17
		ADC_IRQHandler,							// 18
		USB_HP_CAN_TX_IRQHandler,				// 19
		USB_LP_CAN_RX0_IRQHandler,				// 20
		CAN_RX1_IRQHandler,						// 21
		CAN_SCE_IRQHandler,						// 22
		EXTI9_5_IRQHandler,						// 23
		TIM1_BRK_IRQHandler,					// 24
		TIM1_UP_IRQHandler,						// 25
		TIM1_TRG_CCUP_IRQHandler,				// 26
		TIM1_CC_IRQHandler,						// 27
		TIM2_IRQHandler,						// 28
		TIM3_IRQHandler,						// 29
		TIM4_IRQHandler,						// 30
		I2C1_EV_IRQHandler,						// 31
		I2C1_ER_IRQHandler,						// 32
		I2C2_EV_IRQHandler,						// 33
		I2C2_ER_IRQHandler,						// 34
		SPI1_IRQHandler,						// 35
		SPI2_IRQHandler,						// 36
		USART1_IRQHandler,						// 37
		USART2_IRQHandler,						// 38
		USART3_IRQHandler,						// 39
		EXTI15_10_IRQHandler,					// 40
		RTCAlarm_IRQHandler,					// 41
		USBWakeUp_IRQHandler,					// 42
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++
// library.
//
//*****************************************************************************
void Reset_Handler(void)
{
	unsigned long *pulSrc, *pulDest;

	//
	// Copy the data segment initializers from flash to SRAM.
	//
	pulSrc = &_etext;
	for(pulDest = &_data; pulDest < &_edata; )
	{
		*pulDest++ = *pulSrc++;
	}

	//
	// Zero fill the bss segment.  This is done with inline assembly since this
	// will clear the value of pulDest if it is not kept in a register.
	//
	__asm("	ldr	 r0, =_bss\n"
		  "	ldr	 r1, =_ebss\n"
		  "	mov	 r2, #0\n"
		  "	.thumb_func\n"
		  "zero_loop:\n"
		  "		cmp	 r0, r1\n"
		  "		it	  lt\n"
		  "		strlt   r2, [r0], #4\n"
		  "		blt	 zero_loop");

	//
	// Call C++ library initilisation, if present
	//
		if (__libc_init_array)
				__libc_init_array() ;

		//
		// Call the application's entry point.
		// __main() is the entry point for redlib based applications (which calls main())
		// main() is the entry point for newlib based applications
		//
		if (__main)
				__main() ;
		else
				main() ;

		//
		// main() shouldn't return, but if it does, we'll just enter an infinite loop 
		//
		while (1) {
				;
		}
}


//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a handler
// is not present in the application code.
//
//*****************************************************************************
static void IntDefaultHandler(void)
{
	//
	// Go into an infinite loop.
	//
	while(1)
	{
	}
}
