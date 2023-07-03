/*=============================================================================
 * Autor: Ignacio Moya, 2023.
 *===========================================================================*/
#include "sapi.h"
#include "rpm_sensor.h"
#include "timer_18xx_43xx.h"
#include "FreeRTOS.h"

#define RAISING_EDGE 					0	  /* IRQs defines */
#define FALLING_EDGE 					1
#define BOTH_EDGES 						2
#define TICKS_TO_US_FACTOR				(204000000 / 1000000 )	/* Ticks = us * LPC_MAX_FREQ / 1000000 */

typedef struct {
   uint8_t irqChannel;				/* IRQ channel */
   gpioInitLpc4337_t gpioInit;	/* GPIO config: port and pin */

} rpm_sensor_irq_t;

typedef struct {
	uint32_t value;
	uint32_t scale;
	uint8_t name[5];
	uint32_t echoRiseTime,					/* Ticks of echo pulse rise edge */
			echoFallTime,					/* Ticks of echo pulse fall edge */
			lastEchoWidth;					/* Echo pulse width in ticks */
	gpioMap_t rpm_gpio;						/* Gpio configured for echo pin */
	uint8_t enabled;						/* boolean flag */
	rpm_sensor_irq_t irqConfig;	/* Sensor IRQ config */
} rpm_sensor_t;

// echoRiseTime | echoFallTime | lastEchoWidth | echoGpio | enabled | irqConfig (irqChannel, GpioPort, GpioPin) | position */
rpm_sensor_t rpm_sensor = {0 , 0 , 0 , GPIO1 , RPM_SENSOR_DISABLED, { 0, { 3, 4 } } };
uint32_t period_measurements[8] = {0, 0, 0, 0, 0, 0, 0, 0};
volatile bool measurement_done = FALSE;

static void initGPIOIrqs(){

   Chip_PININT_Init(LPC_GPIO_PIN_INT);
}

static void enableGPIOIrq(uint8_t irqChannel, uint8_t port, uint8_t pin, uint8_t edge){

   /*
    * Select irq channel to handle a GPIO interrupt, using its port and pin to specify it
    * From EduCiaa pin out spec: GPIO1[9] -> port 1 and pin 9
    */
   Chip_SCU_GPIOIntPinSel(irqChannel , port, pin);
   /* Clear actual configured interrupt status */
   Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
   /* Set edge interrupt mode */
   Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(irqChannel));

   if ( edge == RAISING_EDGE) {
      /* Enable high edge gpio interrupt */
      Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
   } else if ( edge == FALLING_EDGE) {
      /* Enable low edge gpio interrupt */
      Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
   } else {
      /* Enable high and low edge */
      Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
      Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(irqChannel));
   }

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

static void clearInterrupt(uint8_t irqChannel){
   /* Clear interrupt flag for irqChannel */
   Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,PININTCH(irqChannel));
}

static void serveInterrupt(uint8_t irqChannel){
	static uint8_t cycles = 0;

	if ( Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH(irqChannel) ) {
		//TODO add method to sapi_timer.h in order to get a timer count -> avoid using LPC_TIMER0 directly

		/* Save actual timer count in echoRiseTime */
		rpm_sensor.echoRiseTime = Chip_TIMER_ReadCount(LPC_TIMER0);
		/* Clear rise edge irq */
		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT,PININTCH(irqChannel));
	}
	else {
		//TODO add method to sapi_timer.h in order to get a timer count -> avoid using LPC_TIMER0 directly
		//TODO echoFallTime may not be necesary

		/* Save actual timer count in echoFallTime */
		rpm_sensor.echoFallTime = Chip_TIMER_ReadCount(LPC_TIMER0);
		/* Compute echo pulse width in timer ticks and save in lastEchoWidth */
		rpm_sensor.lastEchoWidth = rpm_sensor.echoFallTime - rpm_sensor.echoRiseTime;
		period_measurements[cycles] = rpm_sensor.echoFallTime - rpm_sensor.echoRiseTime;
		/* Clear falling edge irq */
		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT,PININTCH(irqChannel));
	}
	cycles++;
	if(cycles >= 8){
		cycles = 0;
		measurement_done = TRUE;
	}
	/* Clear IRQ status */
	clearInterrupt(irqChannel);
}

void rpm_init(uint8_t* name){
	strncpy(rpm_sensor.name, name, 5);
	gpioInit(rpm_sensor.rpm_gpio, GPIO_INPUT);
}

uint32_t rpm_measure(){
	measurement_done = FALSE;

	// Enable GPIO IRQ
	enableGPIOIrq(rpm_sensor.irqConfig.irqChannel,
				rpm_sensor.irqConfig.gpioInit.port,
				rpm_sensor.irqConfig.gpioInit.pin,
				BOTH_EDGES);
	//wait until measurements are taken
	while(measurement_done != TRUE){

	};
	measurement_done = FALSE;
	//send data to the queue

	//Disable it so we don't disturb other tasks.
	disableGPIOIrq(rpm_sensor.irqConfig.irqChannel);
	return rpm_sensor.lastEchoWidth;
}

void GPIO0_IRQHandler(void)
{
   serveInterrupt(0);
}
