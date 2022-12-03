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

	hm10_init();
	res = xTaskCreate(hm10_task,
					(const char*)"bluetooth_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+1,
					0);
	assert_task_init(res);

	res = xTaskCreate(adm_temp_task,
					(const char*)"admision_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+2,
					0);
	assert_task_init(res);

	res = xTaskCreate(esc_temp_task,
					(const char*)"escape_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+2,
					0);
	assert_task_init(res);

	res = xTaskCreate(ace_temp_task,
					(const char*)"aceite_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+2,
					0);
	assert_task_init(res);

	res = xTaskCreate(vbatt_task,
					(const char*)"vbatt_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+3,
					0);
	assert_task_init(res);

	res = xTaskCreate(ace_pres_task,
					(const char*)"aceite_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+3,
					0);
	assert_task_init(res);

	res = xTaskCreate(o2_task,
					(const char*)"o2_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+3,
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
