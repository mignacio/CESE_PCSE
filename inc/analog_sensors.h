/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_ANALOG_SENSORS_H_
#define CESE_PCSE_INC_ANALOG_SENSORS_H_

#include "sapi.h"

#define VBATT_DEC_POS 3
#define O2_DEC_POS 1
#define PRES_ACEITE_DEC_POS 1

typedef struct
{
	uint32_t value;
	uint32_t scale;
	uint8_t name[5];
	adcMap_t adc_pin;
	const uint32_t (*look_up_table_ptr)[1024];
}analog_sensor_t;

void analog_sensors_init(void);
void analog_sensor(analog_sensor_t* analog_sens, uint32_t scale, adcMap_t adc_pin,
		uint8_t* name);
uint32_t analog_sensor_read(analog_sensor_t* analog_sens);


#endif /* CESE_PCSE_INC_ANALOG_SENSORS_H_ */
