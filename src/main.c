/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "clock_task.h"
#include "max31855.h"
#include "analog_sensors.h"
#include "measurement.h"
#include "sensor_tasks.h"
#include "hm-10.h"
#include <string.h>

void hm10_task(void* taskParamPtr);
void assert_task_init(BaseType_t res);

TaskHandle_t task_handle_init;

int main(void)
{
	boardConfig();

	measurement_queue_init();

	BaseType_t res;
	res = xTaskCreate(hm10_task,
					(const char*)"bluetooth_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+1,
					0);
	assert_task_init(res);

	res = xTaskCreate(temp_sensor_task,
					(const char*)"temp_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+2,
					0);
	assert_task_init(res);

	res = xTaskCreate(analog_sensors_task,
					(const char*)"analog_sensors",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+3,
					0);
	assert_task_init(res);

	res = xTaskCreate(rpm_sensor_task,
					(const char*)"rpm_sensors",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+4,
					0);
		assert_task_init(res);

	gpioWrite(LED3, ON);

	vTaskStartScheduler();

    configASSERT(0);

    return TRUE;
}

void assert_task_init(BaseType_t res)
{
	if(res != pdPASS)
	{
		gpioWrite(LEDR, ON);
	}
	configASSERT(res == pdPASS);
}

void hm10_task(void* taskParamPtr)
{
	measurement_t measurement;

	hm10_init();
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
