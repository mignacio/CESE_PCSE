/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "sapi.h"

#ifndef CESE_PCSE_INC_MAX31855_C_
#define CESE_PCSE_INC_MAX31855_C_

#define MAX31855_BUFFER_SIZE 4
typedef struct
{
	//spiMap_t spi_port;
	gpioMap_t cs_pin;
	uint8_t buffer[MAX31855_BUFFER_SIZE];

}max31855_t;

void max31855_init(void);
void max31855_read_temp(max31855_t* device);
void max31855_task(void* taskParamPtr);

#endif /* CESE_PCSE_INC_MAX31855_C_ */
