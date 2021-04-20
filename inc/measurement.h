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
#include <string.h>

#define MEASUREMENT_QUEUE_N 10
typedef struct
{
	uint32_t value;
	rtc_t date_time;
	uint8_t decimal_pos;
	uint8_t name[10];
	uint8_t fault_code;
}measurement_t;

QueueHandle_t measurement_queue;

void measurement_queue_init(void);
uint8_t* measurement_to_string(measurement_t* measurement);
#endif /* CESE_PCSE_INC_MEASUREMENT_H_ */
