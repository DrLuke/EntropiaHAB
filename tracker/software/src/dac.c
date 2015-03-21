#include "dac.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


#define LUTVALUES 64	// Must be power of 2
/* 12-bit Sinewave lookup table
 * 512 entries 
 */
const uint16_t sineLut[LUTVALUES] = {
2000,2196,2390,2581,2765,2943,3111,3269,
3414,3546,3663,3764,3848,3914,3962,3990,
4000,3990,3962,3914,3848,3764,3663,3546,
3414,3269,3111,2943,2765,2581,2390,2196,
2000,1804,1610,1419,1235,1057,889,731,
586,454,337,236,152,86,38,10,
0,10,38,86,152,236,337,454,
586,731,889,1057,1235,1419,1610,1804
};

volatile static int lutIndex = 0;	// current index of LUT

void dac_setup(void)
{
	// DAC_OUT1 pin: PA4
	rcc_periph_clock_enable(RCC_GPIOA);		// Enable PortA
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM6EN);	// Enable clock to Timer 6 on APB1
	rcc_periph_clock_enable(RCC_TIM6);

	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);	// Set PA4 to analog mode
	
	rcc_periph_clock_enable(RCC_DAC);		// Enable clock to DAC
	dac_trigger_enable(CHANNEL_1);			// Enable triggering on channel 1
	dac_set_trigger_source(DAC_CR_TSEL1_SW);	// Setup DAC to be triggered by software
	
	timer_set_period(TIM6, 0x7FF);			// Let timer run to full value
	
	nvic_enable_irq(NVIC_TIM6_DAC_IRQ);		// Enable Timer 6 interrupt
	timer_enable_irq(TIM6, TIM_DIER_UIE);
	// Note: On any other timer you'd have to call timer_set_mode() to set it up, but this timer has
	// no changeable features, so it's unnecessary here
}

void dac_tone(int hz)
{
	if(hz == 0)
	{
		// Disable DAC and TIM6
		lutIndex = 0;	// Start with phase 0 when resuming operation after shutdown
		dac_load_data_buffer_single(0, RIGHT12, CHANNEL_1);	// Clean shutoff
		dac_software_trigger(CHANNEL_1);	
		dac_disable(CHANNEL_1);
		timer_disable_counter(TIM6);	// Disable timer
	}
	else
	{
		// Set up timer to proper frequency
		// The prescaler must be set to trigger once for every sample of sinewave
		int period = F_CPU / (hz * LUTVALUES);
		timer_set_period(TIM6, period);
		if(timer_get_counter(TIM6) >= period)
		{
			timer_set_counter(TIM6, 0);	// Reset Counter
		}
		timer_enable_counter(TIM6);
		dac_enable(CHANNEL_1);
	}
}


void tim6_dac_isr(void)
{
	timer_clear_flag(TIM6, TIM_SR_UIF);
	if(++lutIndex > (LUTVALUES-1))
	{
		lutIndex = 0;
	}

	dac_load_data_buffer_single(sineLut[lutIndex]+50, RIGHT12, CHANNEL_1);	// Load new data into DAC
	dac_software_trigger(CHANNEL_1);	// Trigger conversion from data register to analog value
}
