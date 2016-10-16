#include "i2c.h"

void I2C_Start(void)
{
	GPIO_InitTypeDef GPIO_InitS;
	I2C_InitTypeDef I2C_InitS;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitS.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitS.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitS.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitS);

	I2C_InitS.I2C_Mode = I2C_Mode_I2C;
	I2C_InitS.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitS.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
	I2C_InitS.I2C_Ack = I2C_Ack_Enable;
	I2C_InitS.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitS.I2C_ClockSpeed = I2C_Speed;

	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitS);
}

void I2C_WriteByte(uint8_t id, uint8_t w_addr, uint8_t* p_data)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, w_addr);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, *p_data);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C1, ENABLE);
}
void I2C_WriteBlock(uint8_t id, uint8_t w_addr, uint8_t* p_data, uint16_t len)
{
	uint16_t i = 0;
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C1, w_addr);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	while (i < len) {
		I2C_SendData(I2C1, *p_data);
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
		i++; p_data++;
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
}
void I2C_ReadByte(uint8_t id , uint8_t r_addr, uint8_t* p_data)
{
	I2C_ReadBlock(id,r_addr,p_data,1);
}

void I2C_ReadBlock(uint8_t id, uint8_t r_addr, uint8_t* p_data, uint16_t len)
{
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_Cmd(I2C1, ENABLE);
	I2C_SendData(I2C1, r_addr);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C1, ENABLE);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C1, id, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	while (len)
	{
		if (len == 1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}
		if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			*p_data = I2C_ReceiveData(I2C1);
			p_data++;
			len--;
		}
	}
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}
