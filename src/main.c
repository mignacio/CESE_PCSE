/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "clock_task.h"
#include "max31855.h"
#include <string.h>


max31855_t admision;
max31855_t escape;
max31855_t aceite;

max31855_t termocuplas[4];

int main(void)
{
	admision.cs_pin = SGPIO13;
	memset(admision.buffer, 0, MAX31855_BUFFER_SIZE);

	escape.cs_pin = GPIO2;
	memset(escape.buffer, 0, MAX31855_BUFFER_SIZE);

	aceite.cs_pin = GPIO3;
	memset(aceite.buffer, 0, MAX31855_BUFFER_SIZE);

    boardConfig();
    clock_init();
    max31855_init();

    BaseType_t res =
	xTaskCreate(
				clock_task,
				(const char*)"clock_task",
				configMINIMAL_STACK_SIZE*2,
				0,
				tskIDLE_PRIORITY+1,
				0
	);
    configASSERT(res == pdPASS);

    res =
    	xTaskCreate(
				max31855_task,
				(const char*)"themocouple_task",
				configMINIMAL_STACK_SIZE*2,
				(void*)&admision,
				tskIDLE_PRIORITY+1,
				0
    	);
    configASSERT(res == pdPASS);

    vTaskStartScheduler();

    configASSERT(0);

    return TRUE;
}


