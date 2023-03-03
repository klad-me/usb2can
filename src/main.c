#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <can.h>
#include <uart.h>
#include <leds.h>
#include <wdt.h>


#define LED_TX_TIME		20
#define LED_RX_TIME		20


#define LF		10
#define CR		13
#define BELL	7

static const char hex[]="0123456789ABCDEF";


static char txbuf[32], *txptr;

static char rxbuf[32];
static uint8_t rxpos=0;
static uint8_t *rxdata=(uint8_t*)rxbuf;

static bool isOpen=false;


static inline void txc(uint8_t c)
{
	(*txptr++)=c;
}


static inline void txh(uint8_t v)
{
	(*txptr++)=hex[v >> 4];
	(*txptr++)=hex[v & 0x0f];
}


static inline void txh1(uint8_t v)
{
	(*txptr++)=hex[v & 0x0f];
}


static bool unhex(uint8_t len)
{
	if (rxpos != 1+len) return false;
	
	uint8_t in=1, out=0;
	while (len--)
	{
		char c=rxbuf[in++];
		
		if (! isxdigit(c)) return false;
		
		if ( (c>='A') && (c<='F') ) c=c-'A'+10; else
		if ( (c>='a') && (c<='f') ) c=c-'a'+10; else
			c=c-'0';
		
		if ((out & 1) == 0)
			rxdata[out/2] =(uint8_t)c << 4; else
			rxdata[out/2]|=(uint8_t)c;
		
		out++;
	}
	
	return true;
}


static void handle_in(void)
{
	CAN_Message msg;
	char cmd=rxbuf[0];
	uint8_t l=rxpos;
	bool ok=false;
	
	txptr=txbuf;
	
	switch (cmd)
	{
		case 'S':
			// Setup standard bitrate
			if (unhex(1))
			{
				can_setup_S(rxdata[0] >> 4);
				txc(CR);
				ok=true;
			}
			break;
		
		case 's':
			// Setup custom bitrate
			if (unhex(4))
			{
				can_setup_BTR(rxdata[0], rxdata[1]);
				txc(CR);
				ok=true;
			}
			break;
		
		case 'O':
			// Open channel
			if (l==1)
			{
				isOpen=true;
				ok=true;
			}
			break;
		
		case 'C':
			// Close channel
			if (l==1)
			{
				isOpen=false;
				ok=true;
			}
			break;
		
		case 't':
			// Transmit standard frame
			if (unhex(l-1))
			{
				msg.DLC=rxdata[1] & 0x0f;
				if ( (msg.DLC*2 == (l-5)) && (msg.DLC <= 8) )
				{
					msg.ID=(rxdata[0] << 4) | (rxdata[1] >> 4);
					memcpy(msg.data, rxdata+2, msg.DLC);
					msg.IDE=0;
					msg.RTR=0;
					if (can_tx(&msg))
					{
						red=LED_TX_TIME;
						txc('z'); txc(CR);
						ok=true;
					}
				}
			}
			break;
		
		case 'T':
			// Transmit extended frame
			if (unhex(l-1))
			{
				msg.DLC=rxdata[4] >> 4;
				if ( (msg.DLC*2 == (l-10)) && (msg.DLC <= 8) )
				{
					msg.ID=(rxdata[0] << 24) | (rxdata[1] << 16) | (rxdata[2] << 8) | rxdata[3];
					for (uint8_t i=0; i<msg.DLC; i++)
					{
						msg.data[i]=(rxdata[4+i] << 4) | (rxdata[5+i] >> 4);
					}
					msg.IDE=1;
					msg.RTR=0;
					if (can_tx(&msg))
					{
						red=LED_TX_TIME;
						txc('Z'); txc(CR);
						ok=true;
					}
				}
			}
			break;
		
		case 'r':
			// Transmit standard RTR frame
			if ( (l==5) && (unhex(4)) )
			{
				msg.DLC=rxdata[1] & 0x0f;
				msg.ID=(rxdata[0] << 4) | (rxdata[1] >> 4);
				msg.IDE=0;
				msg.RTR=1;
				if (can_tx(&msg))
				{
					red=LED_TX_TIME;
					txc('z'); txc(CR);
					ok=true;
				}
			}
			break;
		
		case 'R':
			// Transmit extended RTR frame
			if ( (l==10) && (unhex(9)) )
			{
				msg.DLC=rxdata[4] >> 4;
				msg.ID=(rxdata[0] << 24) | (rxdata[1] << 16) | (rxdata[2] << 8) | rxdata[3];
				msg.IDE=1;
				msg.RTR=1;
				if (can_tx(&msg))
				{
					red=LED_TX_TIME;
					txc('Z'); txc(CR);
					ok=true;
				}
			}
			break;
		
		case 'F':
			// Read Status Flags
			if (l==1)
			{
				txc('F'); txh(0x00); txc(CR);
				ok=true;
			}
			break;
		
		case 'M':
		case 'm':
			// Set Acceptance Code/Mask Register (unsupported)
			txc(CR);
			ok=true;
			break;
		
		case 'V':
			// Get Version
			txc('V'); txh(0x01); txc(CR);
			ok=true;
			break;
		
		case 'N':
			// Get Serial Number
			txc('N'); txh(0x12); txh(0x34); txc(CR);
			ok=true;
			break;
	}
	
	if (! ok) txc(BELL);
	
	// Отправляем ответ
	uart_tx((const uint8_t*)txbuf, (int)(txptr-txbuf));
}


void main_program(void)
{
	while (1)
	{
		CAN_Message msg;
		
		// Принимаем из CAN
		while ( (uart_tx_free() > 32) && (can_rx(&msg)) )
		{
			green=LED_RX_TIME;
			
			txptr=txbuf;
			
			txc(CR);
			if (! msg.RTR)
			{
				if (! msg.IDE)
				{
					txc('t');
					txh1(msg.ID >> 8);
					txh(msg.ID & 0xff);
					txh1(msg.DLC);
					for (uint8_t i=0; i<msg.DLC; i++)
						txh(msg.data[i]);
					txc(CR);
				} else
				{
					txc('T');
					txh(msg.ID >> 24);
					txh(msg.ID >> 16);
					txh(msg.ID >> 8);
					txh(msg.ID & 0xff);
					txh1(msg.DLC);
					for (uint8_t i=0; i<msg.DLC; i++)
						txh(msg.data[i]);
					txc(CR);
				}
			} else
			{
				if (! msg.IDE)
				{
					txc('r');
					txh1(msg.ID >> 8);
					txh(msg.ID & 0xff);
					txh1(msg.DLC);
					txc(CR);
				} else
				{
					txc('R');
					txh(msg.ID >> 24);
					txh(msg.ID >> 16);
					txh(msg.ID >> 8);
					txh(msg.ID & 0xff);
					txh1(msg.DLC);
					txc(CR);
				}
			}
			
			uart_tx((const uint8_t*)txbuf, (int)(txptr-txbuf));
		}
		
		
		// Принимаем из UART
		char tmp[128];
		uint8_t rx;
		while ( (rx=uart_rx((uint8_t*)tmp, sizeof(tmp))) > 0 )
		{
			char *p=tmp;
			while (rx--)
			{
				char c=(*p++);
				
				if ( (c==CR) || (c==LF) )
				{
					// Конец строки
					if (rxpos > 0)
					{
						rxbuf[rxpos]=0;
						handle_in();
						rxpos=0;
					}
				} else
				{
					// Просто символ
					if (rxpos < sizeof(rxbuf)-1)
						rxbuf[rxpos++]=c;
				}
			}
		}
		
		
		// Пинаем собаку
		wdt_reset();
	}
}
