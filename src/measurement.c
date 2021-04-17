/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "measurement.h"
#include <math.h>

void measurement_queue_init(void)
{
	measurement_queue = xQueueCreate(MEASUREMENT_QUEUE_N, sizeof(measurement_t));
}

const uint8_t* measurement_to_string(measurement_t* measurement)
{
	static uint8_t measurement_string[64];
	uint32_t value_entero = measurement->value/pow(10, measurement->decimal_pos);
	uint32_t value_fraccion = measurement->value % (uint32_t)pow(10, measurement->decimal_pos);

	sprintf(measurement_string, "%04d%02d%02d%02d%02d%02d-%s-%u.%u\r\n",
			measurement->date_time.year,
			measurement->date_time.month,
			measurement->date_time.mday,
			measurement->date_time.hour,
			measurement->date_time.min,
			measurement->date_time.sec,
			measurement->name,
			value_entero,
			value_fraccion);
	return measurement_string;
}

