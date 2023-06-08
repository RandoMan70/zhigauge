#include "main.h"
#include <dwt_timer.h>
#include <be.hpp>
#include <serial.hpp>
#include <angle.hpp>
#include <timer.hpp>
#include <trigger.hpp>

#include <list>
#include <string>
#include <stdio.h>
#include <stdlib.h>

Timer *wTimer;
CAngle *crAngle;
Trigger *igTrigger;

int ignition_angle;
int o2sensor = 222;
int rpms = 1000;

//                             0         1         2         3         4         5         6         7
const char * be_report_mask = "*IXXX.X**iR200G200B200**OXXX.X**oR200G200B200**RXXXX**GXXX.X,XXX.X*";
char *be_report;
void writer(void *arg) {
  rpms = crAngle->rpm();
  char *mask = be_report;
  int angle = crAngle->angle(dwt_timer_get());
  if (angle < 0) {
      angle = 0;
  }

  mask[2] = (angle / 1000) % 10  + '0';
  mask[3] = (angle / 100) % 10 + '0';
  mask[4] = (angle / 10) % 10 + '0';
  mask[6] = (angle) % 10 + '0';

  mask[25] = (o2sensor / 1000) % 10 + '0';
  mask[26] = (o2sensor / 100) % 10 + '0';
  mask[27] = (o2sensor / 10) % 10 + '0';
  mask[29] = (o2sensor) % 10 + '0';

  mask[48] = (rpms / 1000) % 10 + '0';
  mask[49] = (rpms / 100) % 10 + '0';
  mask[50] = (rpms / 10) % 10 + '0';
  mask[51] = (rpms) % 10 + '0';

  mask[55] = mask[2];
  mask[56] = mask[3];
  mask[57] = mask[4];
  mask[59] = mask[6];

  mask[61] = mask[25];
  mask[62] = mask[26];
  mask[63] = mask[27];
  mask[65] = mask[29];

  Serial1.write(mask);
}

void ignition(uint32_t timestamp, void *) {
}

void ignition_timeout(void *) {
}

extern "C" void setup() {
  dwt_timer_init();
  HAL_UART_Transmit(&huart1, (uint8_t*)("Started\r\n"), 9, 1000);
  wTimer = new Timer(50, writer, NULL);
  crAngle = new CAngle(60, 2, dwt_timer_resolution());
  igTrigger = new Trigger(TRIGGER_FALLING, dwt_timer_resolution() / 5000, ignition, NULL, dwt_timer_resolution(), ignition_timeout, NULL);
  be_report = strdup(be_report_mask);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart == &huart1) {
    Serial1.done();
  }
}

extern "C" void loop() {
  unsigned long now = dwt_timer_get();
  GPIO_PinState state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, state);
  crAngle->tick(now, state);
  wTimer->tick();
  igTrigger->update(0, now);
  Serial1.flush();
  unsigned long t2 = dwt_timer_get();
//
//  if (t2-now > 1200) {
//      Serial1.printf("Too long: %lu\n", t2-now);
//  }
}

