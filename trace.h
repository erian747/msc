/**
 * @file trace.h
 */

/** @addtogroup Application
  * @{
  */

/** @defgroup TRACE TRACE
  * @brief Trace management functions
  * @{
  */

#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif


// Trace levels
#define TTRACE_ALL  1
#define TTRACE_INFO  2
#define TTRACE_WARN  3
#define TTRACE_ERROR  4

  // Include config for trace messages
#include "trace_config.h"

// Helper macros
#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y


#if defined(TTRACE_ON) && JOIN(TRACE_ENABLE_, MODULE_NAME) > 0
  #define TTRACE(traceLevel,...)   do { \
                               if (traceLevel >= JOIN(TRACE_ENABLE_, MODULE_NAME)) \
                                 TTRACE_print(traceLevel, __VA_ARGS__); \
                             } while(0)

#elif defined(PCTEST)
  #include <stdio.h>
  #define TTRACE(traceLevel,...) printf(__VA_ARGS__)
#else
  #define TTRACE(traceLevel,...)
#endif



/**
  * @brief Store a trace message with trace level
  * @param[in] level Trace level
  * @param[in] fmt Trace text
  * @param[in] ... Arguments
  */
void TTRACE_print(uint8_t level, const char *fmt, ...);


/**
  * @brief Initializes trace module
  * @retval None
  */
void TTRACE_init(void);

/**
  * @brief Set the trace level to use
  * @param newLevel Range 0 to 255 where 15-255 activates all traces and 0 none
  * @retval None
  */
void TTRACE_setLevel(uint8_t newLevel);

/**
  * @brief Checks whatever there is trace message available to read
  * @retval True if there is messages in trace queue, False if trace queue is empty
  */
uint8_t TTRACE_check(void);


void TTRACE_process(void);
#if TTRACE_USE_UART
typedef struct usart_if_t usart_if_t;

void TTRACE_setOutputUart(usart_if_t *uart);
#endif



#ifdef __cplusplus
}
#endif


#endif

/**
  * @}
  */

/**
  * @}
  */

