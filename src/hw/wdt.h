#ifndef WDT_H
#define WDT_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


void wdt_init(uint32_t ms);
void wdt_reset(void);


#ifdef __cplusplus
}
#endif


#endif
