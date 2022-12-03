/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_SENSOR_TASKS_H
#define CESE_PCSE_SENSOR_TASKS_H

// Tareas para sensores de temperatura
void temp_sensor_task(void* taskParamPtr);

// Tareas para sensores analogicos
void analog_sensors_task(void* taskParamPtr);

#endif
