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
#include "hm-10.h"
#include <strings.h>



void max31855_task(void* taskParamPtr);
void analog_task(void* taskParamPtr);
void hm10_task(void* taskParamPtr);
void assert_task_init(BaseType_t res);

TaskHandle_t task_handle_init;

int main(void)
{
	boardConfig();

	measurement_queue_init();

	BaseType_t res;
	res = xTaskCreate(clock_task,
					(const char*)"clock_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+1,
					0);
	assert_task_init(res);

	hm10_init();
	res = xTaskCreate(hm10_task,
					(const char*)"bluetooth_task",
					configMINIMAL_STACK_SIZE*2,
					0,
					tskIDLE_PRIORITY+1,
					0);
	assert_task_init(res);


	analog_sensor_t o2_sensor, vbatt_sensor, pres_aceite_sensor;
	analog_sensors_init();
	analog_sensor(&o2_sensor, 32, "_O2_\0", CH2);
	o2_sensor.adc_pin = CH2;
	res = xTaskCreate(analog_task,
					(const char*)"o2_task",
					configMINIMAL_STACK_SIZE*2,
					(void*)&o2_sensor,
					tskIDLE_PRIORITY+2,
					0
		);
	assert_task_init(res);

	analog_sensor(&vbatt_sensor, 32, "Vbat", CH1);
	vbatt_sensor.adc_pin = CH1;
//	res = xTaskCreate(analog_task,
//					(const char*)"vbatt_task",
//					configMINIMAL_STACK_SIZE*2,
//					(void*)&vbatt_sensor,
//					tskIDLE_PRIORITY+2,
//					0
//		);
//	assert_task_init(res);

	analog_sensor(&pres_aceite_sensor, 32, "Pace\0", CH3);
	pres_aceite_sensor.adc_pin = CH3;
//	res = xTaskCreate(analog_task,
//					(const char*)"pace_task",
//					configMINIMAL_STACK_SIZE*2,
//					(void*)&pres_aceite_sensor,
//					tskIDLE_PRIORITY+2,
//					0
//		);
//	assert_task_init(res);

	max31855_t admision, escape, aceite;
	max31855_init();

	admision.cs_pin = LCD3;
	max31855(&admision, LCD3, "Tadm\0");
	res = xTaskCreate(max31855_task,
					(const char*)"admision_task",
					configMINIMAL_STACK_SIZE*2,
					(void*)&admision,
					tskIDLE_PRIORITY+3,
					0);
	assert_task_init(res);

	escape.cs_pin = LCDRS;
	max31855(&escape, LCDRS, "Tesc\0");
	res = xTaskCreate(max31855_task,
					(const char*)"escape_task",
					configMINIMAL_STACK_SIZE*2,
					(void*)&escape,
					tskIDLE_PRIORITY+3,
					0);
	assert_task_init(res);

	aceite.cs_pin = LCD4;
	max31855(&aceite, LCD4, "Tace\0");
	res = xTaskCreate(max31855_task,
					(const char*)"aceite_task",
					configMINIMAL_STACK_SIZE*2,
					(void*)&aceite,
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

void analog_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t xPeriodicity = pdMS_TO_TICKS(1000);

	analog_sensor_t* analog_sensor = (analog_sensor_t*)taskParamPtr;
	measurement_t analog_measurement;
	analog_measurement.decimal_pos = VBATT_DEC_POS;
	analog_measurement.fault_code = 0;
	measurement_set_name(&analog_measurement, analog_sensor->name[0]);

	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();
		analog_measurement.ticks = xLastWakeTime;
		taskENTER_CRITICAL();
		switch(analog_sensor->adc_pin)
		{
		case CH1:
			analog_measurement.value = analog_sensor_read(analog_sensor, CH1);
		break;
		case CH2:
			analog_measurement.value = analog_sensor_read(analog_sensor, CH2);
		break;
		case CH3:
		default:
			analog_measurement.value = analog_sensor_read(analog_sensor, CH1);
		break;
		}
		taskEXIT_CRITICAL();
		xQueueSend(measurement_queue, &analog_measurement, portMAX_DELAY);

		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void max31855_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t xPeriodicity = pdMS_TO_TICKS(5000);

	max31855_t* device = (max31855_t*)taskParamPtr;
	measurement_t max31855_measurement;
	max31855_measurement.decimal_pos = MAX31855_EXTERNAL_DEC_POS;
	measurement_set_name(&max31855_measurement, &device->name[0]);

	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();

		max31855_measurement.ticks = xLastWakeTime;
		taskENTER_CRITICAL();
		max31855_read(device);
		taskEXIT_CRITICAL();

		device->external_temp = max31855_ext_temp_to_celsius(device);

		max31855_measurement.value = device->external_temp;
		max31855_measurement.fault_code = device->buffer[3] & 0x0F;

		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);
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
