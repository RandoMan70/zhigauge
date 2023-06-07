#ifndef IGNITION_CRANKSHAFT_HPP
#define IGNITION_CRANKSHAFT_HPP

class CCrankshaft {
  typedef void (*edge_cb_t)(int edge_id, unsigned long ts, void * arg);
  typedef void (*reset_cb_t)(unsigned long ts, void *arg);
  public:
    CCrankshaft(edge_cb_t edge_cb, reset_cb_t reset_cb, unsigned long max_gap_interval, void * cb_arg);
    void tick(unsigned long ts, int state);
  private:
    void push(unsigned int value);
    void reset(unsigned long ts);
    bool is_gap();

    int m_edge_count;
    int m_edge_id;
    int m_pin_state;
    unsigned long m_pin_ts;
    unsigned long m_max_gap_interval;
    int m_recent[4];
    edge_cb_t m_edge_cb;
    reset_cb_t m_reset_cb;
    void *m_cb_arg;
};

#endif // IGNITION_CRANKSHAFT_HPP
