#ifndef OS_CAN1_H
#define OS_CAN1_H


#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef struct CAN_Message
{
	uint32_t ID;
	uint8_t  IDE;
	uint8_t  RTR;
	uint8_t  DLC;
	uint8_t  data[8];
} CAN_Message;


void can_init(void);
void can_int(void);

void can_setup_S(uint8_t idx);
void can_setup_BTR(uint8_t BTR0, uint8_t BTR1);

bool can_tx(CAN_Message *msg);
bool can_rx(CAN_Message *msg);


#ifdef __cplusplus
}
#endif


#endif
