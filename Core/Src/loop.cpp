#include "main.h"
#include <dwt_timer.h>
#include <be.hpp>
#include <serial.hpp>
#include <angle.hpp>
#include <timer.hpp>

#include <list>
#include <string>
#include <stdio.h>

Timer *wTimer;
CAngle *crAngle;

void writer(void *arg) {
  int angle = crAngle->angle(dwt_timer_get());
  if (angle > 36000) {
      Serial1.write("     !!!!!        ");
  }
  char buf[16];
  buf[0] = (angle / 1000) % 10  + '0';
  buf[1] = (angle / 100) % 10 + '0';
  buf[2] = (angle / 10) % 10 + '0';
  buf[3] = '.';
  buf[4] = (angle) % 10 + '0';
  buf[5] = '\n';
  buf[6] = '\0';
  Serial1.write(buf);
}

extern "C" void setup() {
  dwt_timer_init();
  HAL_UART_Transmit(&huart1, (uint8_t*)("Started\r\n"), 9, 1000);
  wTimer = new Timer(100, writer, NULL);
  crAngle = new CAngle(60, 2, dwt_timer_resolution());
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &huart1) {
    Serial1.done();
  }
}

extern "C" void loop() {
  unsigned long t1 = dwt_timer_get();
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, state);
  crAngle->tick(dwt_timer_get(), state);
  wTimer->tick();
  Serial1.flush();
  unsigned long t2 = dwt_timer_get();

  if (t2-t1 > 1500) {
      Serial1.printf("Too long: %lu\n", t2-t1);
  }
}

