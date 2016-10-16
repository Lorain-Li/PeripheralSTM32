#ifndef __TIM_H__
#define __TIM_H__

#include "config.h"
#include "queue.h"
#include "types.h"

extern void TIM2_Start(void);
extern void Delay_ms(uint16_t ms);

#if (__ENABLE_TIM2_INT__)
extern void TIM2_IRQHandler(void);
#endif

#endif


