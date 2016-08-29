#ifndef __HEATER_H__
#define __HEATER_H__

#include <stdint.h>

#include "hal.h"

#define PWM_TIME_MS 5000

class Heater
{
  static void timer_handler(void *arg);
public:
  Heater(ioportid_t triacPort, uint8_t triacPin);
  void enable(bool enabled);
  void setValue(float value)
  {
    int tmpDutyCycle = value*100;
    if(tmpDutyCycle > 100)
      tmpDutyCycle = 100;
    if(tmpDutyCycle < 0)
      tmpDutyCycle = 0;
    dutyCycle = tmpDutyCycle;
  }


private:
  void heater_triac_control(bool newState);
  void heater_relay_control(bool newState);
  int heater_timer_trigg(void);
  ioportid_t mTriacPort;
  uint8_t mTriacPin;
  bool mTriacOnPolarity = 0;
  int onTime = 0;
  int offTime = 0;
  int dutyCycle = 0;
  uint8_t mState;


};
#endif //__HEATER_H__
