#ifndef LEDS_H
#define LEDS_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


extern uint8_t red, green;


void leds_init(void);
void leds_int(void);


#ifdef __cplusplus
}
#endif


#endif
