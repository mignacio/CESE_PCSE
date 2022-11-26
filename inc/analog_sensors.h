/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_ANALOG_SENSORS_H_
#define CESE_PCSE_INC_ANALOG_SENSORS_H_

#include "sapi.h"

#define VBATT_DEC_POS 4
#define O2_DEC_POS 1
#define PRES_ACEITE_DEC_POS 1

typedef struct
{
	uint32_t value;
	uint32_t scale;
	uint8_t name[5];
	adcMap_t adc_pin;
}analog_sensor_t;

void analog_sensors_init(void);
void analog_sensor(analog_sensor_t* analog_sens, uint32_t scale, uint8_t* name,
		adcMap_t adc_pin);
uint32_t analog_sensor_read(analog_sensor_t* analog_sens, adcMap_t adc_pin);


#endif /* CESE_PCSE_INC_ANALOG_SENSORS_H_ */
