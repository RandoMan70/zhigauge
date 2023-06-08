#ifndef __SERIAL_ASYNC_WRITER_HEADER
#define __SERIAL_ASYNC_WRITER_HEADER

#include "main.h"
#include <stdio.h>
#include <stdarg.h>

class SerialASyncWriter {
public:
  SerialASyncWriter(UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma_usart) :
    m_busy(false), m_huart(huart), m_hdma_usart(hdma_usart), m_write_idx(0), m_read_idx(0) {}
  void write(const char *s) {
    unsigned char idx = m_write_idx;
    do {
	if (*s == '\0') {
	    break;
	}
	m_buffer[idx ++] = *(s++);
    } while (idx != m_read_idx);
    if (idx == m_read_idx) {
	return;
    }
    m_write_idx = idx;
    flush();
  }
  void printf(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(m_print_buffer, sizeof(m_print_buffer), fmt, args);
    va_end(args);
    write(m_print_buffer);
  }
  void flush() {
    if (m_busy || m_read_idx == m_write_idx) {
	return;
    } else if (m_read_idx < m_write_idx) {
	HAL_UART_Transmit_DMA(m_huart, (uint8_t *)(m_buffer + m_read_idx), m_write_idx - m_read_idx);
	m_read_idx = m_write_idx;
	m_busy = true;
    } else if (m_read_idx > m_write_idx) {
	HAL_UART_Transmit_DMA(m_huart, (uint8_t *)(m_buffer + m_read_idx), 255 - m_read_idx + 1);
	m_read_idx = 0;
	m_busy = true;
    }
  }
  void done() {
    m_busy = false;
  }
private:
  bool m_busy;
  UART_HandleTypeDef *m_huart;
  DMA_HandleTypeDef *m_hdma_usart;

  char m_print_buffer[UINT8_MAX + 1];
  char m_buffer[UINT8_MAX + 1];
  uint8_t m_write_idx;
  uint8_t m_read_idx;
};

extern SerialASyncWriter Serial1;
extern SerialASyncWriter Serial2;

#endif  //  __SERIAL_ASYNC_WRITER_HEADER

