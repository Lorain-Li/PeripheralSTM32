#include "tim.h"

void TIM2_Start(void)
{
	NVIC_InitTypeDef NVIC_InitS;
	TIM_TimeBaseInitTypeDef TIM_TBIS;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#if (__ENABLE_TIM2_INT__)
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitS.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitS);
#endif
	TIM_DeInit(TIM2);
//计数器周期：（TIM_Prescaler+1）/主频 = 0.01ms,中断周期：0.1*TIM_Period = 0.1s
	TIM_TBIS.TIM_Period = 10000;
	TIM_TBIS.TIM_Prescaler = 720 - 1;		
	TIM_TBIS.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TBIS);
#if (__ENABLE_TIM2_INT__)
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
#endif
	TIM_Cmd(TIM2, ENABLE);
}
#if (__ENABLE_TIM2_INT__)
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_FLAG_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
		tim2flag = FALSE;
		TIM_Cmd(TIM2, DISABLE);
	}
}
#endif

void Delay_ms(uint16_t ms)
{
	while(ms)
	{
		if(TIM2->CNT > 999)
		{
			ms--;
			TIM2->CNT = 0;
		}
	}
}