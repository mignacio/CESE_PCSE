/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/

#include "sensor_tasks.h"
#include "sapi.h"
#include "max31855.h"
#include "analog_sensors.h"
#include "measurement.h"
#include "sapi.h"
#include "FreeRTOS.h"
#include <string.h>

void vbatt_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(1000);

	analog_sensor_t vbatt_sensor;
	vbatt_sensor.scale = 32;
	vbatt_sensor.adc_pin = CH1;
	strncpy(vbatt_sensor.name, "Vbat\0", 5);

	measurement_t analog_measurement;
	analog_measurement.decimal_pos = VBATT_DEC_POS;
	analog_measurement.fault_code = 0;
	measurement_set_name(&analog_measurement, vbatt_sensor.name);

	vTaskDelay(pdMS_TO_TICKS(100));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();
		analog_measurement.ticks = xLastWakeTime;

		analog_measurement.value = analog_sensor_read(&vbatt_sensor);

		xQueueSend(measurement_queue, &analog_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void ace_pres_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(1000);

	analog_sensor_t ace_pres;
	ace_pres.scale = 32;
	ace_pres.adc_pin = CH3;
	strncpy(ace_pres.name, "Pace\0", 5);

	measurement_t analog_measurement;
	analog_measurement.decimal_pos = VBATT_DEC_POS;
	analog_measurement.fault_code = 0;
	measurement_set_name(&analog_measurement, ace_pres.name);

	vTaskDelay(pdMS_TO_TICKS(200));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();
		analog_measurement.ticks = xLastWakeTime;

		analog_measurement.value = analog_sensor_read(&ace_pres);

		xQueueSend(measurement_queue, &analog_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void o2_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(1000);

	analog_sensor_t o2_sensor;
	o2_sensor.scale = 32;
	o2_sensor.adc_pin = CH2;
	strncpy(o2_sensor.name, "_O2_\0", 5);

	measurement_t analog_measurement;
	analog_measurement.decimal_pos = VBATT_DEC_POS;
	analog_measurement.fault_code = 0;
	measurement_set_name(&analog_measurement, o2_sensor.name);

	vTaskDelay(pdMS_TO_TICKS(300));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();
		analog_measurement.ticks = xLastWakeTime;

		analog_measurement.value = analog_sensor_read(&o2_sensor);

		xQueueSend(measurement_queue, &analog_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void adm_temp_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(2500);

	max31855_t adm_temp;
	strncpy(adm_temp.name, "Tadm\0", 5);
	memset(adm_temp.buffer, 0, MAX31855_BUFFER_SIZE);
	adm_temp.cs_pin = LCD3;
	gpioInit(adm_temp.cs_pin, GPIO_OUTPUT);

	measurement_t max31855_measurement;
	max31855_measurement.decimal_pos = MAX31855_EXTERNAL_DEC_POS;
	measurement_set_name(&max31855_measurement, adm_temp.name);

	vTaskDelay(pdMS_TO_TICKS(400));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();

		max31855_measurement.ticks = xLastWakeTime;
		taskENTER_CRITICAL();
		max31855_read(&adm_temp);
		taskEXIT_CRITICAL();

		//device->external_temp = max31855_ext_temp_to_celsius(device);
		adm_temp.external_temp = max31855_int_temp_to_celsius(&adm_temp);

		max31855_measurement.value = adm_temp.external_temp;
		max31855_measurement.fault_code = adm_temp.buffer[3] & 0x0F;

		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void esc_temp_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(2500);

	max31855_t esc_temp;
	strncpy(esc_temp.name, "Tesc\0", 5);
	memset(esc_temp.buffer, 0, MAX31855_BUFFER_SIZE);
	esc_temp.cs_pin = LCDRS;
	gpioInit(esc_temp.cs_pin, GPIO_OUTPUT);

	measurement_t max31855_measurement;
	max31855_measurement.decimal_pos = MAX31855_EXTERNAL_DEC_POS;
	measurement_set_name(&max31855_measurement, esc_temp.name);

	vTaskDelay(pdMS_TO_TICKS(500));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();

		max31855_measurement.ticks = xLastWakeTime;
		taskENTER_CRITICAL();
		max31855_read(&esc_temp);
		taskEXIT_CRITICAL();

		//device->external_temp = max31855_ext_temp_to_celsius(device);
		esc_temp.external_temp = max31855_int_temp_to_celsius(&esc_temp);

		max31855_measurement.value = esc_temp.external_temp;
		max31855_measurement.fault_code = esc_temp.buffer[3] & 0x0F;

		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

void ace_temp_task(void* taskParamPtr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriodicity = pdMS_TO_TICKS(2500);

	max31855_t ace_temp;
	strncpy(ace_temp.name, "Tace\0", 5);
	memset(ace_temp.buffer, 0, MAX31855_BUFFER_SIZE);
	ace_temp.cs_pin = LCD4;
	gpioInit(ace_temp.cs_pin, GPIO_OUTPUT);

	measurement_t max31855_measurement;
	max31855_measurement.decimal_pos = MAX31855_EXTERNAL_DEC_POS;
	measurement_set_name(&max31855_measurement, ace_temp.name);

	vTaskDelay(pdMS_TO_TICKS(600));
	while(TRUE)
	{
		xLastWakeTime = xTaskGetTickCount();

		max31855_measurement.ticks = xLastWakeTime;
		taskENTER_CRITICAL();
		max31855_read(&ace_temp);
		taskEXIT_CRITICAL();

		//device->external_temp = max31855_ext_temp_to_celsius(device);
		ace_temp.external_temp = max31855_int_temp_to_celsius(&ace_temp);

		max31855_measurement.value = ace_temp.external_temp;
		max31855_measurement.fault_code = ace_temp.buffer[3] & 0x0F;

		xQueueSend(measurement_queue, &max31855_measurement, portMAX_DELAY);
		vTaskDelayUntil(&xLastWakeTime, xPeriodicity);
	}
}

