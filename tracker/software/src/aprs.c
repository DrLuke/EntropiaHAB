#include "aprs.h"

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>

volatile static bool nextBitReady = true;

void aprs_sendPacket(char packet[], int packetLen)
{
	timer_reset(TIM2);				// Reset all settings
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);	// Enable clock to timer
	nvic_enable_irq(NVIC_TIM2_IRQ);			// Enable timer 3 IRQ
	timer_enable_irq(TIM2, TIM_DIER_UIE);		// Enable update interrupt
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_period(TIM2, (F_CPU / 1200));		// Set timer to 1200 baud

	timer_enable_counter(TIM2);


	for(int bytei = 0; bytei < packetLen; bytei++)
	{
		for(int biti = 0; biti < 8; biti++)
		{
			char bit = (packet[bytei] >> (7-biti)) & 0x01;	// Extract bit to transmit
			// Bell 202 Modem frequencies
			while(!nextBitReady);	// Wait until the next bit can be sent
			if(bit)			// If bit = 1, send 1.2kHz tone
			{
				dac_tone(1200);
			}
			else			// If bit = 0, send 2.2 kHz tone
			{
				dac_tone(2200);
			}
			nextBitReady = false;
		}
	}
	while(!nextBitReady);			// Wait for last bit to be transmitted
	
	dac_tone(0);				// Disable DAC output
	nvic_disable_irq(NVIC_TIM2_IRQ);	// Disable Timer 2 interrupt
	timer_disable_counter(TIM2);		// Disable counter
	rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);	// Disable clock to counter
}


void tim2_isr(void)
{
	timer_clear_flag(TIM2, TIM_SR_UIF);	// Clear Interrupt
	nextBitReady = true;	// Allow transmission of next bit
	gpio_toggle(GPIOC, GPIO8);
}
