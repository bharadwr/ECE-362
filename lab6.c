#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "wavetable.h"
#include <stdio.h>
#include <stdlib.h>
#define SAMPLES 128

void micro_wait(unsigned int micro_seconds);

int s1 = 0;
int s2 = 0;
int waveCount = 0;
float a1 = 1;
float a2 = 1;
int circ_buf[SAMPLES] = {0};
uint32_t start = 0, end = 0;
			
void insert_circ_buf(int val) {
    if(start <= end && end <= (SAMPLES-1)) {
        circ_buf[end++] = val;
    } else if(start < end && end > (SAMPLES-1)) {
        circ_buf[0] = val;
        end = 0;
        start = end + 1;
    } else if(end < start && start < (SAMPLES-1)) {
        start++;
        circ_buf[end++] = val;
    } else {
        start = 0;
        circ_buf[end++] = val;
    }
}

float get_time_period(int min, int max) {
    int start_interval = 0;
    int sample_count = 0;
    int avg_samples = 0;
    int no_cycles = 0;

    for(int i = 0; i < SAMPLES; i++) {


        if(circ_buf[i] < (0.3*max) && start_interval == 1) {
            start_interval = 0;
            avg_samples += sample_count;
            sample_count = 0;
            no_cycles++;
        }

        if(circ_buf[i] > (0.8*max) && start_interval == 0) {
            start_interval = 1;

        }


        if(start_interval == 1)
            sample_count++;
    }

    int avg_interval = avg_samples / no_cycles;
    return (2 * avg_interval * 200e-6);
}

// This function
// 1) enables clock to port A,
// 2) sets PA0, PA1, PA2 and PA4 to analog mode
void setup_gpio() {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= 0x33F; //hardcoding the values, fight me
}

// This function should enable the clock to the
// onboard DAC, enable trigger,
// setup software trigger and finally enable the DAC.
void setup_dac() {
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; //Enable clock to DAC
	DAC->CR &= ~DAC_CR_EN1; //Disable DAC Channel 1
	DAC->CR &= ~DAC_CR_BOFF1; //Do not turn buffer off
	DAC->CR |= DAC_CR_TEN1; //enable trigger
	DAC->CR |= DAC_CR_TSEL1; //all ones, select software trig
	DAC->CR |= DAC_CR_EN1; //enable dac channel 1
}

// This function should,
// enable clock to timer2,
// setup prescalaer and arr so that the interrupt is triggered 100us,
// enable the timer 2 interrupt and start the timer.
void setup_timer2() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 60 - 1;
	TIM2->ARR = 80 - 1;
	TIM2->CR1 |= TIM_CR1_CEN;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC->ISER[0] = 1 << TIM2_IRQn;
}

// This function should, enable clock to timer3,
// setup prescalaer and arr so that the interrupt is triggered 200us,
// enable the timer 3 interrupt and start the timer.
void setup_timer3() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = 96 - 1;
	TIM3->ARR = 100 - 1;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
	NVIC->ISER[0] = 1 << TIM3_IRQn;
}

// This function should enable the clock to ADC,
// turn on the clocks, wait for ADC to be ready.
void setup_adc() {
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
}

// This function should return the value from the ADC
// conversion of the channel specified by the “channel” variable.
// Make sure to set the right bit in channel selection
// register and do not forget to start adc conversion.
int read_adc_channel(unsigned int channel) {
	ADC1->CHSELR = 0;
	ADC1->CHSELR |= 1 << channel;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));
	ADC1->CR |= ADC_CR_ADSTART;
	while(!(ADC1->ISR & ADC_ISR_EOC));
	return ADC1->DR;
}

//The interrupt handler should read the value from the ADC’s channel 2 input.
// Use the insert_circ_buffer() function to insert the read value into the circular buffer.
void TIM3_IRQHandler() {
	TIM3->SR &= ~TIM_SR_UIF;
	int adc_value = read_adc_channel(2);
	insert_circ_buf(adc_value);
}

// TIM2_IRQHandler: The interrupt handler should start the DAC conversion using the software trigger,
// and should use the wavetable.h to read from the array and write it into the DAC.
// Every time the interrupt is called you will read a new element from the “wavetable” array.
// So you might need to use a global variable as an index to the array.
// Note that the array has 100 elements, make sure you do not read wavetable[100].
void TIM2_IRQHandler() {
	//if (waveCount == 100)
		//waveCount = 0;
	
	TIM2->SR &= ~TIM_SR_UIF;
	while((DAC->SWTRIGR & DAC_SWTRIGR_SWTRIG1));
	//DAC->DHR12R1 = wavetable[waveCount];
	s1 = (s1 + 1) % 100;
	s2 = (s2 + 2) % 100;
	DAC->DHR12R1 = (int)(a1 * wavetable[s1] + a2 * wavetable[s2]) >> 1;
	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
//	waveCount++

}

int main(void)
{
	serial_init();
    setup_gpio();
    setup_adc();
    setup_dac();
    setup_timer2();
    setup_timer3();

    float freq = 0;
    int i = 0;
    int max = -2147483648, min = 2147483648;
    while(1) {
    	serial_init();
    	/*
    	Loop through the circular buffer, to calculate min and max.
    	• Call get_time_period(min, max) to estimate the period of the sampled signal.
    	• Calculate the frequency.
    	• Use printf to display, “a1”, “a2” and “frequency” in the serial terminal
    	*/
    	a1 = (read_adc_channel(0) / 4095.0);
    	a2 = (read_adc_channel(1) / 4095.0);
        micro_wait(500000);

        max = circ_buf[0], min = circ_buf[0];
        for (i = 1; i < SAMPLES; i++) {
        	if (circ_buf[i] > max) max = circ_buf[i];
        	if (circ_buf[i] < min) min = circ_buf[i];
        }
        freq = 1/get_time_period(min, max);
        printf("\na1 = %.5f   a2 = %.5f   freq = %.1f\n", a1, a2, freq);
    }
}