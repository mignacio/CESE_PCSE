/*=============================================================================
 * Autor: Ignacio Moya, 2021.
 *===========================================================================*/

#include "hm-10.h"

void hm10_init()
{
	gpioInit(HM10_STATE_PIN, GPIO_INPUT);
	uartInit(HM10_UART, HM10_BAUDRATE);
	printf("hmlisto\r\n");
	hm10_send_at_command("NAMEPCESE-PCSE");//seteo nombre del módulo
	hm10_send_at_command("TYPE0");//conectar sin pedir clave
}

void hm10_send_at_command(uint8_t* command)
{

}

void hm10_send_string(uint8_t* string)
{

}
