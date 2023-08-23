/*=============================================================================
 * Autor: Ignacio Moya, 2023.
 *===========================================================================*/
#include "sapi.h"
#include "rpm_sensor.h"
#include "FreeRTOS.h"
#include <string.h>

#define RAISING_EDGE 					0	  /* IRQs defines */

typedef struct {
   uint8_t irqChannel;				/* IRQ channel */
   gpioInitLpc4337_t gpioInit;	/* GPIO config: port and pin */

} rpm_sensor_irq_t;

typedef struct {
	uint32_t value;
	uint32_t scale;
	uint8_t name[5];
	gpioMap_t rpm_gpio;						/* Gpio configured for echo pin */
	uint8_t enabled;						/* boolean flag */
	rpm_sensor_irq_t irqConfig;
} rpm_sensor_t;

rpm_sensor_t rpm_sensor;

static void enableGPIOIrq(uint8_t irqChannel, uint8_t port, uint8_t pin){

   /*
    * Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
    * From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
    */
   Chip_SCU_GPIOIntPinSel(irqChannel , port, pin);
   /* Clear actual configured interrupt status */
   Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
   /* Set edge interrupt mode */
   Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(irqChannel));

   Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(irqChannel));

   /* Clear pending irq channel interrupts */
   NVIC_ClearPendingIRQ(PIN_INT0_IRQn + irqChannel);
   /* Enable irqChannel interrupt */
   NVIC_EnableIRQ(PIN_INT0_IRQn + irqChannel);
}

static void disableGPIOIrq(uint8_t irqChannel){
   /* Clear pending irq channel interrupts */
   NVIC_ClearPendingIRQ(PIN_INT0_IRQn + irqChannel);
   /* Disable irqChannel interrupt */
   NVIC_DisableIRQ(PIN_INT0_IRQn + irqChannel);
}

uint8_t cycles = 0;
static void serveInterrupt(uint8_t irqChannel){
	if ( Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH(irqChannel) ) {
		cycles++;
	}
	/* Clear IRQ status */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,PININTCH(irqChannel));
}

void rpm_init(uint8_t* name){
	strncpy(rpm_sensor.name, name, 5);
	gpioInit(GPIO1, GPIO_INPUT);
	rpm_sensor.value = 0;
	rpm_sensor.scale = 0;
	rpm_sensor.rpm_gpio = GPIO1;
	rpm_sensor.irqConfig.irqChannel = 3;
	rpm_sensor.irqConfig.gpioInit.port = 3;
	rpm_sensor.irqConfig.gpioInit.pin = 3;

	//init irqs
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	cycles = 0;
}

uint32_t rpm_measure(){
	cycles = 0;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	// Enable GPIO IRQ
	enableGPIOIrq(rpm_sensor.irqConfig.irqChannel,
			rpm_sensor.irqConfig.gpioInit.port, rpm_sensor.irqConfig.gpioInit.pin);

	vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));

	//Disable it so we don't disturb other tasks.
	disableGPIOIrq(rpm_sensor.irqConfig.irqChannel);
	return cycles;
}

void GPIO3_IRQHandler(void)
{
   serveInterrupt(rpm_sensor.irqConfig.irqChannel);
   cycles++;
}
