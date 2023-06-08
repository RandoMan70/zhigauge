#define TRIGGER_RISING 0
#define TRIGGER_FALLING 1

typedef void (*TriggerCb)(uint32_t timestamp, void *arg) ;
typedef void (*TimeoutCb)(void *arg) ;

enum State {state_waiting, state_verifying, state_triggered, state_timeout};

class Trigger {
public:
  Trigger(int edge, uint32_t pulse_length, TriggerCb cb, void *arg, uint32_t timeout, TimeoutCb timeout_cb, void * timeout_arg) {
    m_edge = edge;
    m_pulse_length = pulse_length;
    m_trigger_cb = cb;
    m_trigger_arg = arg;
    m_timeout = timeout;
    m_timeout_cb = timeout_cb;
    m_timeout_arg = timeout_arg;

    m_timeout_start_timestamp = 0;
  }

  void update(int value, uint32_t timestamp) {
    switch (m_state) {
      case state_waiting:
        waiting(value, timestamp);
        break;
      case state_verifying:
        verifying(value, timestamp);
        break;
      case state_triggered:
        triggered(value, timestamp);
        break;
      case state_timeout:
        timeout(value, timestamp);
        break;
    }

    m_value = value;
  }

private:
  bool pulse(int value) {
    return m_edge == TRIGGER_FALLING ? value == 0 : value != 0;
  }

  bool activity(int value) {
    return value != m_value;
  }

  void switch_waiting(uint32_t timestamp) {
    m_state = state_waiting;
    m_pulse_start_timestamp = 0;
    m_timeout_start_timestamp = timestamp;
  }

  void switch_verifying(uint32_t timestamp) {
    m_state = state_verifying;
    m_pulse_start_timestamp = timestamp;    
  }

  void switch_trigger(uint32_t timestamp) {
    m_state = state_triggered;
    m_trigger_cb(m_pulse_start_timestamp, m_trigger_arg);
  }

  void switch_timeout(uint32_t timestamp) {
    m_state = state_timeout;
    m_timeout_cb(m_timeout_arg);
  }

  void waiting(int value, uint32_t timestamp) {
    if (pulse(value)) {
      switch_verifying(timestamp);
      return;
    }

    if (activity_timeout(timestamp)) {
      switch_timeout(timestamp);
      return;
    }
  }

  void verifying(int value, uint32_t timestamp) {
    if (!pulse(value)) {
      switch_waiting(timestamp);
      return;
    }

    if (pulse_assured(timestamp)) {
      switch_trigger(timestamp);
      return;
    }

    if (activity_timeout(timestamp)) {
      switch_timeout(timestamp);
      return;
    }
}

  void triggered(int value, uint32_t timestamp) {
    if (!pulse(value)) {
      switch_waiting(timestamp);
      return;
    }

    if (activity_timeout(timestamp)) {
      switch_timeout(timestamp);
      return;
    }
  }

  void timeout(int value, uint32_t timestamp) {
    if (activity(value)) {
      switch_waiting(timestamp);
      return;
    }
  }

  bool activity_timeout(uint32_t timestamp) {
    return timestamp - m_timeout_start_timestamp > m_timeout;
  }

  bool pulse_assured(uint32_t timestamp) {
    return timestamp - m_pulse_start_timestamp > m_pulse_length;
  }

private:
  void trigger() {
    m_trigger_cb(m_pulse_start_timestamp, m_trigger_arg);
  }

  int m_edge;
  uint32_t m_pulse_length;
  TriggerCb m_trigger_cb;
  void * m_trigger_arg;
  uint32_t m_timeout;
  TimeoutCb m_timeout_cb;
  void * m_timeout_arg;

  State m_state = state_timeout;
  uint32_t m_pulse_start_timestamp = 0;
  int m_value = 0;

  uint32_t m_timeout_start_timestamp = 0;
  int m_timeout_done = 0;

};
