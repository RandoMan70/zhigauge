#include "main.h"
#include "serial.hpp"

SerialASyncWriter Serial1(&huart1, &hdma_usart1_tx);
SerialASyncWriter Serial2(&huart2, &hdma_usart2_tx);
