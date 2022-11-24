/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_MEASUREMENT_H_
#define CESE_PCSE_INC_MEASUREMENT_H_

#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define MEASUREMENT_QUEUE_N 10
#define MEASUREMENT_NAME_LENGTH 5
typedef struct
{
	uint32_t value;
	TickType_t ticks;
	uint8_t decimal_pos;
	uint8_t name[MEASUREMENT_NAME_LENGTH];
	uint8_t fault_code;
}measurement_t;

QueueHandle_t measurement_queue;

void measurement_queue_init(void);
void measurement_set_name(measurement_t* measurement, uint8_t* name_str);
uint8_t* measurement_to_string(measurement_t* measurement);
#endif /* CESE_PCSE_INC_MEASUREMENT_H_ */
