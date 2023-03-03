#ifndef TIMER_H
#define TIMER_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef uint32_t Timer_t;
#define T_set(tmr, value)	do { (tmr)=ms_count() + (uint32_t)(value); } while(0)
#define T_expired(tmr)		((((uint32_t)(ms_count() - (tmr))) & 0x80000000) == 0)
#define T_remains(tmr)		((int)((int32_t)(tmr) - (int32_t)ms_count()))


extern volatile uint32_t __ms_count;
#define ms_count()	(__ms_count)


void timer_init(void);


#ifdef __cplusplus
}
#endif


#endif
