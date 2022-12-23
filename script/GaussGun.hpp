#ifndef GAUSSGUN_CLS
#define GAUSSGUN_CLS

#include <Arduino.h>

class Coil
{
  public:
    Coil(byte pin) : m_pin(pin)
    {
      pinMode(pin, OUTPUT);
      setOff();
    }

    void setOn()
    {
      digitalWrite(m_pin, HIGH);

      m_start_time = micros();
      m_enabled = true;
    }

    void setOff()
    {
      digitalWrite(m_pin, LOW);

      m_start_time = 0;
      m_enabled = false;
    }

    bool isEnabled() const
    {
      return m_enabled;
    }

    unsigned long getStartTime() const
    {
      return m_start_time;
    }

  private:
    byte m_pin;
    bool m_enabled;
    unsigned long m_start_time;
};



class Sensor
{
  public:
    Sensor(byte pin, int min_limit = 999, int max_limit = 1000) : m_pin(pin), m_max(max_limit), m_min(min_limit)
    {
      pinMode(m_pin, INPUT_PULLUP);
    }

    void upd()
    {
      int value = analogRead(m_pin);
      
      if ((value >= m_max) && (! m_triggered))
      {
        m_start_time = micros();
        m_triggered = true;
      } else if ((value <= m_min) && m_triggered)
      {
        m_end_time = micros();
        m_triggered = false;
      }
      
    }

    bool isTriggered() const
    {
      return m_triggered;
    }

    unsigned long getEndTime() const
    {
      return m_end_time;
    }

    unsigned long getStartTime() const
    {
      return m_start_time;
    }

  private:
    byte m_pin;
    unsigned long m_start_time;
    unsigned long m_end_time;
    int m_max;
    int m_min;

    bool m_triggered;
};

#endif
