/**
 * Brief description:
 * Text trace functionality.
 * @file trace.c
 */


/** @addtogroup TRACE
  * @{
  */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "blf.h"
#include "trace.h"
#include "mcal.h"

#include "mcal_usart.h"

#define TTRACE_MAX_NR_ARGUMENTS 8
#define TTRACE_MAX_STRING_LENGTH 128
// Max length of text generated when reading traces
#define TRACE_MAX_LINE_LENGHT	100
// Maximum number of tracemessages to queue
#define TRACE_QUEUE_LENGTH    32
// Indicates that a trace buffer element was the last written before the queue was full
#define TRACE_FULL_FLAG 0x10

static BLFQueue *traceQueue;
static volatile uint8_t messages;
static uint8_t traceLevel;
static usart_if_t *uart;
static char *sendBuffer;


void TTRACE_init(void)
{
  traceLevel = 0xFF; // Enable all traces as default
  traceQueue = BLF_createQueue();
  messages = 0;
  sendBuffer = 0;
}

/**
  * @brief Trace message struct
  *
  */
typedef struct
{
  const char *text;   /**< String containing output text*/
  uint8_t nrArgs;        /**< Number of arguments */
  uint8_t flags;         /**< Flags*/
  uint16_t timeStamp;   /**< Time stamp indicating when this trace happened */
  uint32_t args[TTRACE_MAX_NR_ARGUMENTS];     /**< Arguments */
} tracemsg_t;


/**
  * @brief Get the lowest 16bits of time
  * @retval The lowest 16 bits of current time
  */

static __inline uint16_t getTimeStamp(void)
{
  //uint32_t utchHigh, utcLow;
 // RTC_getTimeUTC(&utchHigh, &utcLow);
  //return utcLow & 0xFFFF;
  return 0;
}



void TTRACE_print(uint8_t level, const char *fmt, ...)
{
  va_list va;                //lint !e40, !e522 va_list not found by lint
  va_start(va,fmt);
  // Protect access to messages
  MCAL_LOCK_IRQ();
  if((messages >= TRACE_QUEUE_LENGTH) || (level > traceLevel))
  {
    MCAL_UNLOCK_IRQ();
    va_end(va);
    return;
  }
  
  messages++;
  MCAL_UNLOCK_IRQ();
  
  
  
  // Allocate trace buffer
  tracemsg_t *tm = (tracemsg_t *)BLF_alloc(sizeof(tracemsg_t));
  
  uint32_t args = 0;
  // Find and store arguments
  for(const char *cp = fmt; *cp != 0; cp++)
  {
    if(*cp == '%')
    {  
      if(args >= TTRACE_MAX_NR_ARGUMENTS)
        break;
      // Get argument from stack
      uint32_t arg_val = va_arg(va, uint32_t);
    
      // Special case for string
      if(cp[1] == 's')
      {
#if defined(__arm__) // Target optimization, if string is located in flash, only store pointer to it
        if(! MCAL_IS_IN_FLASH(arg_val))
#endif 
        {
          const char *sstr = (const char *)arg_val; 
          char *strbuf = BLF_alloc(TTRACE_MAX_STRING_LENGTH);
          for(uint32_t n = 0; *sstr != 0; n++)
          {  
            strbuf[n] = *sstr++;
            if(n >= (TTRACE_MAX_STRING_LENGTH-1))
            {
              strbuf[n] = 0;
              break;
            }
          }
          arg_val = (uint32_t)strbuf;
        }
         
      }
      // Store argument
      tm->args[args] = arg_val; 
      args++;      
      
    
    }
  }

  va_end(va);

  tm->text = fmt;
  tm->nrArgs = args;
  tm->timeStamp = getTimeStamp();


  // If trace queue becomes full, set flag
  tm->flags = (messages >= TRACE_QUEUE_LENGTH) ? TRACE_FULL_FLAG : 0;
  BLF_appendQueue(traceQueue, tm);
  
}





/**
  * @brief Set current trace level
  * @param[in] newLevel New Trace level
  */
void TTRACE_setLevel(uint8_t newLevel)
{
  traceLevel = newLevel;
}


static char *uint_to_dec(uint32_t num, char *bf)
{
  // End terminate
  *bf-- = 0;
  uint32_t a = num;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    *bf-- =  (a % 10) + '0';
    a /= 10;
  }
  return bf+1;
}

static char *int_to_dec(int32_t num, char *bf)
{
  uint32_t a = num > 0 ? num : -num;
  // End terminate
  *bf-- = 0;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    *bf-- =  (a % 10) + '0';
    a /= 10;
  }

  if(num < 0)
    *bf-- = '-';

  return bf+1;
}

static char *uint_to_hex(uint32_t num, char cs, char *bf)
{
  // End terminate
  *bf-- = 0;

  uint32_t a = num;

  if(a == 0)
    *bf-- = '0';

  while(a)
  {
    uint8_t nib = (a & 0xf);
    *bf-- = nib + ((nib > 9) ? (cs-10) : '0');
    a >>= 4;
  }
  return bf+1;
}

// Output string type
typedef struct
{
  char *data;
  uint8_t length;
  uint8_t maxLen;
} outstr_t;


// Write char to length limited string
static void writeToStr(outstr_t *os, char ch)
{
  if((os->length+1) < os->maxLen)
  {
    os->data[os->length] = ch;
  }
  os->length++;
}

