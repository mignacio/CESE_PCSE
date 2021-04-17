/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#ifndef CESE_PCSE_INC_HM_10_H_
#define CESE_PCSE_INC_HM_10_H_

#include "sapi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>

#define HM10_UART UART_USB
#define HM10_BAUDRATE 9600
#define HM10_STATE_PIN GPIO7

void hm10_init(void);

void hm10_send_at_command(uint8_t* command);

void hm10_send_string(uint8_t* string);

#endif /* CESE_PCSE_INC_HM_10_H_ */
