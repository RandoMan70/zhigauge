#ifndef __DWT_TIMER_HEADER
#define __DWT_TIMER_HEADER

#include <stdint.h>

#define    DWT_CYCCNT    *(volatile uint32_t*)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t*)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t*)0xE000EDFC

void dwt_timer_init();
uint32_t dwt_timer_get();
void dwt_timer_delay(unsigned long ticks);
uint32_t dwt_timer_resolution();

#endif  //  __DWM_TIMER_HEADER
