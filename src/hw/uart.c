#include "uart.h"

#include <string.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_nvic.h>


#define TXQ_SIZE	1024
#define RXQ_SIZE	1024


static uint8_t txq[TXQ_SIZE], rxq[RXQ_SIZE];
static uint16_t txq_head=0, txq_tail=0, rxq_tail=0;

static DMA_InitTypeDef DMA_InitStructure;
static volatile uint8_t tx_dma_running=0;


void uart_init(uint32_t baud)
{
	GPIO_InitTypeDef		GPIO_InitStructure;
	USART_InitTypeDef		USART_InitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;
	
	// Enable USART1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// Configure USART1 Tx (PA9) as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure USART1 Rx (PA10) as input
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure USART1
	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Clock=USART_Clock_Disable;
	USART_InitStructure.USART_CPOL=USART_CPOL_Low;
	USART_InitStructure.USART_CPHA=USART_CPHA_2Edge;
	USART_InitStructure.USART_LastBit=USART_LastBit_Disable;
	USART_Init(USART1, &USART_InitStructure);
	
	// Configure RX DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
	DMA_DeInit(DMA_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxq;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = sizeof(rxq);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA_Channel5, &DMA_InitStructure);
	
	// Configure TX DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA, ENABLE);
	DMA_DeInit(DMA_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	//DMA_InitStructure.DMA_MemoryBaseAddr не установлен
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize не установлен
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// Настраиваем прерывание от DMA передачи
	NVIC_InitStructure.NVIC_IRQChannel = DMAChannel4_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	DMA_ITConfig(DMA_Channel4, DMA_IT_TC, ENABLE);
	
	// Enable USART & RX DMA
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART1, ENABLE);
	DMA_Cmd(DMA_Channel5, ENABLE);
}


uint16_t uart_rx(uint8_t *buf, uint16_t size)
{
	uint16_t rxq_head = RXQ_SIZE - DMA_GetCurrDataCounter(DMA_Channel5);
	uint16_t rx=0;
	
	while ( (rxq_head != rxq_tail) && (size > 0) )
	{
		uint16_t l=(rxq_head > rxq_tail) ? (rxq_head - rxq_tail) : (RXQ_SIZE - rxq_tail);
		if (l > size) l=size;
		
		memcpy(buf, rxq+rxq_tail, l);
		buf+=l;
		rx+=l;
		size-=l;
		rxq_tail=(rxq_tail+l) & (RXQ_SIZE-1);
	}
	
	return rx;
}


uint16_t uart_rx_avail(void)
{
	int rxq_head = RXQ_SIZE - DMA_GetCurrDataCounter(DMA_Channel5);
	
	int avail=rxq_head - (int)rxq_tail;
	if (avail < 0) avail+=RXQ_SIZE;
	
	return avail;
}


static void arm_dma_tx(void)
{
	// Проверяем - есть ли что передавать
	if (txq_head == txq_tail) return;
	
	// Получаем размер данных для передачи
	uint16_t l=(txq_tail > txq_head) ? (TXQ_SIZE - txq_tail) : (txq_head - txq_tail);
	
	// Настраиваем DMA
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)(txq+txq_tail);
	DMA_InitStructure.DMA_BufferSize = l;
	DMA_Init(DMA_Channel4, &DMA_InitStructure);
	
	// Ставим флаг, что DMA работает
	tx_dma_running=1;
	
	// Запускаем передачу
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA_Channel4, ENABLE);
}


void DMAChannel4_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA_IT_TC4)==SET)
	{
		// Конец передачи
		USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
		DMA_Cmd(DMA_Channel4, DISABLE);
		DMA_ClearITPendingBit(DMA_IT_TC4);
		tx_dma_running=0;
		
		// Смещаем указатель FIFO
		txq_tail=(txq_tail + DMA_InitStructure.DMA_BufferSize) & (TXQ_SIZE-1);
		
		// Перезапускаем передачу, если надо
		arm_dma_tx();
	}
}


uint16_t uart_tx(const uint8_t *buf, uint16_t size)
{
	if (size == 0) return 0;
	
	uint16_t tx=0;
	
	// Заполняем буфер передачи
	while ( (size > 0) && (((txq_head+1) & (TXQ_SIZE-1)) != txq_tail) )
	{
		uint16_t l=(txq_head >= txq_tail) ? (TXQ_SIZE - txq_head) : (txq_tail - txq_head - 1);
		if (txq_tail == 0) l--;
		if (l > size) l=size;
		
		memcpy(txq+txq_head, buf, l);
		buf+=l;
		tx+=l;
		size-=l;
		txq_head=(txq_head+l) & (TXQ_SIZE-1);
	}
	
	// Запускаем DMA, если требуется
	if (! tx_dma_running) arm_dma_tx();
	
	return tx;
}


uint16_t uart_tx_free(void)
{
	int used=(int)txq_head - (int)txq_tail;
	if (used < 0) used+=TXQ_SIZE;
	
	return (TXQ_SIZE-1) - used;
}
