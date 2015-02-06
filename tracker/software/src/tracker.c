#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dac.h>

#define PORT_LED GPIOC
#define PIN_LED GPIO8

#include "settings.h"
#include "ax25.h"
#include "dac.h"

static void gpio_setup(void)
{
	/* Enable GPIOB clock. */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);


	/* Set GPIO6 (in GPIO port B) to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
}


int main(void)
{
	rcc_clock_setup_in_hsi_out_48mhz();
	int i;
	int c = 0x00;

	gpio_setup();
	gpio_set(PORT_LED, PIN_LED);


	
	dac_setup();
	dac_tone(2000);

	/* Blink the LED (PC8) on the board. */
	for(;;)
	{
		dac_tone(2200);
		gpio_toggle(GPIOC, GPIO8);

		for (i = 0; i < 50000; i++) 
		{
			__asm__("NOP");
		}


		dac_tone(1200);
		for (i = 0; i < 50000; i++) 
		{
			__asm__("NOP");
		}

		dac_tone(0);
		for (i = 0; i < 50000; i++) 
		{
			__asm__("NOP");
		}
	}

	return 0;
}
