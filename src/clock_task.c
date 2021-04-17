/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "sapi.h"
#include "clock_task.h"
#include "clock_task.h"

#define LED_RATE_MS 500
#define LOADING_RATE_MS 250

#define LED_RATE pdMS_TO_TICKS(LED_RATE_MS)
#define LOADING_RATE pdMS_TO_TICKS(LOADING_RATE_MS)

rtc_t date_time;

DEBUG_PRINT_ENABLE;

void clock_init(void)
{
	//No es posible usar rtcInit y FreeRTOS porque rtcInit llama
	//a la funcion de sapi "delay"
	Chip_RTC_Init(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC, ENABLE);

	//vTaskDelay(pdMS_TO_TICKS(2100));

	date_time.year = 2021;
	date_time.month = 4;
	date_time.mday = 1;
	date_time.wday = 4;
	date_time.hour = 3;
	date_time.min = 2;
	date_time.sec = 0;
	rtcWrite(&date_time);

	debugPrintConfigUart(UART_USB, 115200);
	printf("Clock Init OK\r\n");
}

void clock_task(void* taskParamPtr)
{
    while(TRUE)
    {
    	taskENTER_CRITICAL();
        rtcRead(&date_time);
        taskEXIT_CRITICAL();
        vTaskDelay(pdMS_TO_TICKS(LED_RATE));
    }
}

rtc_t clock_task_get_date_time()
{
	return date_time;
}
