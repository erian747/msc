#define MODULE_NAME BSP
#include <stdint.h>
#include "bsp.h"
#include "mcal.h"
#include "mcal_gpio.h"
#include "mcal_rtc.h"
#include "mcal_i2c.h"
#include "blf.h"
#include "lcd/ssd2119.h"

#define USE_STM32F4BB_LCD 1

typedef struct mgm_desc_t mgm_desc_t;

struct mgm_desc_t
{
  const char *type;
  const char *name;
  const char *dataType;
  const char *optional;
  const mgm_desc_t *parent;
  void (*setter)(void *self, uint32_t phase);

};

static void set_phase_angle(void *self, uint32_t phase)
{

}
// Object types
const char *mgmTypeSetter = {"setter"};
const char *mgmTypeGetter = {"getter"};
const char *mgmTypeGroup = {"group"};

// Datatypes
const char *mgmDataTypeInteger = {"i"};
const char *mgmDataTypeBool = {"b"};

static const mgm_desc_t dimmerGroupDesc = {mgmTypeGroup, "Dimmer", 0, "layout:vbox", 0, 0};
static const mgm_desc_t angleDesc = {mgmTypeSetter, "Angle", mgmDataTypeInteger,
                                     "default:99 rmin:0 rmax:100 rep:sbox unit:degrees", &dimmerGroupDesc, set_phase_angle};

static void mgm_addItem(const mgm_desc_t *desc, void *ctx)
{

}


// Events
enum
{
  BLINK_IND = BLF_FIRST_USER_SIG,
};


typedef struct
{
  BLFTimer *timer;
  uint8_t ledStat;
  i2c_t *i2c;
} bsp_t;

// LCD display
lcd_t *ssd2119;

extern "C" void HardFault_Handler(void)
{
  MCAL_ASSERT(0);
}

/*
static void sdio_config(void)
{


  // Data pins
  GPIO_configPin(2, 8, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);
  GPIO_configPin(2, 9, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);
  GPIO_configPin(2, 10, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);
  GPIO_configPin(2, 11, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);

  // CMD line
  GPIO_configPin(3, 2, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);

  // Clock pin
  GPIO_configPin(2, 12, GPIO_MODE_AF | GPIO_OUTPUT_PP, 0xc);

  // Detect pin
  GPIO_configPin(1, 15, GPIO_MODE_INPUT | GPIO_INPUT_PULL_UP, 1);

}
*/



static void runningBlinkInd(bsp_t *self, BLFTimer *evt);
/*
static BLFState running =
{
  [BLF_ENTRY_SIG] = 0,
  [BLINK_IND] = (BLFEventHandler)runningBlinkInd,
};
*/

static void runningBlinkInd(bsp_t *self, BLFTimer *evt)
{
  self->ledStat++;
  //GPIO_write(BSP_GREEN_LED, self->ledStat & 1); // Toggle green led
  //Restart timer
  //BLF_startTimer(evt, 500);

  //RTC_time_t t;
  //RTC_getTime(&t);
  //ssd2119->print(ssd2119, 0, 6, "Time: %02d:%02d:%02d", t.h, t.m, t.s);
  // Check for Lwip timeouts
}


// Init function
static void init(bsp_t *self, void *evt)
{
  // Start background timer
/*
  BLF_startTimer(self->timer, 500);
  BLF_transit(self, running);

  self->i2c = I2C_create(0);
  I2C_open(self->i2c, &i2c_cfg, i2c_rx_callback, self);
  I2C_request_t *req = (I2C_request_t *)BLF_alloc(sizeof(*req) + 1);
  req->address = 0x41;
  req->data[0] = 0;
  req->txLength = 1;
  req->rxLength = 2;
  I2C_request(self->i2c, req);
*/
}






