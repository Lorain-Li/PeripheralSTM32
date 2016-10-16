#ifndef __QUEUE__
#define __QUEUE__

#include "types.h"

#define MAX_QUEUE 512

typedef struct QUEUETYPE
{
	uint16_t q_head, q_foot, q_length;
	uint8_t data[MAX_QUEUE];
} QUEUE;


extern bool QueueAddByte(QUEUE *q, uint8_t data);
extern bool QueueGetByte(QUEUE *q, uint8_t *p_data);
extern bool QueueAddBytes(QUEUE *q, uint8_t *p_data, uint16_t len);
extern uint16_t QueueAddForce(QUEUE *q, uint8_t *p_data);
#endif
