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

    void clean()
    {
      m_enabled = false;
      m_start_time = 0;
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

    void tick()
    {
      int value = analogRead(m_pin);

      if (m_measure && (value >= m_max))
        m_time += micros() - m_prev_time;

      m_prev_time = micros();
    }

    void startMeasure()
    {
      m_time = 0;
      m_measure = true;
    }

    void endMeasure()
    {
      m_measure = false;
    }

    unsigned long getImpulseTime() const
    {
      return m_time;
    }

    void clean()
    {
      m_time = 0;
      m_measure = false;
    }

  private:
    byte m_pin;
    int m_max;
    int m_min;

    unsigned long m_time = 0;
    unsigned long m_prev_time = 0;

    bool m_measure;
};



class Voltmeter
{
  public:
    Voltmeter(byte pin, double r_coef = 99.56518) : m_pin(pin)
    {
      pinMode(m_pin, INPUT);

      m_coef = r_coef * (5.0 / 1024.0);
    }

    void tick()
    {
      m_value = analogRead(m_pin);
    }

    int getVoltage() const
    {
      return static_cast<int>((double)m_value * m_coef);
    }

  private:
    byte m_pin;
    int m_value;
    double m_coef;
};

#endif
