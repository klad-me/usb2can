#include "can.h"

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_can.h>
#include <stm32f10x_nvic.h>
#include <string.h>


#define RXQ_SIZE		32
static CanRxMsg rxq[RXQ_SIZE];
static uint8_t rxq_head=0, rxq_tail=0;

#define TXQ_SIZE		32
static CanTxMsg txq[TXQ_SIZE];
static uint8_t txq_head=0, txq_tail=0;

#define TX_TIMEOUT		100
static uint8_t tx_tmr[3]={0,0,0};

static CAN_InitTypeDef  CAN_InitStructure;


void can_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// Настраиваем CANRX (PA11)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Настраиваем CANTX (PA12)
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Включаем CAN
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);
	
	// Настраиваем CAN
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	can_setup_S(3);	// 100k
	
	// Настраиваем прерывания
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN_TX_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN_RX0_IRQChannel;
	NVIC_Init( &NVIC_InitStructure );
	
	NVIC_InitStructure.NVIC_IRQChannel = CAN_RX1_IRQChannel;
	NVIC_Init( &NVIC_InitStructure );
}


static void can_setup(void)
{	
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	
	// Сброс
	CAN_DeInit();
	
	// Настройка
	CAN_Init(&CAN_InitStructure);
	
	// Сообщения с ExtId
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0004;		// IDE должен был 1
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0004;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	// Сообщения с StdId
	CAN_FilterInitStructure.CAN_FilterNumber = 2;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;		// IDE должен был 0
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0004;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FilterFIFO1;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	// Разрешаем прерывания
	CAN_ITConfig(CAN_IT_FMP0, ENABLE);
	CAN_ITConfig(CAN_IT_FMP1, ENABLE);
}


void can_setup_S(uint8_t idx)
{
	// 24M / 16 / ( 1(sync)+11(bs1)+3(bs2) ) = 100k (sample time 80%)
	struct
	{
		uint16_t PS;
		uint8_t BS1, BS2;
	} list[] =
	{
		{ 160, CAN_BS1_11tq, CAN_BS2_3tq },	// 0: 10k
		{  80, CAN_BS1_11tq, CAN_BS2_3tq },	// 1: 20k
		{  32, CAN_BS1_11tq, CAN_BS2_3tq },	// 2: 50k
		{  16, CAN_BS1_11tq, CAN_BS2_3tq },	// 3: 100k
		{  12, CAN_BS1_12tq, CAN_BS2_3tq },	// 4: 125k
		{   6, CAN_BS1_12tq, CAN_BS2_3tq },	// 5: 250k
		{   3, CAN_BS1_12tq, CAN_BS2_3tq },	// 6: 500k
		{   2, CAN_BS1_11tq, CAN_BS2_3tq },	// 7: 800k
		{   2, CAN_BS1_9tq,  CAN_BS2_2tq },	// 8: 1M
	};
	
	if (idx > 8) return;
	
	CAN_InitStructure.CAN_Prescaler = list[idx].PS;
	CAN_InitStructure.CAN_SJW = CAN_SJW_4tq;
	CAN_InitStructure.CAN_BS1 = list[idx].BS1;
	CAN_InitStructure.CAN_BS2 = list[idx].BS2;
	
	can_setup();
}


void can_setup_BTR(uint8_t BTR0, uint8_t BTR1)
{
	CAN_InitStructure.CAN_Prescaler = ((BTR0 & 0x3f)+1)*2;	// умножение *2 есть в SJA1000
	CAN_InitStructure.CAN_SJW = (BTR0 >> 6) & 0x03;
	CAN_InitStructure.CAN_BS1 = BTR1 & 0x0f;
	CAN_InitStructure.CAN_BS2 = (BTR1 >> 4) & 0x07;
	
	can_setup();
}


void can_int(void)
{
	for (uint8_t i=0; i<3; i++)
	{
		if (tx_tmr[i] > 0) tx_tmr[i]--; else
		if (CAN_TransmitStatus(i) == CANTXPENDING)
		{
			// Таймаут передачи - убиваем пакет
			CAN_CancelTransmit(i);
			tx_tmr[i]=TX_TIMEOUT;
		}
	}
}


