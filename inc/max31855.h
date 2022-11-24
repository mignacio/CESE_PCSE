/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_MAX31855_C_
#define CESE_PCSE_INC_MAX31855_C_

#include "sapi.h"

#define MAX31855_BUFFER_SIZE 4
#define MAX31855_INTERNAL_DEC_POS 4
#define MAX31855_EXTERNAL_DEC_POS 2

#define MAX31855_SCV_FAULT	4
#define MAX31855_SCG_FAULT	3
#define MAX31855_OC_FAULT	1

typedef struct
{
	int32_t internal_temp;
	int32_t external_temp;
	uint8_t buffer[MAX31855_BUFFER_SIZE];
	uint8_t name[5];//Nombre del device 4 chars de largo
	gpioMap_t cs_pin;
	uint8_t fault;
}max31855_t;

void max31855_init(void);
void max31855(max31855_t* device, gpioMap_t chip_select, uint8_t* name_str);
void max31855_read(max31855_t* device);
int32_t max31855_ext_temp_to_celsius(max31855_t* device);
int32_t max31855_int_temp_to_celsius(max31855_t* device);

#endif /* CESE_PCSE_INC_MAX31855_C_ */
