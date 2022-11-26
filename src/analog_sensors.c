/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "analog_sensors.h"
#include "sapi.h"
#include <string.h>

void analog_sensors_init(void)
{
	adcConfig(ADC_ENABLE);
}

void analog_sensor(analog_sensor_t* analog_sens, uint32_t scale, uint8_t* name,
		adcMap_t adc_pin)
{
	analog_sens->value = 0;
	analog_sens->scale = scale;
	strncpy(analog_sens->name, name, 5);
	analog_sens->adc_pin = adc_pin;
}

uint32_t analog_sensor_read(analog_sensor_t* analog_sens, adcMap_t adc_pin)
{
	uint16_t adc_readout = adcRead(adc_pin);
	uint32_t analog_value = (uint32_t)adc_readout * (uint32_t)analog_sens->scale);
	return analog_value;
}