void USB_HP_CAN_TX_IRQHandler(void)
{
	// Можно передавать
	CAN_ClearITPendingBit(CAN_IT_TME);
	
	// Проверим - есть ли сообщение для передачи
	if (txq_head==txq_tail)
	{
		// Нечего передавать - отключаем прерывание
		CAN_ITConfig(CAN_IT_TME, DISABLE);
		return;
	}
	
	// Запускаем передачу
	uint8_t mbox=CAN_Transmit(&txq[txq_tail]);
	if (mbox <= 2) tx_tmr[mbox]=TX_TIMEOUT;
	
	// Сдвигаемся в очереди
	txq_tail=(txq_tail+1) % TXQ_SIZE;
}


void USB_LP_CAN_RX0_IRQHandler(void)
{
	// Принимаем из FIFO0
	CAN_ClearITPendingBit(CAN_IT_FMP0);
	
	uint8_t next=(rxq_head+1) % RXQ_SIZE;
	if (next != rxq_tail)
	{
		CAN_Receive(CAN_FIFO0, &rxq[rxq_head]);
		rxq_head=next;
	} else
	{
		CAN_FIFORelease(CAN_FIFO0);
	}
}


void CAN_RX1_IRQHandler(void)
{
	// Принимаем из FIFO1
	CAN_ClearITPendingBit(CAN_IT_FMP1);
	
	uint8_t next=(rxq_head+1) % RXQ_SIZE;
	
	if (next != rxq_tail)
	{
		CAN_Receive(CAN_FIFO1, &rxq[rxq_head]);
		rxq_head=next;
	} else
	{
		CAN_FIFORelease(CAN_FIFO1);
	}
}


bool can_tx(CAN_Message *msg)
{
	// Проверим место в очереди
	uint8_t next=(txq_head+1) % TXQ_SIZE;
	if (next==txq_tail) return false;
	
	// Копируем сообщение
	CanTxMsg *m=&txq[txq_head];
	if (msg->DLC > 8) msg->DLC=8;
	m->DLC=msg->DLC;
	if (msg->IDE)
	{
		m->StdId=(msg->ID >> 18) & 0x7FF;
		m->ExtId=msg->ID & 0x3FFFF;
		m->IDE=CAN_ID_EXT;
	} else
	{
		m->StdId=msg->ID & 0x7FF;
		m->IDE=CAN_ID_STD;
	}
	m->RTR=msg->RTR ? CAN_RTR_REMOTE : CAN_RTR_DATA;
	if (! msg->RTR) memcpy(m->Data, msg->data, msg->DLC);
	
	uint32_t pri=__GetBASEPRI();
	__BASEPRICONFIG(15 << 4);
		
		// Двигаем очередь
		txq_head=next;
		
		// Берем первое сообщение из очереди
		m=&txq[txq_tail];
		
		// Пробуем отправить напрямую
		uint8_t mbox=CAN_Transmit(m);
		if (mbox <= 2)
		{
			// Отправилось
			tx_tmr[mbox]=TX_TIMEOUT;
			txq_tail=(txq_tail+1) % TXQ_SIZE;
		}
		
		// Разрешаем прерывание по завершению передачи
		CAN_ITConfig(CAN_IT_TME, ENABLE);
		
	__BASEPRICONFIG(pri);
	
	return true;
}


bool can_rx(CAN_Message *msg)
{
	// Проверим - есть ли что-то в очереди
	if (rxq_head == rxq_tail) return false;
	
	// Копируем
	CanRxMsg *m=&rxq[rxq_tail];
	msg->IDE=(m->IDE == CAN_ID_EXT);
	if (msg->IDE)
		msg->ID=(m->StdId << 18) | m->ExtId; else
		msg->ID=m->StdId;
	msg->RTR=(m->RTR == CAN_RTR_REMOTE);
	msg->DLC=m->DLC;
	if (m->RTR == CAN_RTR_DATA) memcpy(msg->data, m->Data, m->DLC);
	
	// Двигатаем указатель
	rxq_tail=(rxq_tail+1) % RXQ_SIZE;
	
	return true;
}
