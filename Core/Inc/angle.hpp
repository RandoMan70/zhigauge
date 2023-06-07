#ifndef IGNITION_ANGLE_HPP
#define IGNITION_ANGLE_HPP

#include "crankshaft.hpp"

class CAngle {
  public:
    CAngle(int teeth_total, int teeth_gap, unsigned long ts_resolution);
    void tick(unsigned long ts, int state);
  private:
    void edge(int id, unsigned long ts);
    void reset(unsigned long ts);
    unsigned int edge_delta(unsigned long ts);
    static void edge_cb(int id, unsigned long ts, void * arg);
    static void reset_cb(unsigned long ts, void * arg);

    int m_edge_count;
    unsigned long m_timer_resolition;
    unsigned long m_last_edge_ts;
    unsigned long m_last_turn_ts;
    unsigned long m_minimal_edge_interval;

    CCrankshaft *cshaft;
};

#endif // IGNITION_ANGLE_HPP
