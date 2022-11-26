/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "measurement.h"
#include "sapi.h"
#include <math.h>
#include <string.h>

void measurement_queue_init(void)
{
	measurement_queue = xQueueCreate(MEASUREMENT_QUEUE_N, sizeof(measurement_t));
}

void measurement_set_name(measurement_t* measurement, uint8_t name_str[5])
{
	strncpy(measurement->name, name_str, MEASUREMENT_NAME_LENGTH);
}

uint8_t* measurement_to_string(measurement_t* measurement)
{
	static uint8_t measurement_string[64];

	sprintf(measurement_string, "\x1D%d\x1D%s\x1D%d\x1D%d\x1D%02x\x03\r\n",
			measurement->ticks,
			measurement->name,
			measurement->value,
			measurement->decimal_pos,
			measurement->fault_code);
	return measurement_string;
}

