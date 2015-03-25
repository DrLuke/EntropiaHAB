#include "aprs.h"
#include "ax25.h"

#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#ifdef __APRS_C_DEBUG_FRAME
#include <libopencm3/stm32/gpio.h>
#endif

volatile static bool nextBitReady = true;	// Stores whether or not the next bit can be sent

/**********************************************************
 * aprs_transmitPacket
 *
 * Purpose: Convenience method
 *
 * Operation:
 *  1.) Construct ax.25 packet using the data provided using ax25_constructPacket
 *  2.) Send the packet out via aprs_sendPacket
 *
 */

void aprs_transmitPacket(uint8_t* dest, uint8_t destSSID, uint8_t* src, uint8_t srcSSID, uint8_t* info)
{
	uint8_t packet[300];	// TODO: find out maximum packet length
	int packetlen = ax25_constructPacket(packet, dest, destSSID, src, srcSSID, info);
	
	aprs_sendPacket(packet, packetlen);
}


/**********************************************************
 * aprs_sendpacket
 *
 * Purpose: Outputs a datapacket as tones of 1.2kHz and 2.2kHz
 *
 * Operation: 
 *  1.) Set up timers and interrupts for 1200 baud timing
 *  2.) Iterate through each byte of packet
 *  3.) Iterate through each bit of packet
 *  4.) Extract current bit from current byte
 *  5.) If the bit is 0, change the tone, otherwise don't
 *  6.) Wait until the current bit is transmitted.
 *  7.) Set the tone for the next bit
 *  8.) Check if 5 consecutive ones have been transmitted
 *  9.) If so, transmit an extra zero is transmitted next
 * 10.) Wait for the last bit to finish transmitting
 * 11.) Reset all timers and disable tone output
 *
 **********************************************************/
void aprs_sendPacket(uint8_t packet[], int packetLen)
{
	timer_reset(TIM2);				// Reset all settings
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);	// Enable clock to timer
	nvic_enable_irq(NVIC_TIM2_IRQ);			// Enable timer 3 IRQ
	timer_enable_irq(TIM2, TIM_DIER_UIE);		// Enable update interrupt
	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_period(TIM2, (F_CPU / 1200));		// Set timer to 1200 baud

	timer_enable_counter(TIM2);

	uint8_t tone = 0;	// 0 = 2200 Hz, 1 = 1200 Hz
	uint8_t oneCount = 0;	// Count the number of ones that occur for bit stuffing
	
	for(int bytei = 0; bytei < packetLen; bytei++)
	{
		for(int biti = 0; biti < 8; biti++)
		{
			uint8_t bit = (packet[bytei] >> biti) & 0x01;	// Extract bit to transmit
			// Bell 202 Modem frequencies
			tone ^= (~bit) & 0x01;	// Toggle tone if bit is 0, don't if bit is 1
			// Count the amount of consecutive ones
			if(bit)
				oneCount++;
			else
				oneCount = 0;

			// disable bit stuffing for flag bytes
			if(packet[bytei] == 0x7e)
				oneCount = 0;

			while(!nextBitReady);	// Wait until the next bit can be sent
			if(tone)		// If tone = 0, send 2.2 khz tone
			{
				dac_tone(1200);
			}
			else			// If tone = 0, send 1.2 kHz tone
			{
				dac_tone(2200);
			}
			nextBitReady = false;


			if(oneCount == 5)	// If we just transmitted the 5th one, transmit a 0
			{
				tone ^= 0x01;
				while(!nextBitReady);
				if(tone)
				{
					dac_tone(1200);
				}
				else
				{
					dac_tone(2200);
				}
				nextBitReady = false;
				oneCount = 0;
			}
		}
	}

	while(!nextBitReady);			// Wait for last bit to be transmitted
	
	dac_tone(0);				// Disable DAC output
	nvic_disable_irq(NVIC_TIM2_IRQ);	// Disable Timer 2 interrupt
	timer_disable_counter(TIM2);		// Disable counter
	rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);	// Disable clock to counter
}


/**********************************************************
 * tim2_isr(void)
 * 
 * Purpose: Set "nextBitReady" to true when next bit can be transmitted
 *
 **********************************************************/
void tim2_isr(void)
{
	timer_clear_flag(TIM2, TIM_SR_UIF);	// Clear Interrupt
	nextBitReady = true;	// Allow transmission of next bit
	#ifdef __APRS_C_DEBUG_FRAME
	gpio_toggle(GPIOC,GPIO8);
	#endif
}
