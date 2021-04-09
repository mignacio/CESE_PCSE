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

void clock_init()
{
	//No es posible usar rtcInit y FreeRTOS porque rtcInit llama
	//a la funcion de sapi "delay"
	Chip_RTC_Init(LPC_RTC);
	Chip_RTC_Enable(LPC_RTC, ENABLE);


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
        gpioWrite(LEDB, ON);
        rtcRead(&date_time);
        printf("%02d/%02d/%04d, %02d:%02d:%02d\r\n",
        		date_time.mday, date_time.month, date_time.year,
				date_time.hour, date_time.min, date_time.sec );

        vTaskDelay(LED_RATE_MS / portTICK_RATE_MS);

        gpioWrite(LEDB,OFF);
        vTaskDelay(LED_RATE);
    }
}
