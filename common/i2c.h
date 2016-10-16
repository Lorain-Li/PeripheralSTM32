#ifndef __I2C__
#define __I2C__

#include "config.h"
#include "types.h"

#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xA0

extern void I2C_Start(void);
extern void I2C_WriteByte(uint8_t id, uint8_t w_addr, uint8_t* p_data);
extern void I2C_ReadByte(uint8_t id , uint8_t r_addr, uint8_t* p_data);
extern void I2C_ReadBlock(uint8_t id, uint8_t r_addr, uint8_t* p_data, uint16_t len);
extern void I2C_WriteBlock(uint8_t id, uint8_t w_addr, uint8_t* p_data, uint16_t len);
#endif
