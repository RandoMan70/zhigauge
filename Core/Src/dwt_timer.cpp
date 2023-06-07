#include <dwt_timer.h>
#include "stm32f1xx_hal.h"

void dwt_timer_init() {
  SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //Enable DWT
  DWT_CONTROL|= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t dwt_timer_get() {
	return DWT_CYCCNT;
}

void dwt_timer_delay(unsigned long ticks) {
	int started = DWT_CYCCNT;
	while (DWT_CYCCNT - started < ticks){};
}

uint32_t dwt_timer_resolution() {
	return HAL_RCC_GetHCLKFreq();
}
