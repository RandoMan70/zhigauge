// Support for Bluetooth Electronics dashboard https://keuwl.com/apps/bluetoothelectronics/
#ifndef __BLUETOOTH_ELECTRONICS_WRITERS_HEADER
#define __BLUETOOTH_ELECTRONICS_WRITERS_HEADER

#include "main.h"
#include "serial.hpp"
#include <string.h>
#include <string>

class BEGenericWriter {
public:
  explicit BEGenericWriter(SerialASyncWriter *writer){
    m_writer = writer;
  }
protected:
  void start(const char sign) {
    buf = "*";
    buf += sign;
  }
  void append(const char c) {
    buf += c;
  }
  void append(const char *c) {
    buf += c;
  }
  void append(std::string s) {
    buf += s;
  }
  void commit() {
    buf += "*";
    m_writer->write(buf.c_str());
    buf.clear();
  }
  void reserve(int value) {
    buf.reserve(value);
  }
private:
  std::string buf;
  SerialASyncWriter *m_writer;
};

class BEValue : public BEGenericWriter {
public:
  BEValue(SerialASyncWriter *writer, const char sign) : BEGenericWriter(writer) {
    m_sign = sign;
    reserve(16);
  };
  void set(std::string s) {
    if (s == m_text) {
      return;
    }
    start(m_sign);
    append(s);
    commit();
    m_text = s;
  }
  void set(int v) {
    sprintf(m_buffer, "%i", v);
    set(m_buffer);
  }
  void set(long int v) {
    sprintf(m_buffer, "%li", v);
    set(m_buffer);
  }
  void set(unsigned int v) {
    sprintf(m_buffer, "%u", v);
    set(m_buffer);
  }
  void set(unsigned long int v) {
    sprintf(m_buffer, "%lu", v);
    set(m_buffer);
  }
  const char * value() {
    return m_text.c_str();
  }
private:
  char m_buffer[32];
  char m_sign;
  std::string m_text;
};

class BEColor{
public:
  BEColor() {
    r = 0;
    g = 0;
    b = 0;
  }
  BEColor(int r_value, int g_value, int b_value) {
    r = r_value;
    g = g_value;
    b = b_value;
  }
  bool operator == (BEColor &other) {
    return r == other.r && g == other.g && b == other.b;
  }
  int r, g, b;
};

class BELamp : BEGenericWriter {
public:
  BELamp(SerialASyncWriter *writer, const char sign) : BEGenericWriter(writer) {
    m_sign = sign;
    reserve(16);
  }
  void set(BEColor color) {
    if (color == m_color) {
      return;
    }
    start(m_sign);
    append('R');
    append(color.r);
    append('G');
    append(color.g);
    append('B');
    append(color.b);
    commit();
    m_color = color;
  }
private:
  char m_sign;
  BEColor m_color;
};

class BEGraph : BEGenericWriter {
public:
  BEGraph(SerialASyncWriter *writer, const char sign, int count) : BEGenericWriter(writer) {
    m_sign = sign;
    m_count = count;
    m_values = new float[count];
    reserve(count * 6 + 3);
  }
  void update(int idx, float value) {
    if (idx < 0 || idx >= m_count) {
      return;
    }
    m_values[idx] = value;
  }
  void flush() {
    start(m_sign);
    for(int idx = 0; idx < m_count; idx++) {
      if (idx != 0) {
        append(",");
      }
      append(m_values[idx]);
    }
    commit();
  }
  ~BEGraph() {
    delete m_values;
  }
private:
  char m_sign;
  int m_count;
  float *m_values;
};

#endif  //  __BLUETOOTH_ELECTRONICS_WRITERS_HEADER

