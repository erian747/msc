#ifndef FLOWSENSOR_H
#define FLOWSENSOR_H
#include <stdint.h>
#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct flow_sensor_t flow_sensor_t;

flow_sensor_t *flowsensor_create(ioportid_t pin);


int flowsensor_getValue(flow_sensor_t *self);

#ifdef __cplusplus
}
#endif

#endif

