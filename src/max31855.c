/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "max31855.h"

spiMap_t spi_port = SPI0;

void max31855_init()
{
	spiInit(spi_port);
}

void max31855_read_temp(max31855_t* device)
{
	gpioWrite(device->cs_pin, OFF);
	spiRead(spi_port, device->buffer, MAX31855_BUFFER_SIZE);
	gpioWrite(device->cs_pin, ON);
}

void max31855_task(void* taskParamPtr)
{
	max31855_t device = *((max31855_t*)taskParamPtr);

	while(TRUE)
	{
		gpioWrite(LED2, ON);
		vTaskDelay(pdMS_TO_TICKS(100));
		max31855_read_temp(taskParamPtr);
		printf("%02d, %02d, %02d, %02d\r\n",
				device.buffer[0],
				device.buffer[1],
				device.buffer[2],
				device.buffer[3]);
		gpioWrite(LED2, OFF);
		vTaskDelay(pdMS_TO_TICKS(900));
	}
}
