/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "clock_task.h"
#include "max31855.h"
#include "measurement.h"
#include "hm-10.h"
#include <string.h>


max31855_t admision;
max31855_t escape;
max31855_t aceite;

max31855_t termocuplas[4];

void max31855_task(void* taskParamPtr);
void hm10_task(void* taskParamPtr);

int main(void)
{
	boardConfig();
	measurement_queue_init();
	clock_init();
	hm10_init();

	max31855(&admision, ENET_RXD1, "Tadm\0");
	admision.cs_pin = ENET_RXD1;
	max31855(&escape, ENET_MDC, "Tesc\0");
	escape.cs_pin = ENET_MDC;
	max31855(&aceite, ENET_CRS_DV, "Tace\0");
	aceite.cs_pin = ENET_CRS_DV;

//
//	aceite.cs_pin = GPIO3;
//	memset(aceite.buffer, 0, MAX31855_BUFFER_SIZE);

    max31855_init();

    BaseType_t res =
	xTaskCreate(
				clock_task,
				(const char*)"clock_task",
				configMINIMAL_STACK_SIZE*2,
				0,
				tskIDLE_PRIORITY+1,
				0
	);
    configASSERT(res == pdPASS);

    res =
    	xTaskCreate(
				max31855_task,
				(const char*)"admision_task",
				configMINIMAL_STACK_SIZE*2,
				(void*)&admision,
				tskIDLE_PRIORITY+1,
				0
    	);
    configASSERT(res == pdPASS);
    res =
		xTaskCreate(
				max31855_task,
				(const char*)"escape_task",
				configMINIMAL_STACK_SIZE*2,
				(void*)&escape,
				tskIDLE_PRIORITY+1,
				0
		);
    res =
		xTaskCreate(
				max31855_task,
				(const char*)"aceite_task",
				configMINIMAL_STACK_SIZE*2,
				(void*)&aceite,
				tskIDLE_PRIORITY+1,
				0
		);
	configASSERT(res == pdPASS);
    res =
    	xTaskCreate(
				hm10_task,
				(const char*)"bluetooth_task",
				configMINIMAL_STACK_SIZE*2,
				0,
				tskIDLE_PRIORITY+2,
				0
    	);

    vTaskStartScheduler();

    configASSERT(0);

    return TRUE;
}

void max31855_task(void* taskParamPtr)
{
	max31855_t* device = (max31855_t*)taskParamPtr;
	measurement_t max31855_measurement;
	uint32_t aux_value;
	int32_t internal_temp_raw;
	int32_t external_temp_raw;

	strcpy(max31855_measurement.name, device->name);
	while(TRUE)
	{
		aux_value = 0;
		internal_temp_raw = 0;
		external_temp_raw = 0;

		gpioWrite(LED2, ON);
		vTaskDelay(pdMS_TO_TICKS(100));

		taskENTER_CRITICAL();
		max31855_read(device);
		taskEXIT_CRITICAL();

		internal_temp_raw |= (int16_t)(device->buffer[3] >> 4);
		internal_temp_raw |= (int16_t)(device->buffer[2] << 4);
		if((internal_temp_raw & 0x800) != 0)
		{
			internal_temp_raw |= 0xF800;
			device->internal_temp = 10000 * -0.0625 * internal_temp_raw;
		}
		else
		{
			internal_temp_raw &= 0x07FF;
			device->internal_temp = 10000 * 0.0625 * internal_temp_raw;
		}

		external_temp_raw |= (int16_t)(device->buffer[1] >> 2);
		external_temp_raw |= (int16_t)(device->buffer[0] << 6);
		if((external_temp_raw & 0x0200) != 0)
		{
			external_temp_raw |= 0xC000;
			device->external_temp = 1000 * -0.025 * external_temp_raw;
		}
		else
		{
			external_temp_raw &= 0x3FFF;
			device->external_temp = 1000 * 0.025 * external_temp_raw;
		}

		max31855_measurement.value = device->internal_temp;
		max31855_measurement.date_time = clock_task_get_date_time();
		max31855_measurement.decimal_pos = MAX31855_INTERNAL_DEC_POS;
		max31855_measurement.fault_code = device->buffer[3] & 0x0F;
		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);

		gpioWrite(LED2, OFF);
		vTaskDelay(pdMS_TO_TICKS(900));
	}
}

void hm10_task(void* taskParamPtr)
{
	measurement_t measurement;
	while(TRUE)
	{
		if (pdTRUE == xQueueReceive(measurement_queue, &measurement, portMAX_DELAY))
		{
			taskENTER_CRITICAL();
			printf("%s", measurement_to_string(&measurement));
			taskEXIT_CRITICAL();
		}
	}
}
