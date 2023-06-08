#include "angle.hpp"
#include <stdint.h>
#include "serial.hpp"
#include <math.h>

CAngle::CAngle(int teeth_total, int teeth_gap, unsigned long ts_resolution) {
  m_edge_count = (teeth_total - teeth_gap) * 2 - 1;
  m_minimal_edge_interval = (ts_resolution * 10) / (teeth_total * 2); // Assuming minimally one rotation per 10 seconds
  cshaft = new CCrankshaft(edge_cb, reset_cb, m_minimal_edge_interval, this);

  reset();
}

void CAngle::tick(unsigned long ts, int state) {
  cshaft->tick(ts, state);
}

int CAngle::angle(unsigned long ts) {
  if (m_last_edge_interval == 0 || m_last_edge_ts == 0) {
      return -1;
  }

  return 3600 * m_last_edge_id / m_edge_count + 3600 * (ts - m_last_edge_ts) / m_last_edge_interval / m_edge_count;
}

unsigned int CAngle::edge_delta(unsigned long ts) {
  unsigned long delta = ts - m_last_edge_ts;
  m_last_edge_ts = ts;
  if (delta < m_minimal_edge_interval) {
    return delta;
  } else {
    return m_minimal_edge_interval;
  }
}

void CAngle::edge(int id, unsigned long ts) {
  if (id >= m_edge_count) {
    return;
  }

  m_last_edge_id = id;
  if (m_last_edge_ts != 0) {
    m_last_edge_interval = ts - m_last_edge_ts;
  }

  m_last_edge_ts = ts;
}

void CAngle::reset() {
  Serial1.printf("Reset\n");
  m_last_edge_id = -1;
  m_last_edge_ts = 0;
  m_last_edge_interval = 0;
}

void CAngle::edge_cb(int id, unsigned long ts, void * arg) {
  ((CAngle *) arg )->edge(id, ts);
}

void CAngle::reset_cb(unsigned long ts, void * arg) {
  ((CAngle *) arg )->reset();
}
