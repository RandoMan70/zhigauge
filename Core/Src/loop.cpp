#include "main.h"
#include <dwt_timer.h>
#include <be.hpp>

extern "C" void setup() {
  dwt_timer_init();
}

extern "C" void loop() {
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, state);
}
