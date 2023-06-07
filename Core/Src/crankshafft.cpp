#include "crankshaft.hpp"
#include "math.h"

CCrankshaft::CCrankshaft(edge_cb_t edge_cb, reset_cb_t reset_cb, unsigned long max_gap_interval, void * cb_arg) {
  m_edge_cb = edge_cb;
  m_reset_cb = reset_cb;
  m_cb_arg = cb_arg;
  m_max_gap_interval = max_gap_interval;
  reset(0);
};

void CCrankshaft::reset(unsigned long ts) {
  m_edge_id = -1;
  m_pin_state = -1;
  m_pin_ts = 0;
  
  for (int i = 0; i < 4; i ++) {
    m_recent[i] = 0;
  }

  if (m_reset_cb != NULL) m_reset_cb(ts, m_cb_arg);
}

void CCrankshaft::push(unsigned int value) {
  m_recent[3] = m_recent[2];
  m_recent[2] = m_recent[1];
  m_recent[1] = m_recent[0];
  m_recent[0] = value;
}

bool CCrankshaft::is_gap() {
  int tail_medium = (m_recent[2] + m_recent[3]) / 2;
  int tail_delta = abs(m_recent[2] - m_recent[3]);

  // Too large difference in tail
  if (tail_delta >= tail_medium / 8) {
    return false;
  }

  int head_delta_0 = m_recent[0] - tail_medium;
  int head_delta_1 = m_recent[1] - tail_medium;

  // Head must be bigger
  if (head_delta_0 <= 0 || head_delta_1 <= 0) {
    return false;
  }

  // Gap segments must be at least 1.5 times bigger
  // TODO: Probably reduce comparison to tail_medium / 2
  if (head_delta_0 < tail_medium || head_delta_1 < tail_medium) {
    return false;
  }

  return true;
}


void CCrankshaft::tick(unsigned long ts, int state) {
  // No change - nothing to update
  if (state == m_pin_state) {
    return; 
  }

  // Pin state initialization
  if (m_pin_state == -1) {
    m_pin_state = state;
    m_pin_ts = ts;
    return;
  }

  // Get and filter segment duration
  unsigned long duration = ts - m_pin_ts;
  if (duration >= m_max_gap_interval || duration == 0) {
    reset(ts);
    return;
  }

  m_pin_state = state;
  m_pin_ts = ts;

  // Push valid duration to history
  push(duration);

  // Wait for fully filled history
  if (m_recent[3] == 0) {
    return;
  }

  // If it's a gap - initialize segment id and segments count
  if (is_gap()) {
    m_edge_id = 0;
    if (m_edge_cb != NULL) m_edge_cb(m_edge_id, ts, m_cb_arg);
    return;
  }   

  // Still waiting for initial gap
  if (m_edge_id < 0) {
    return;
  }

  // Having next segment
  m_edge_id += 1;
  if (m_edge_cb != NULL) m_edge_cb(m_edge_id, ts, m_cb_arg);
}
