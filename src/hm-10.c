/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/

#include "hm-10.h"
#include "sapi.h"
#include "FreeRTOS.h"

void hm10_init()
{
	gpioInit(HM10_STATE_PIN, GPIO_INPUT);
	uartInit(HM10_UART, HM10_BAUDRATE);
	printf("hmlisto\r\n");
	//hm10_send_at_command("RESET");
	hm10_send_at_command("NAMEICE-ADQ");
	hm10_send_at_command("TYPE0");//conectar pidiendo clave
	//hm10_send_at_command("PASS123456");
}

void hm10_send_at_command(uint8_t* command)
{
	uartWriteString(HM10_UART, "AT+");
	uartWriteString(HM10_UART, command);
	uartWriteString(HM10_UART, "\r\n");
	vTaskDelay(pdMS_TO_TICKS(100));
}

void hm10_send_string(uint8_t* string)
{
	uartWriteString(HM10_UART, string);
}