void BSP_init(uint8_t mode)
{

  // Common GPIO config
  const GPIO_config_t gpioCfg[] =
	{
    // PORTA
    {GPIO_MAKEPIN(0,1),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(0,2),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {BSP_HEATER_TRIAC,   .mode = GPIO_MODE_OUTPUT, 	                .param = 0}, 	// Heater triac

    {GPIO_MAKEPIN(0,7),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet

    // PORTB
    {GPIO_MAKEPIN(1,6),  .mode = GPIO_MODE_AF, 			                .param = 0x7}, 	// USART1_TX (debug)
    {GPIO_MAKEPIN(1,7),  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP, .param = 0x7}, 	// USART1_RX (debug)
#ifdef USE_STM32F4BB_LCD
    {GPIO_MAKEPIN(1,8),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_OD, 		.param = 0x4}, 	// I2C1_SCL (STMPE811QTR Touch sensor)
    {GPIO_MAKEPIN(1,9),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_OD, 		.param = 0x4}, 	// I2C1_SDA (STMPE811QTR Touch sensor)
#endif
    {GPIO_MAKEPIN(1,10), .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(1,11), .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(1,12), .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(1,13), .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet

		// PORTC
    {GPIO_MAKEPIN(2,1),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(2,4),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet
    {GPIO_MAKEPIN(2,5),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_PP,     .param = 0xb}, 	// Ethernet

    {GPIO_MAKEPIN(2,6),  .mode = GPIO_MODE_AF, 			                .param = 0x8}, 	// USART6_TX (BB-Dis, MAX2992)
		{GPIO_MAKEPIN(2,7),  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP, .param = 0x8}, 	// USART6_RX (BB-Dis, MAX2992)
#ifdef USE_STM32F4BB_LCD
    {GPIO_MAKEPIN(2,10),  .mode = GPIO_MODE_AF | GPIO_OUTPUT_OD,    .param = 0x7}, 	// USART3_TX (Not used)
    {GPIO_MAKEPIN(2,11),  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP, .param = 0x7}, 	// USART3_RX (Not used)
#endif

		// PORTD
#ifdef USE_STM32F4BB_LCD
    {GPIO_MAKEPIN(3,0),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,1),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,3),  .mode = GPIO_MODE_OUTPUT, 	.param = 0},	  // LCD Reset
    {GPIO_MAKEPIN(3,4),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,5),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,7),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD NE3 configuration
    {GPIO_MAKEPIN(3,8),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,9),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,10), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
#else
    {GPIO_MAKEPIN(3,8),  .mode = GPIO_MODE_AF, 			                .param = 0x7}, 	// USART3_TX (MAX2992)
    {GPIO_MAKEPIN(3,9),  .mode = GPIO_MODE_AF | GPIO_INPUT_PULL_UP, .param = 0x7}, 	// USART3_RX (MAX2992)
#endif
    {BSP_GREEN_LED,      .mode = GPIO_MODE_OUTPUT, 	.param = 0}, 	// Green Led
#ifdef USE_STM32F4BB_LCD
    {GPIO_MAKEPIN(3,13), .mode = GPIO_MODE_OUTPUT, 	.param = 1},	  // LCD PWM
    {GPIO_MAKEPIN(3,14), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(3,15), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
#else
    {GPIO_MAKEPIN(3,13), .mode = GPIO_MODE_OUTPUT, 	.param = 0}, 	// Orange Led
    {GPIO_MAKEPIN(3,14), .mode = GPIO_MODE_OUTPUT, 	.param = 0}, 	// Red Led
    {GPIO_MAKEPIN(3,15), .mode = GPIO_MODE_OUTPUT, 	.param = 0}, 	// Blue LED
#endif

    // PORTE

    {GPIO_MAKEPIN(4,2),  .mode = GPIO_MODE_OUTPUT | GPIO_OUTPUT_PP, 	.param = 0},	  // Ethernet PHY reset
#ifdef USE_STM32F4BB_LCD
    {GPIO_MAKEPIN(4,3),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD SRAM Address line configuration LCD-DC
    {GPIO_MAKEPIN(4,7),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,8),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,9),  .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,10), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,11), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,12), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,13), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,14), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
    {GPIO_MAKEPIN(4,15), .mode = GPIO_MODE_AF, 			.param = 0xc}, 	// LCD data & OE/WE
#endif
  };

  GPIO_configPins(gpioCfg, sizeof(gpioCfg) / sizeof(gpioCfg[0]));


  // Create thread
 // bsp_t *self = (bsp_t *)BLF_createThread(sizeof(*self), 0, (BLFEventHandler)init, 0);

  // Create heartbeat led-blink timer
  //self->timer = BLF_createTimer(self, BLINK_IND, 0);
/*
  // Debug UART
  Gpio::config(1,6, GPIO_MODE_AF, 0x7);
  Gpio::config(1,7, GPIO_MODE_AF | GPIO_INPUT_PULL_UP, 0x7);
  // Onewire UART
  Gpio::config(2,6, GPIO_MODE_AF, 0x8);
  Gpio::config(2,7, GPIO_MODE_AF | GPIO_INPUT_PULL_UP, 0x8);
  // LCD
  Gpio::config(3,0, GPIO_MODE_AF, 0xc);
  Gpio::config(3,1, GPIO_MODE_AF, 0xc);

  Gpio::config(3,3, GPIO_MODE_OUTPUT, 0x0);
  Gpio::config(3,4, GPIO_MODE_AF, 0xc);
  Gpio::config(3,5, GPIO_MODE_AF, 0xc);
  Gpio::config(3,7, GPIO_MODE_AF, 0xc);
  Gpio::config(3,8, GPIO_MODE_AF, 0xc);
  Gpio::config(3,9, GPIO_MODE_AF, 0xc);
  Gpio::config(3,10, GPIO_MODE_AF, 0xc);
  Gpio::config(3,13, GPIO_MODE_OUTPUT, 0x1);
  Gpio::config(3,14, GPIO_MODE_AF, 0xc);
  Gpio::config(3,15, GPIO_MODE_AF, 0xc);
  Gpio::config(4,3, GPIO_MODE_AF, 0xc);
  Gpio::config(4,7, GPIO_MODE_AF, 0xc);
  Gpio::config(4,8, GPIO_MODE_AF, 0xc);
  Gpio::config(4,9, GPIO_MODE_AF, 0xc);
  Gpio::config(4,10, GPIO_MODE_AF, 0xc);
  Gpio::config(4,11, GPIO_MODE_AF, 0xc);
  Gpio::config(4,12, GPIO_MODE_AF, 0xc);
  Gpio::config(4,13, GPIO_MODE_AF, 0xc);
  Gpio::config(4,14, GPIO_MODE_AF, 0xc);
  Gpio::config(4,15, GPIO_MODE_AF, 0xc);
*/
  ssd2119 = ssd2119_create();

  LCD_SetFont(&Font8x12);

  LCD_Clear(LCD_COLOR_WHITE);

  // Set the LCD Back Color
  LCD_SetBackColor(LCD_COLOR_WHITE);
  // Set the LCD Text Color
  LCD_SetTextColor(LCD_COLOR_BLUE);

  ssd2119->print(ssd2119, 0, 0, "MSC R0.0.1");







}

