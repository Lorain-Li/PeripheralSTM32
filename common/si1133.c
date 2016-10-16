#include "si1133.h"

int16_t SI1133_ReadRegister(uint8_t addr)
{
	uint8_t x;
	I2C_ReadByte(SI1133_PART_ID, addr, &x);
	return x;
}

int16_t SI1133_WriteRegister(uint8_t addr, uint8_t data)
{
	I2C_WriteByte(SI1133_PART_ID, addr, &data);
	return 0;
}

static int16_t SI1133_WriteBlock(uint8_t addr, uint8_t* p_data, uint16_t len)
{
	I2C_WriteBlock(SI1133_PART_ID, addr, p_data, len);
	return 0;
}

static int16_t SI1133_ReadBlock(uint8_t addr, uint8_t* p_data, uint16_t len)
{
	I2C_ReadBlock(SI1133_PART_ID, addr, p_data, len);
	return 0;
}

static int16_t WaitUntilSleep(void)
{
	int16_t retval = -1;
	uint8_t count = 0;
	while (count < 5)
	{
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_CHIPSTAT_MASK) == RSP0_SLEEP)   break;
		if (retval <  0)   return retval;
		count++;
	}
	return 0;
}

static int16_t SI1133_SendCMD(uint8_t command)
{
	int16_t  response, retval;
	uint8_t  count = 0;
	response = SI1133_ReadRegister(REG_RESPONSE0);
	if (response < 0) return response;
	response = response & RSP0_COUNTER_MASK;
	for (count = 0 ; count < 5; count++)
	{
		if ((retval = WaitUntilSleep()) != 0) return retval;
		if (command == 0) break;
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) == response) break;
		else if (retval < 0) return retval;
		else
			response = retval & RSP0_COUNTER_MASK;
	}
	if ((retval = (SI1133_WriteRegister(REG_COMMAND, command)) != 0))
	{
		return retval;
	}
	for (count = 0 ; count < 5; count++)
	{
		if (command == 0)  break;
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) != response) break;
		else if (retval < 0) return retval;
	}
	return 0;
}

int16_t SI1133_ReadParameter(uint8_t addr)
{
	int16_t retval;
	retval = SI1133_SendCMD(CMD_READ | (addr & 0x3F));
	if ( retval != 0 )    return retval;
	retval = SI1133_ReadRegister(REG_RESPONSE1);
	return retval;
}

int16_t SI1133_WriteParameter(uint8_t addr, uint8_t data)
{
	uint8_t buffer[2];
	int16_t retval, response_stored, response;
	buffer[0] = data; buffer[1] = CMD_WRITE | (addr & 0x3F);
	if ((retval = WaitUntilSleep()) != 0)    return retval;
	response_stored = RSP0_COUNTER_MASK & SI1133_ReadRegister(REG_RESPONSE0);
	retval = SI1133_WriteBlock(REG_HOSTIN0, (uint8_t *)buffer, 2);
	if (retval != 0)  return retval;
	response = SI1133_ReadRegister(REG_RESPONSE0);
	while ((response & RSP0_COUNTER_MASK) == response_stored)
	{
		response = SI1133_ReadRegister(REG_RESPONSE0);
	}
	if (retval < 0)   return retval;
	else   return 0;
}

static int16_t SI1133_Reset(void)
{
	int16_t retval = 0;
	Delay_ms(10);
	Delay_ms(10);
	Delay_ms(10);
	retval += SI1133_WriteRegister(REG_COMMAND, CMD_RESET_SW);
	Delay_ms(10);
	return retval;
}

int16_t SI1133_NOP(void)
{
	return SI1133_SendCMD(CMD_RESET_CTR);
}

int16_t SI1133_Force(void)
{
	return SI1133_SendCMD(CMD_FORCE);
}

int16_t SI1133_Start (void)
{
	return SI1133_SendCMD(CMD_START);
}

int16_t SI1133_Pause(void)
{
	uint8_t countA, countB;
	int8_t  retval = 0;
	while ((RSP0_CHIPSTAT_MASK & retval) != RSP0_SLEEP)
	{
		retval = SI1133_ReadRegister(REG_RESPONSE0);
	}
	countA = 0;
	while (countA < 5)
	{
		countB = 0;
		while (countB < 5)
		{
			retval = SI1133_ReadRegister(REG_RESPONSE0);
			if ((retval & RSP0_COUNTER_MASK) == 0)     break;
			else
			{
				SI1133_WriteRegister(REG_COMMAND, 0x00);
			}
			countB++;
		}
		SI1133_SendCMD(CMD_PAUSE);
		countB = 0;
		while (countB < 5)
		{
			retval = SI1133_ReadRegister(REG_RESPONSE0);
			if ((retval & RSP0_COUNTER_MASK) != 0)        break;
			countB++;
		}
		retval = SI1133_ReadRegister(REG_RESPONSE0);
		if ((retval & RSP0_COUNTER_MASK) == 1 )      break;
		countA++;
	}
	return 0;
}
int16_t SI1133_Init(void)
{
	int16_t    retval;
	retval  = SI1133_Reset();
	Delay_ms(10);
	retval += SI1133_WriteParameter( PARAM_CHAN_LIST, 	0x3f);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG0, 	0x78);
	retval += SI1133_WriteParameter( PARAM_ADCSENS0, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG0, 0x40);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG1, 	0x6d);
	retval += SI1133_WriteParameter( PARAM_ADCSENS1, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG1, 0x40);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG2, 	0x6b);
	retval += SI1133_WriteParameter( PARAM_ADCSENS2, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG2, 0x40);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG3, 	0x61);
	retval += SI1133_WriteParameter( PARAM_ADCSENS3, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG3, 0x40);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG4, 	0x60);
	retval += SI1133_WriteParameter( PARAM_ADCSENS4, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG4, 0x40);
	retval += SI1133_WriteParameter( PARAM_ADCCONFIG5, 	0x79);
	retval += SI1133_WriteParameter( PARAM_ADCSENS5, 	0x89);
	retval += SI1133_WriteParameter( PARAM_MEASCONFIG5, 0x40);
	retval += SI1133_WriteParameter( PARAM_MEASRATE_H, 	0x09);
	retval += SI1133_WriteParameter( PARAM_MEASRATE_L, 	0xc4);
	retval += SI1133_WriteRegister(  REG_IRQ_ENABLE, 	0x3f);
	retval += SI1133_Start();
	return retval;
}

