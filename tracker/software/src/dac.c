#include "dac.h"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/dac.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


#define LUTVALUES 128	// Must be power of 2
/* 12-bit Sinewave lookup table
 * 512 entries 
 */
const uint16_t sineLut[LUTVALUES] = {
2000,2098,2196,2293,2390,2486,2581,2674,
2765,2855,2943,3028,3111,3191,3269,3343,
3414,3482,3546,3606,3663,3715,3764,3808,
3848,3883,3914,3940,3962,3978,3990,3998,
4000,3998,3990,3978,3962,3940,3914,3883,
3848,3808,3764,3715,3663,3606,3546,3482,
3414,3343,3269,3191,3111,3028,2943,2855,
2765,2674,2581,2486,2390,2293,2196,2098,
2000,1902,1804,1707,1610,1514,1419,1326,
1235,1145,1057,972,889,809,731,657,
586,518,454,394,337,285,236,192,
152,117,86,60,38,22,10,2,
0,2,10,22,38,60,86,117,
152,192,236,285,337,394,454,518,
586,657,731,809,889,972,1057,1145,
1235,1326,1419,1514,1610,1707,1804,1902
};

static int lutIndex = 0;	// current index of LUT

void dac_setup(void)
{
	// DAC_OUT1 pin: PA4
	rcc_periph_clock_enable(RCC_GPIOA);	// Enable PortA
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM6EN);	// Enable clock to Timer 6 on APB1
	rcc_periph_clock_enable(RCC_TIM6);

	gpio_mode_setup(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO4);	// Set PA4 to analog mode
	
	rcc_periph_clock_enable(RCC_DAC);	// Enable clock to DAC
	dac_trigger_enable(CHANNEL_1);	// Enable triggering on channel 1
	dac_set_trigger_source(DAC_CR_TSEL1_SW);	// Setup DAC to be triggered by software
	
	timer_set_period(TIM6, 0x7FF);	// Let timer run to full value
	
	nvic_enable_irq(NVIC_TIM6_DAC_IRQ);	// Enable Timer 6 interrupt
	timer_enable_irq(TIM6, TIM_DIER_UIE);
	//timer_reset(TIM6);	// Reset Timer to default settings
	// Note: On any other timer you'd have to call timer_set_mode() to set it up, but this timer has
	// no changeable features, so it's unnecessary here
}

void dac_tone(int hz)
{
	if(hz == 0)
	{
		// Disable DAC and TIM6
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