// Put
static void putchw(outstr_t *os, uint8_t n, char pc, char* bf)
{
  char* p=bf;
  while (*p++ && n > 0)
    n--;
  // Leading pad
  while (n-- > 0)
  writeToStr(os,pc);

  while(*bf != 0)
  {
    writeToStr(os,*bf);
    bf++;
  }
}


enum
{
  S_TEXT,
  S_FORMAT_START,
};

static int dbg_snprintf(char *outStr, uint32_t maxLength, const char *fmt, uint32_t *args)
{
  const char *parseStr = fmt;
  outstr_t os;
  os.maxLen = maxLength;
  os.data = outStr;
  os.length = 0;
  char bf[12];

  uint8_t state = S_TEXT;

  // Default pad with space
  char padChar = ' ';
  uint8_t width = 0;

  while(*parseStr != 0)
  {
    char ch = *parseStr++;

    if(state == S_TEXT)
    {
      if(ch == '%')
      state = S_FORMAT_START;
      else
      writeToStr(&os, ch);
    }
    else
    {
      switch(ch)
      {
        case 'c' :
        {
          writeToStr(&os, *args++);    //lint !e40
          state = S_TEXT;
        } break;

        case 'u' :
        {
          char *s_ptr = uint_to_dec(*args++,&bf[11]);  //lint !e40, !e78, !e530
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        case 'i' :
        case 'd' :
        {
          char *s_ptr = int_to_dec(*args++, &bf[11]);   //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        // Hex start found
        case 'x' :
        {
          char *s_ptr = uint_to_hex(*args++,'a',&bf[11]);  //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;

        // HEX in captials start found
        case 'X' :
        {
          char *s_ptr = uint_to_hex(*args++,'A',&bf[11]);  //lint !e40, !e78
          putchw(&os, width, padChar, s_ptr);
          state = S_TEXT;
        } break;
        // String
        case 's' :
        {
          char *str = (char *)*args++;
          char *p = str;
          while(*str != 0)
            writeToStr(&os,*str++);  
#if defined(__arm__) // Target optimization, buffer only needs to be released if not located in flash
        if(! MCAL_IS_IN_FLASH(p))
          BLF_free(p);
#endif 
                  
        } break;
        case '0' :
        {
          // Pad with zeroes
          padChar = ch;
        } break;

        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case '8' :
        case '9' :
        {
          width = ch - '0';
        } break;

        // Ignore precision
        case '.' :
        {
        } break;

        // Unknown parameter flag
        default :
        {
          state = S_TEXT;
          writeToStr(&os, ch);
        }
      }
    }
  }
  // Terminate
  if((os.length + 1) < os.maxLen)
  {
    os.data[os.length] = 0;
  }
  else
  {
    os.data[os.maxLen-1] = 0;
  }

  return (int)os.length;
}


/**
 * @brief Process messages in trace queue and output to UART
 */
void TTRACE_process(void)
{
  if(sendBuffer != 0)
    return;
  
  uint32_t length = 0;
  tracemsg_t *tm = 0;
  MCAL_LOCK_IRQ();
  if(messages > 0)
  {
    messages--;
    tm = (tracemsg_t *)BLF_takeFirstInQueue(traceQueue);
  }
  else
  {
    MCAL_UNLOCK_IRQ();
    return;
  }
  MCAL_UNLOCK_IRQ();
  
  if(tm == 0)
    return;
  
  // Calculate maximum needed buffer length,
  // 7 comes from  "0000: " + zero terminator
  // 11 comes from "0000: " + "[F]" + zero terminator
  uint32_t allocLen = TRACE_MAX_LINE_LENGHT + ((tm->flags & TRACE_FULL_FLAG) ? 11 : 7);
  // Allocate buffer
  char *cp = (char *)BLF_alloc(allocLen);

  // only 4 characters allocated for rtc, value not truncated
  uint32_t timeStamp = tm->timeStamp & 0x1fff;
  dbg_snprintf((char *)cp, 7, "%04d: ", &timeStamp);

  length = dbg_snprintf(&cp[6], TRACE_MAX_LINE_LENGHT, tm->text, tm->args);
  
  // Clamp string length to buffer size to indicate the actual written length
  if(length >= TRACE_MAX_LINE_LENGHT)
  {
    length = TRACE_MAX_LINE_LENGHT-1;
  }
  
  // Put buffer queue full indication after text message (start inserting starting from zero termination)
  if(tm->flags & TRACE_FULL_FLAG)
  {
    (void)dbg_snprintf(&cp[length + 6], 4, "[F]", 0);
    length += 3;
  }

  BLF_free(tm);
  sendBuffer = cp;
  USART_send(uart, cp, length+6);

}


/**
  * @brief  USART Tx transfer done callback
  * @param cbid Not used
  */
static void txCb(void *cbid)
{
  BLF_free(sendBuffer);
  sendBuffer = 0;
}

extern void ttrace_key_cb(char data) __attribute__((weak));

/**
  * @brief  USART received TU callback
  * @param cbid Not used
  */
static void rxCb(void *cbid, uint32_t data)
{
  // Call keyboard callback if used
  if(ttrace_key_cb != 0)
    ttrace_key_cb((char)data);
}



void TTRACE_setOutputUart(usart_if_t *output_uart)
{
  uart = output_uart; 
  USART_open(uart, 115200, rxCb, txCb, 0);  
}



/**
  * @}
  */


