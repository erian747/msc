#ifndef BSP_H
#define BSP_H
#include <stdint.h>
//#include "mcal_gpio.h"

//#include "lcd/ssd2119.h"
 // #include "stm32f4_discovery_lcd.h"
//  extern lcd_t *ssd2119;



// PA
#define BSP_USER_BUTTON    GPIO_MAKEPIN(0, 0)
#define BSP_HEATER_TRIAC   GPIO_MAKEPIN(0, 8)
// PD
#define BSP_GREEN_LED      GPIO_MAKEPIN(3, 12)


void BSP_init(uint8_t mode);


#endif
