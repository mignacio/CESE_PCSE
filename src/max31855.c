/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "sapi.h"
#include "sapi_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "max31855.h"
#include "clock_task.h"
#include "measurement.h"
#include <string.h>
#include <stdint.h>

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
	strncpy(device->name, name_str, 5);
}

void max31855_read(max31855_t* device)
{
	gpioWrite(device->cs_pin, OFF);
	spiRead(spi_port, device->buffer, MAX31855_BUFFER_SIZE);
	gpioWrite(device->cs_pin, ON);
}

int32_t max31855_ext_temp_to_celsius(max31855_t* device)
{
	int32_t external_temp_raw = 0;
	external_temp_raw = (((int32_t)device->buffer[1]) >> 2) +
						((int32_t)device->buffer[0] << 6);
	if(external_temp_raw & 0x00002000)
	{
		external_temp_raw |= 0xFFFFC000; //negativo, extiendo bit de signo 1
	}
	else
	{
		external_temp_raw &= 0x00003FFF; //positivo, extiendo bit de signo 0
	}
	device->external_temp = external_temp_raw;
	return ((device->external_temp) * 25); //* 0.25 * 100
}

int32_t max31855_int_temp_to_celsius(max31855_t* device)
{
	int32_t internal_temp_raw = 0;
	internal_temp_raw = (int32_t)(device->buffer[3] >> 4) +
						 (int32_t)(device->buffer[2] << 4);
	if((internal_temp_raw & 0x00000800) != 0)
	{
		internal_temp_raw |= 0xFFFFF000;//extiendo bit de signo, 1
	}
	else
	{
		internal_temp_raw &= 0x000007FF; //extiendo bit de signo, 0
	}

	device->internal_temp = internal_temp_raw;

	return ((device->internal_temp) * 0.0625 * 1000);
}
