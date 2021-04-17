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
//	int32_t value_entero = measurement->value/(int32_t)pow(10, measurement->decimal_pos);
//	int32_t value_fraccion = measurement->value % (int32_t)pow(10, measurement->decimal_pos);

	sprintf(measurement_string, "\x02%04d%02d%02d%02d%02d%02d\x1D%s\x1D%d\x1D%d\x1D%02x\x03\r\n",
			measurement->date_time.year,
			measurement->date_time.month,
			measurement->date_time.mday,
			measurement->date_time.hour,
			measurement->date_time.min,
			measurement->date_time.sec,
			measurement->name,
			measurement->value,
			measurement->decimal_pos,
			measurement->fault_code);
	return measurement_string;
}

