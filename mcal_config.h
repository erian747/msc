
#define MCAL_USE_HSE_CRYSTAL 1
#define MCAL_USE_LSE_CRYSTAL 0
#define MCAL_USE_FPU 1

#ifndef MCU_SYSFREQ
  #define MCU_SYSFREQ 72000000
#endif

#define MCAL_TIMER_USAGE_MASK 0xff
#define MCAL_USART_USAGE_MASK 0x3f
#define MCAL_SPI_USAGE_MASK   0
#define MCAL_ADC_USAGE_MASK   1
#define MCAL_EXTI_USAGE			  1

#define MCAL_USE_ASSERT 			1

#define MCAL_USE_TRACE				1

//------------------------------------------------------------------------
// ADC options

#define MCAL_ADC_REF_VOLTAGE 3300

//------------------------------------------------------------------------
// SPI options

#define MCAL_SPI_MASTER_TIMER 2
//------------------------------------------------------------------------
// Traceing

#include "trace.h"

#define MCAL_TRACE TTRACE

#define MCAL_TRACE_INFO   TTRACE_INFO
#define MCAL_TRACE_WARN   TTRACE_WARN
#define MCAL_TRACE_ERROR  TTRACE_ERROR
