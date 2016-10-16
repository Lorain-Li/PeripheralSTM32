#include "can.h"

QUEUE Q_CAN		= {0, 0, 0};
uint8_t mailbox;
CanTxMsg TxMessage = {0x12345678, 0x00000000, CAN_ID_STD, CAN_RTR_DATA, 0x00};
CanRxMsg RxMessage;

extern QUEUE buffer1, buffer2, buffer3;

void CAN_Start(void)
{
	GPIO_InitTypeDef GPIO_InitS;
	NVIC_InitTypeDef NVIC_InitS;
	CAN_InitTypeDef CAN_InitS;
	CAN_FilterInitTypeDef  CAN_FilterInitS;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

	NVIC_InitS.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitS.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitS);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_8;  //CAN_RX
	GPIO_InitS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitS);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_9;  //CAN_TX
	GPIO_InitS.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitS);

	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitS);
	CAN_InitS.CAN_TTCM = DISABLE;
	CAN_InitS.CAN_ABOM = DISABLE;
	CAN_InitS.CAN_AWUM = DISABLE;
	CAN_InitS.CAN_NART = DISABLE;
	CAN_InitS.CAN_RFLM = DISABLE;
	CAN_InitS.CAN_TXFP = DISABLE;
#if (__CAN_MODE_LOOPBACK__)
	CAN_InitS.CAN_Mode = CAN_Mode_LoopBack;  //loopback mode , 自收自发
#else
	CAN_InitS.CAN_Mode = CAN_Mode_Normal;  //正常发送与接收
#endif
	CAN_InitS.CAN_SJW = CAN_SJW_1tq;
	CAN_InitS.CAN_BS1 = CAN_BS1_4tq;
	CAN_InitS.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitS.CAN_Prescaler = 45;
	if (CAN_Init(CAN1, &CAN_InitS) == CANINITFAILED)
	{

	}
	CAN_FilterInitS.CAN_FilterNumber = 0;
	CAN_FilterInitS.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitS.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitS.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitS.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitS.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitS.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitS.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitS.CAN_FilterActivation = ENABLE;
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
	CAN_FilterInit(&CAN_FilterInitS);

	mailbox = 0xff;
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	uint16_t i, tmp;
	CanRxMsg RxMSG;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMSG);
	for (i = 0; i < RxMSG.DLC; i++)
	{
		tmp = (uint16_t)(RxMSG.Data[i] & 0x00ff);
		QueueAddByte(&Q_CAN, tmp);
	}
	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
}


void CAN_SendData(uint8_t data)
{
#ifndef __SIMPLE__
	if (TxMessage.DLC < 8)
	{
		TxMessage.Data[TxMessage.DLC++] = data;
	}
	if (TxMessage.DLC == 8)
	{
		if (mailbox == 0xff)
		{
			mailbox = CAN_Transmit(CAN1, &TxMessage);
		}
		else
		{
			while (CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok) ;
			mailbox = CAN_Transmit(CAN1, &TxMessage);
		}
		TxMessage.DLC = 0;
	}
#else
	TxMessage.DLC = 1;
	TxMessage.Data[0] = data;
	if (mailbox == 0xff)
	{
		mailbox = CAN_Transmit(CAN1, &TxMessage);
	}
	else
	{
		while (CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok) ;
		mailbox = CAN_Transmit(CAN1, &TxMessage);
	}
#endif
}


