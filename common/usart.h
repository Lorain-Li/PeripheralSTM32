#ifndef __USART_H__
#define __USART_H__

#include "config.h"
#include "types.h"
#include "queue.h"

extern void USART1_Start(uint32_t bandrate, uint16_t parity, uint16_t worldlen, uint16_t stopbit);
extern void USART3_Start(uint32_t bandrate, uint16_t parity, uint16_t worldlen, uint16_t stopbit);
extern void USART1_IRQHandler(void);
extern void USART3_IRQHandler(void);
extern void RS232_SendData(uint8_t data);
extern void RS485_SendData(uint8_t data);

#endif

