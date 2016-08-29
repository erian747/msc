#define MODULE_NAME HEATER
#include "heater.h"
#include "ch.h"


#define RELAY_TOGGLE_TIME_MS 100
enum
{
  STATE_OFF = 0,
  STATE_ON = 1,
};

static struct VirtualTimer vt;

void Heater::timer_handler(void *ctx)
{
  Heater *self = (Heater *)ctx;
  int delayTimeMs = self->heater_timer_trigg();
  chSysLockFromIsr();
  chVTSetI(&vt, MS2ST(delayTimeMs), timer_handler, self);
  chSysUnlockFromIsr();


}




Heater::Heater(ioportid_t triacPort, uint8_t triacPin)
{
  palSetPadMode (triacPort,triacPin, PAL_MODE_OUTPUT_OPENDRAIN);
  this->mTriacPort = triacPort;
  this->mTriacPin = triacPin;
  mState = STATE_OFF;
  chVTSet(&vt, MS2ST(1000), timer_handler, this);
}

int Heater::heater_timer_trigg(void)
{
  int delayTimeMs = PWM_TIME_MS;

  switch(mState)
  {
  case STATE_OFF :
    onTime = (PWM_TIME_MS * dutyCycle) / 100;
    if(onTime > 0)
    {
      heater_triac_control(true);
      mState = STATE_ON;
      delayTimeMs = onTime;
    }
  break;


  case STATE_ON :
    offTime = (PWM_TIME_MS * (100-dutyCycle)) / 100;
    if(offTime > 0)
    {
      heater_triac_control(false);
      mState = STATE_OFF;
      delayTimeMs = offTime;
    }
  break;

  }
  return delayTimeMs;
}



void Heater::heater_triac_control(bool newState)
{
  if (newState == mTriacOnPolarity)
    palSetPad(mTriacPort, mTriacPin);
  else
    palClearPad(mTriacPort, mTriacPin);

}

void Heater::heater_relay_control(bool newState)
{
}
