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

void peripherals_init_task(void* taskParamPtr);
void max31855_task(void* taskParamPtr);
void hm10_task(void* taskParamPtr);

TaskHandle_t task_handle_init;

int main(void)
{
	boardConfig();

	BaseType_t res =
	xTaskCreate(
				peripherals_init_task,
				(const char*)"peripherals_init_task",
				configMINIMAL_STACK_SIZE*2,
				0,
				tskIDLE_PRIORITY+1,
				&task_handle_init
	);
	configASSERT(res == pdPASS);
    vTaskStartScheduler();

    configASSERT(0);

    return TRUE;
}
void peripherals_init_task(void* taskParamPtr)
{
	measurement_queue_init();

		hm10_init();

		max31855(&admision, LCD3, "Tadm\0");
		admision.cs_pin = LCD3;
		max31855(&escape, LCDRS, "Tesc\0");
		escape.cs_pin = LCDRS;
		max31855(&aceite, LCD4, "Tace\0");
		aceite.cs_pin = LCD4;

	    max31855_init();
	    clock_init();

	    BaseType_t res =
		xTaskCreate(
					clock_task,
					(const char*)"clock_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+2,
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

	    vTaskSuspend(task_handle_init); //mi trabajo aquí ha terminado
}
//#define QUEUE_DEBUG
void max31855_task(void* taskParamPtr)
{
	max31855_t* device = (max31855_t*)taskParamPtr;
	measurement_t max31855_measurement;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t xPeriodicity = pdMS_TO_TICKS(2000);
	uint32_t aux_value;
	int32_t internal_temp_raw;
	int32_t external_temp_raw;

	strcpy(max31855_measurement.name, device->name);
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();
		aux_value = 0;
		internal_temp_raw = 0;
		external_temp_raw = 0;
#ifdef QUEUE_DEBUG
		gpioWrite(LED2, ON);
		vTaskDelay(pdMS_TO_TICKS(100));
#endif
		taskENTER_CRITICAL();
		max31855_read(device);
		taskEXIT_CRITICAL();

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

		device->internal_temp = internal_temp_raw * 0.0625 * 1000;

		int32_t external_temp_raw = 0;
		external_temp_raw = (int32_t)(device->buffer[1] >> 2) +
							(int32_t)(device->buffer[0] << 6);
		if((external_temp_raw & 0x00002000) != 0)
		{
			external_temp_raw |= 0xFFFFC000;//extiendo bit de signo, 1
		}
		else
		{
			external_temp_raw &= 0x00003FFF; //extiendo bit de signo, 0
		}
		device->external_temp = external_temp_raw * 25; // * 100 * 0.25

		max31855_measurement.value = device->external_temp;
		max31855_measurement.date_time = clock_task_get_date_time();
		max31855_measurement.decimal_pos = MAX31855_EXTERNAL_DEC_POS;
		max31855_measurement.fault_code = device->buffer[3] & 0x0F;

		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);
#ifdef QUEUE_DEBUG
		gpioWrite(LED2, OFF);
#endif
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void hm10_task(void* taskParamPtr)
{
	measurement_t measurement;
	while(TRUE)
	{
		if(gpioRead(HM10_STATE_PIN))
		{
			gpioWrite(LEDB, ON);
			if(pdTRUE == xQueueReceive(measurement_queue, &measurement, portMAX_DELAY))
			{
				taskENTER_CRITICAL();
				hm10_send_string(measurement_to_string(&measurement));
				taskEXIT_CRITICAL();
			}
		}
		else
		{
			gpioWrite(LEDB, ON);
			vTaskDelay(pdMS_TO_TICKS(500));
			gpioWrite(LEDB, OFF);
			vTaskDelay(pdMS_TO_TICKS(500));
		}

	}
}
