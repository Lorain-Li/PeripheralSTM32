#ifndef __CAN_H__
#define __CAN_H__

#include "config.h"
#include "types.h"
#include "queue.h"

extern void CAN_Start(void);
extern void CAN_SendData(uint8_t data);
extern void USB_LP_CAN1_RX0_IRQHandler(void);
#endif

