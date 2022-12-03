/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_SENSOR_TASKS_H
#define CESE_PCSE_SENSOR_TASKS_H

// Tareas para sensores de temperatura
void adm_temp_task(void* taskParamPtr);
void esc_temp_task(void* taskParamPtr);
void ace_temp_task(void* taskParamPtr);

// Tareas para sensores analogicos
void vbatt_task(void* taskParamPtr);
void ace_pres_task(void* taskParamPtr);
void o2_task(void* taskParamPtr);

#endif
