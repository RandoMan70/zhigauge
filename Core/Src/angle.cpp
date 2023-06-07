#include "angle.hpp"
#include <stdint.h>

//    unsigned long a = (unsigned long) 3600 * (unsigned long) idx / (unsigned long) (teeth_total * 2);

CAngle::CAngle(int teeth_total, int teeth_gap, unsigned long ts_resolution) {
  m_edge_count = (teeth_total - teeth_gap) * 2 - 1;
  m_minimal_edge_interval = (ts_resolution * 10) / (teeth_total * 2); // Assuming minimally one rotation per 10 seconds
  cshaft = new CCrankshaft(edge_cb, reset_cb, m_minimal_edge_interval, this);
}

void CAngle::tick(unsigned long ts, int state) {
  cshaft->tick(ts, state);
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
}

void CAngle::reset(unsigned long ts) {
}

void CAngle::edge_cb(int id, unsigned long ts, void * arg) {
  ((CAngle *) arg )->edge(id, ts);
}

void CAngle::reset_cb(unsigned long ts, void * arg) {
  ((CAngle *) arg )->reset(ts);
}
