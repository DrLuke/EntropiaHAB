#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define PORT_LED GPIOC
#define PIN_LED GPIO8

static void gpio_setup(void)
{
	/* Enable GPIOB clock. */
	/* Manually: */
	//RCC_AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOC);


	/* Set GPIO6 (in GPIO port B) to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
}

int main(void)
{
	int i;

	gpio_setup();
	gpio_set(PORT_LED, PIN_LED);

	/* Blink the LED (PC8) on the board. */
	while (1) 
	{
		gpio_toggle(PORT_LED, PIN_LED);	/* LED on/off */
		gpio_toggle(PORT_LED, GPIO9);
		for (i = 0; i < 1000000; i++) 
		{
			/* Wait a bit. */
			__asm__("nop");
		}
	}

	return 0;
}
