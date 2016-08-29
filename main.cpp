#define MODULE_NAME MAIN
#include <stdio.h>
#include "bsp.h"
#include "onewire.h"
#include "ds1820b.h"
#include "trace.h"
#include "heater.h"
#include "gui.h"
#include "ch.h"
#include "hal.h"
#include "jansson.h"
#include "oneWire.h"
#include <time.h>
#include "regulator.h"
#include "flowsensor.h"
#include "io_ifc.h"


static OneWireDriver owDrv;
static const OneWireConfig owCfg = { .dqPort = GPIOC,
				     .dqPad =  GPIOC_PIN6,
				     .dqAlternate = 8,
				     .uartd = &UARTD6 };


/**
  * @brief Program entry point
  * @param None
  * @retval None
  */




static OneWireRomAddress searchRom;

int main(void)
{
  halInit();
  chSysInit();

  gui_init();


  // Init OneWire interface
  oneWireInit(&owDrv, &owCfg);


  uint8_t resb = oneWireSearchRom(&owDrv, false, &searchRom, 1);

  bool_t res = oneWireReadRom (&owDrv, &searchRom);
	if(res)
  {
    ds1820BInit (&owDrv,&searchRom, 12);
  }

  //chprintf(chp, "read rom : 0x%x 0x%x 0x%x 0x%x     0x%x 0x%x 0x%x 0x%x %s\r\n",
  //rom.addr[0], rom.addr[1], rom.addr[2], rom.addr[3], rom.addr[4],
  //rom.addr[5], rom.addr[6], rom.addr[7],
  //res ? "SUCCESS" : "FAILED");

  io_ifc_init();
  Regulator regulator(0.25);
  Heater heater(GPIOA, GPIOA_PIN8);
  heater.setValue(0.5);
  regulator.setPoint(65);
  //flowsensor_create(0);

while(1) {


		//if(res)
    //{
	    ds1820BAskTemp(&owDrv, &searchRom);
	    const float temp = ds1820BGGetTempFromRam(&owDrv, &searchRom);
      io_ifc_sensor_data(temp);
      //gui_graph_new_value(temp);
      float heaterDuty;
	    // Regulate temperature in mash state
	    if(io_ifc_mode() == 0)
      {
        heaterDuty = io_ifc_ref();
      }
      else // if temperature setpoint
      {
        regulator.setPoint(io_ifc_ref());
        regulator.input(temp);
        heaterDuty = regulator.output();
      }

      heater.setValue(heaterDuty);
      chThdSleepMilliseconds(800);
    //}

	}
  return 0;
}

