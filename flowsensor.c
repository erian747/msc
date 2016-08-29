
#include "ch.h"
#include "flowsensor.h"



// For YF-S201  F = 7.5 * Q (L / Min)
#define PULSES_PER_LITER 5
#define CALC_PERIOD_MS 1000
#define BSP_FLOW_SENSOR_TIMER 1

static flow_sensor_t *flow_sensor = 0;

struct flow_sensor_t
{
  ioportid_t pin;
  EXTDriver extDriver;
  volatile uint16_t count;
  uint16_t readings;
  uint32_t mlSecond;
};

static void extcb1(EXTDriver *extp, expchannel_t channel) {

  flow_sensor_t *self = flow_sensor;
  chSysLockFromIsr();
  self->count++;
  chSysUnlockFromIsr();
}

int flowsensor_getValue(flow_sensor_t *self)
{
  return self->mlSecond;

}

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};


static void gpt1cb(GPTDriver *gptp) {

  (void)gptp;
  flow_sensor_t *self = flow_sensor;
  self->mlSecond = self->count;
  self->count = 0;
}
/*
 * GPT2 configuration.
 */
static const GPTConfig gpt1cfg = {
  10000,    /* Timer clock.*/
  gpt1cb,        /* Timer callback.*/
  0
};

flow_sensor_t *flowsensor_create(ioportid_t pin)
{
  flow_sensor_t *self = (flow_sensor_t *)chHeapAlloc(0, sizeof(*self));
  flow_sensor = self;

  self->pin = pin;

  gptStart(&GPTD1, &gpt1cfg);
  gptStartContinuous(&GPTD1, 10000);

  extStart(&EXTD1, &extcfg);

  self->count = 0;
  self->readings = 0;
  return self;
}


