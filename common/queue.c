#include "queue.h"

bool QueueAddByte(QUEUE *q, uint8_t data)
{
	if (q->q_length < MAX_QUEUE)
	{
		q->data[q->q_foot++] = data;
		q->q_length++;
		q->q_foot = q->q_foot % MAX_QUEUE;
		return TRUE;
	}
	else return FALSE;
}

bool QueueGetByte(QUEUE *q, uint8_t *p_data)
{
	if (q->q_length > 0)
	{
		*p_data = q->data[q->q_head++];
		q->q_length--;
		q->q_head = q->q_head % MAX_QUEUE;
		return TRUE;
	}
	else return FALSE;
}

bool QueueAddBytes(QUEUE *q, uint8_t *p_data, uint16_t len)
{
	uint16_t i = 0; uint8_t tmp;
	if (q->q_length < MAX_QUEUE - len) return FALSE;
	else
	{
		while (i < len)
		{
			if (!QueueAddByte(q, p_data[i]))
			{
				i--; QueueGetByte(q, &tmp);
			}
			i++;
		}
		return TRUE;
	}
}

uint16_t QueueAddForce(QUEUE *q, uint8_t *p_data)
{
	uint16_t i = 0;
	while (QueueAddByte(q, p_data[i]))
	{
		i++;
	}
	return i;
}

