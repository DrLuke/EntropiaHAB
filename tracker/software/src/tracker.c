#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dac.h>

#define PORT_LED GPIOC
#define PIN_LED GPIO8

#include "settings.h"
#include "ax25.h"
#include "dac.h"
#include "aprs.h"
#include "systick.h"

#define NOP __asm__("nop")

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

	gpio_setup();
	dac_setup();
	systick_setup();

	for(;;)
	{
		for(int a = 0; a < 50000; a++)
		{
			NOP;
		}
	}

	return 0;
}
