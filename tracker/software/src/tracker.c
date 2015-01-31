#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>


#define PORT_LED GPIOC
#define PIN_LED GPIO8

#include "ax25.h"

static void gpio_setup(void)
{
	/* Enable GPIOB clock. */
	/* Manually: */
	//RCC_AHBENR |= RCC_AHBENR_GPIOCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);


	/* Set GPIO6 (in GPIO port B) to 'output push-pull'. */
	/* Using API functions: */
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LED);
	gpio_mode_setup(PORT_LED, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
}

static void usart_setup()
{
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO9);

	gpio_set_af(GPIOA, GPIO_AF1, GPIO9);

	rcc_periph_clock_enable(RCC_USART1);
	usart_set_baudrate(USART1, 57600);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_CR2_STOP_1_0BIT);
	usart_set_mode(USART1, USART_MODE_TX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);
}


int main(void)
{
	int i;

	gpio_setup();
	gpio_set(PORT_LED, PIN_LED);

	usart_setup();

	/* Blink the LED (PC8) on the board. */
	while (1) 
	{
		gpio_toggle(PORT_LED, PIN_LED);

		char packet[256];

		char dest[] = "DEST  ";
		char source[] = "SOURCE";

		char info[] = ">INFORMATION GOES HERE";
		int infolen = 60;

		ax25_constructPacket(packet, dest, source, info, infolen);

		for(int i = 0; i < 256; i++)
		{
			usart_send_blocking(USART1, packet[i]);
		}

		for (i = 0; i < 500000; i++) 
		{
			__asm__("NOP");
		}

	}

	return 0;
}
