/*=============================================================================
 * Autor: Ignacio Moya, 2023.
 *===========================================================================*/
#ifndef CESE_PCSE_RPM_SENSOR_H
#define CESE_PCSE_RPM_SENSOR_H

void rpm_init(uint8_t* name);
uint32_t rpm_measure(void);

void GPIO3_IRQHandler(void);

#endif
