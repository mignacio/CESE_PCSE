/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "max31855.h"
#include "clock_task.h"
#include "measurement.h"
#include <string.h>

spiMap_t spi_port = SPI0;

void max31855_init()
{
	spiInit(spi_port);
}

void max31855(max31855_t* device, gpioMap_t chip_select, uint8_t* name_str)
{
	device->cs_pin = chip_select;
	gpioInit(device->cs_pin, GPIO_OUTPUT);
	memset(device->buffer, 0, MAX31855_BUFFER_SIZE);
	strcpy(device->name, name_str);//, MAX31855_BUFFER_SIZE);
}

void max31855_read(max31855_t* device)
{
	gpioWrite(device->cs_pin, OFF);
	spiRead(spi_port, device->buffer, MAX31855_BUFFER_SIZE);
	gpioWrite(device->cs_pin, ON);
}
