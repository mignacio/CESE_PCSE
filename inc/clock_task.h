/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_CLOCK_TASK_H_
#define CESE_PCSE_INC_CLOCK_TASK_H_

#include "sapi.h"

void clock_init(void);

void clock_task(void* taskParamPtr);

rtc_t clock_task_get_date_time();

#endif /* CESE_PCSE_INC_CLOCK_TASK_H_ */
