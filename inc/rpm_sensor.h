/*=============================================================================
 * Autor: Ignacio Moya, 2023.
 *===========================================================================*/
#ifndef CESE_PCSE_RPM_SENSOR_H
#define CESE_PCSE_RPM_SENSOR_H

typedef enum {
	RPM_SENSOR_ENABLED, RPM_SENSOR_DISABLED
} rpm_sensor_enabled_t;

void rpm_measure(void);

void GPIO0_IRQHandler(void);

#endif
