#ifndef __REGULAR_TIMER_HEADER
#define __REGULAR_TIMER_HEADER

#include "dwt_timer.h"

class Timer {
public:
  Timer(unsigned long interval_ms, void(*cb)(void *arg), void *arg) : m_cb(cb), m_cb_arg(arg) {
    m_interval = (dwt_timer_resolution() / 1000) * interval_ms;
    m_started = dwt_timer_get();

  }
  void tick() {
    unsigned long now = dwt_timer_get();
    if (now - m_started >= m_interval) {
	m_cb(m_cb_arg);
	m_started = now;
    }
  }
private:
  unsigned long m_started;
  unsigned long m_interval;
  void(*m_cb)(void *arg);
  void *m_cb_arg;
};

#endif  //  __REGULAR_TIMER_HEADER
