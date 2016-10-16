#include "usart.h"

QUEUE Q_UART1	= {0, 0, 0};
QUEUE Q_UART3	= {0, 0, 0};

void USART1_Start(uint32_t bandrate, uint16_t parity, uint16_t worldlen, uint16_t stopbit)
{
	GPIO_InitTypeDef GPIO_InitS;
	NVIC_InitTypeDef NVIC_InitS;
	USART_InitTypeDef USART_InitS;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_10;//RX
	GPIO_InitS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitS);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_9;//TX
	GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitS.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitS);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitS.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitS);

	USART_InitS.USART_BaudRate = bandrate;
	USART_InitS.USART_WordLength = worldlen;
	USART_InitS.USART_Parity = parity;
	USART_InitS.USART_StopBits = stopbit;
	USART_InitS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitS.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitS);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void USART3_Start(uint32_t bandrate, uint16_t parity, uint16_t worldlen, uint16_t stopbit)
{
	GPIO_InitTypeDef GPIO_InitS;
	NVIC_InitTypeDef NVIC_InitS;
	USART_InitTypeDef USART_InitS;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_11;//RX
	GPIO_InitS.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitS);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_10;//TX
	GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitS.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitS);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_0;//RS485 Control line
	GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitS.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitS);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitS.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitS.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitS.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitS);

	USART_InitS.USART_BaudRate = bandrate;
	USART_InitS.USART_WordLength = worldlen;
	USART_InitS.USART_Parity = parity;
	USART_InitS.USART_StopBits = stopbit;
	USART_InitS.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitS.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitS);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART3, ENABLE);
}

void USART1_IRQHandler(void)//USART1 interrupt program
{
	uint16_t tmp;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		// Read one byte from the receive data register
		tmp = USART_ReceiveData(USART1);
		QueueAddByte(&Q_UART1, tmp);
	}
}

void USART3_IRQHandler(void)//USART1 interrupt program
{
	uint16_t tmp;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		// Read one byte from the receive data register
		tmp = USART_ReceiveData(USART3);
		QueueAddByte(&Q_UART3, tmp);
	}
}

void RS232_SendData(uint8_t data)
{
	if (USART_GetITStatus(USART1, USART_IT_TXE) == RESET)
	{
		USART_SendData(USART1, data);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) ;
	}
}

void RS485_SendData(uint8_t data)
{
	if (USART_GetITStatus(USART3, USART_IT_TXE) == RESET)
	{
		GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
		USART_SendData(USART3, data);
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) ;
		GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_RESET);
	}
}

