// hw910.c
//
// All of these exercises should be completed using the Standard Peripheral
// firmware.  Use only direct I/O register manipulation to implement these
// subroutines.  (Do not use Standard Peripheral macros.)  You should also
// assume that the system clock frequency, and the input to all timers, is
// 48 MHz.
//

// This homework requires you to look up hardware configuration information
// in the Family Reference Manual as well as the STM32F051R8T6 datasheet, to
// which you will have access during the lab practical exam.  Various lecture
// materials can guide you to the appropriate tables in the documentation.
// Remember that you will not have access to the lecture notes (or Piazza)
// during your lab practical exam.  For any problem where you need to
// consult the lecture notes, try to find examples you need in the appropriate
// section of Appendix A of the Family Reference Manual.

// This homework will be tested automatically.  At some point in the future,
// a test module will give you information only on what works and what doesn't.
// If you connect a CFAL1602 OLED LCD to SPI2 channel as you did for Lab 8,
// it will report information when the autotest() subroutine is invoked.

// Unless explicitly stated, you should not assume that any subroutine you
// write is invoked before another one.  You should also not assume that
// I/O register values are in their default (reset) state.  For instance, an
// I/O register field that is expected to be 00 on reset, may be set to 11
// or 10 or 01.  It is your responsibility to clear such a field before
// ORing in a new value.

// Finally, neither this nor any other homework exercise is not meant to be
// a collaborative effort with other students.  Do it on your own.  This will
// be important not only for your preparation for the practical, but to
// ensure you are not cited for copying your solution from other students.

#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdint.h>
#include <math.h>

int16_t wavetable[256];
int offset;
int counter;

// Problem 1:
// Fill in the function to initialize GPIO Port C so that:
//  - PC9  is an output, using medium speed, push-pull drivers
//  - PC10 is an output, using high speed, open-drain drivers
//  - PC11 is an input, with a pull-up resistor
//  - PC12 is an input, with a pull-down resistor
// Do not change the configuration of any other pin on any other port.
void problem1(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	GPIOC->MODER &= ~GPIO_MODER_MODER9;
	GPIOC->MODER &= ~GPIO_MODER_MODER10;
	GPIOC->MODER |= GPIO_MODER_MODER9_0; //output
	GPIOC->MODER |= GPIO_MODER_MODER10_0; //output

	GPIOC->MODER &= ~GPIO_MODER_MODER11; //input
	GPIOC->MODER &= ~GPIO_MODER_MODER12; //input

	GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0; //medium speed
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10; //high speed

	GPIOC->OTYPER &= GPIO_OTYPER_OT_9; //push pull
	GPIOC->OTYPER |= GPIO_OTYPER_OT_10; //open drain

	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR11;
	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR12;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR11_0; //pull up res
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR12_1; //pull down res
}

// Problem 2:
// Fill in the function to initialize GPIO Port C so that:
//  - PC8 is an output, using high speed, push-pull drivers, using the
//    alternate function that connects it to Timer 3, channel 3
//  - PC9 is an output, using high speed, push-pull drivers, using the
//    alternate function that connects it to Timer 4, channel 4
void problem2(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER &= ~GPIO_MODER_MODER8;
	GPIOC->MODER |= GPIO_MODER_MODER8_1; //set to af mode 01
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_8; //push pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8; //high speed
	GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH0; //AF0 = 0000 TIM3CH3

	GPIOC->MODER &= ~GPIO_MODER_MODER9;
	GPIOC->MODER |= GPIO_MODER_MODER9_1; //set to af mode
	GPIOC->OTYPER &= ~GPIO_OTYPER_OT_9; //push pull
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9; //high speed
	GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH1; //AF0 = 0000, TIM4CH4
}

// Problem 3:
// Fill in the function to initialize Timer 3 so that:
//  - The counter is up-counting.
//  - The counter goes from 0 to 99.
//  - The counter will be reset to zero 300 times per second.
//  - Channels 3 and 4 are set for PWM mode 1.
//  - Preload enable is set for CCR3 and CCR4.
//  - Outputs for channel 3 and 4 are enabled.
//  - The CCR3 register is set to 12.
//  - The CCR4 register is set to 84.
//  - Enable the timer.
//  (Change no other configuration registers for any other functionality.)
void problem3(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 &= ~TIM_CR1_DIR; //count up
	TIM3->ARR = 100 - 1;
	TIM3->PSC = 1600 - 1; // 300 = 48e6 / (ARR * PSC)
	TIM3->CCMR2 &= ~TIM_CCMR2_OC3M;
	TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; //110 for PWM Mode 1
	TIM3->CCMR2 &= ~TIM_CCMR2_OC4M;
	TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; //110 for PWM Mode 1
	TIM3->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE; //enable preload
	TIM3->CCER |= TIM_CCER_CC3E; //enable channel 3
	TIM3->CCER |= TIM_CCER_CC4E; //enable channel 4
	TIM3->CCR3 = 12;
	TIM3->CCR4 = 84;
	TIM3->CR1 |= TIM_CR1_CEN;
}

// Problem 4:
// Fill in the function below so that it uses floating-point arithmetic
// to initialize the wavetable array (at the top of the file) so that
// it approximates a sine wave with the following formula:
//
// for(x=0; x < sizeof wavetable / sizeof wavetable[0]; x += 1)
//     wavetable[x] = 32767 * sin(x * 2 * M_PI / 256);
//
// This should form a sinusoid with an amplitude of 32767 that would
// repeat after 256 steps.  Its maximum value will be 32767, and its
// minimum value will be -32767.
void problem4(void) {
	int x;
	for(x=0; x < sizeof wavetable / sizeof wavetable[0]; x++)
		wavetable[x] = 32767 * sin(x * 2 * M_PI / 256);
}

// Problem 5:
// Fill in the function to initialize the DAC so that:
//  - its output is routed to its output pin.
//  - it is triggered by software.
//  - the trigger is enabled.
//  - it is enabled.
// You should be able to "listen to" the DAC by plugging in a speaker.
void problem5(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //enable sysclock to gpio
	GPIOA->MODER |= GPIO_MODER_MODER4; //setting dac output pin to analog mode
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; //enable sysclock to dac
	DAC->CR |= DAC_CR_TEN1 | DAC_CR_TSEL1 | DAC_CR_EN1;  //enable trig
}

// Problem 6:
// Fill in the function below to output the value of
// (wavetable[offset] + 32768)>>4  to the DAC.  You can assume the wavetable
// array contains values that were initialized by problem4().
// The addition will shift each entry in the wavetable to the
// positive range of values in the range 1 ... 65535.
// The ">>4" will divide each value by 16 to produce values
// in the range 0 ... 4095.
// Then:
//  - Write the converted result to DHR12R1.
//  - Software trigger the DAC.
void problem6(void) {
	DAC->DHR12R1 = (wavetable[offset] + 32768) >> 4;
	DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1; //software trigger the dac
}

// Problem 7:
// Write the interrupt service routine for Timer 2 below.
// You should be able to determine the name for it.
// The ISR should do the following:
//  - Acknowledge the source of the interrupt.  (See Problem 8.)
//  - If the 'offset' variable is greater than or equal to the
//    number of elements in wavetable, set it to zero.
//  - Invoke problem6().
//  - Increment the 'offset' variable.
// Remember that you can find the names of ISRs in startup/startup_stm32f0xx.S.
void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM_SR_UIF;
	if (offset >= 256)
		offset = 0;
	problem6();
	offset++; //takeoff is better
}

// Problem 8:
// Fill in the function below to initialize Timer 2 so that:
//  - The counter is set back to zero 64000 times per second.
//  - The ARR register is 1.
//  - The timer generates a interrupt on each update event.
//  - Enable the timer.
//  - Ensure that interrupt is enabled.
//  (Change no other configuration registers for any other functionality.)
// Remember that the value you assign to the NVIC ISER is
//   1 << interrupt_name_IRQn
//
// If you run
//    problem4(); problem5(); problem6(); problem8();
// you should hear a 250 Hz sine wave on the DAC output.
void problem8(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->ARR = 2 - 1;
	TIM2->PSC = 375 - 1;
	TIM2->DIER |= TIM_DIER_UIE; //enable interrupt on update event
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC->ISER[0] = 1 << TIM2_IRQn;
}

// Problem 9:
// Fill in the function below so that it uses floating-point arithmetic
// to initialize the wavetable array (at the top of the file) so that
// it approximates a sine wave with the following formula:
//
// for(x=0; x < sizeof wavetable / sizeof wavetable[0]; x += 1)
//     wavetable[x] = (32767 * sin(x * 2 * M_PI / 256) + 32768) / 16;
//
// This should form a sinusoid with an amplitude of 2047 that is offset
// by 2048.  Its maximum value will be 4095, and its minimum value will be 0.
void problem9(void) {
	int x;
	for(x=0; x < sizeof wavetable / sizeof wavetable[0]; x++)
		wavetable[x] = (32767 * sin(x * 2 * M_PI / 256) + 32768) / 16;
}

// Problem 10:
// Fill in the function below to set up a DMA channel such that:
//  - The peripheral output is the DAC DHR12R1 register.
//  - The input is the memory region for wavetable.
//  - The count is the number of elements in wavetable.
//  - Ensure it is copying from memory to the peripheral.
//  - Circular transfers are enabled.
//  - You must determine the values for the MSIZE and PSIZE fields.
//  - The DMA channel is triggered by the timer 15 update event.
// You must determine which DMA channel to use!  (Which DMA channel
// can be triggered by the Timer 15 update event?)
void problem10(void) {
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	DMA1_Channel5->CMAR = (uint32_t)(wavetable); //source wavetable
	DMA1_Channel5->CPAR = (uint32_t)&(DAC->DHR12R1); //dest DAC DHR12R1 address
	DMA1_Channel5->CNDTR = sizeof wavetable/sizeof wavetable[0];
	DMA1_Channel5->CCR |= DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_CIRC | DMA_CCR_EN;
	NVIC->ISER[0] = 1 << TIM15_IRQn;
}

// Problem 11:
// Fill in the function below to set up timer 15 so that:
//  - It issues an update event at a rate of 160 kHz.
//  - It triggers a DMA channel on an update event.
//  - Set the MMS field so that the update event is selected for TRGO.
//  - Set the auto-reload preload enable flag.
//  - Enable the timer.
// FURTHERMORE:
//  - Reconfigure the DAC so it is triggered by Timer 15 TRGO.
//  - Enable DMA in the DAC CR.
//  - Turn off Timer 2.
//
// If you run
//    problem5(); problem6(); problem9(); problem10(); problem11();
// you should hear a 625 Hz tone on the DAC output.
void problem11(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR = RCC_APB2ENR_TIM15EN;
	TIM15->PSC = 30 - 1;
	TIM15->ARR = 10 - 1;
	TIM15->DIER |= TIM_DIER_UDE; //trigger dma channel on update
	TIM15->CR2 &= ~TIM_CR2_MMS;
	TIM15->CR2 |= TIM_CR2_MMS_1; //MMS:010 for update event
	TIM15->CR1 |= TIM_CR1_ARPE | TIM_CR1_CEN; //auto-reload preload enable flag
	DAC->CR |= DAC_CR_TSEL1;
	DAC->CR &= ~DAC_CR_TSEL1_2; //011 TIM15 TRGO
	DAC->CR |= DAC_CR_DMAEN1; //Enable DMA in DAC CR
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

// Problem 12:
// Write an interrupt service routine to be invoked by the DMA channel
// you used for Problem 10.
// It should do the following:
//  - Acknowledge the source of the interrupt.  (See Problem 13.)
//  - Increment the 'counter' variable.
//  - If counter is greater than or equal to 2:
//    - set counter to 0
//    - if the rate of timer 15 is 160 kHz, set it to 320 kHz
//    - else set the timer 15 rate to 160 kHz
// Remember that you can find names of ISRs in startup/startup_stm32f0xx.S.
//
// This should cause the DAC to output two cycles of a sinusoid at one
// frequency, and then two cycles of the same sinusoid at twice the
// frequency.
void DMA1_Channel4_5_IRQHandler() {
	DMA1->IFCR |= DMA_IFCR_CGIF5;
	counter++;
	if (counter >= 2) {
		counter = 0;
		if ((48000000 / ((TIM15->PSC + 1) * (TIM15->ARR + 1))) == 160000)
			TIM15->ARR = 5 - 1;
		else
			TIM15->ARR = 10 - 1;
	}
}

// Problem13:
// Fill in the function below to:
//  - Enable the interrupt for the ISR named in Problem 12.
//  - Set the DMA channel used in Problem 10 to invoke an interrupt
//    each time there is a transfer complete event for the DMA.
// (Remember that the value you should assign to the NVIC ISER is
//  1 << interrupt_name_IRQn )
//
// If you run
//   problem5(); problem6(); problem9(); problem10(); problem11(); problem13();
// you should hear a strange tone and see shifting frequencies on a scope.
void problem13(void) {
	DMA1_Channel5->CCR |= DMA_CCR_TCIE | DMA_CCR_EN; //transfer complete enable
	NVIC->ISER[0] = 1 << DMA1_Channel4_5_IRQn;
}

// Problem 14:
// Fill in the function below to initialize the wavetable array such that:
//  - wavetable[0] is 5
//  - wavetable[1] is 5
//  - wavetable[2] is 5
//  - wavetable[3] is 5
//  - wavetable[4] is 5
//  - wavetable[5] is 95
void problem14(void) {
	wavetable[0] = 5;
	wavetable[1] = 5;
	wavetable[2] = 5;
	wavetable[3] = 5;
	wavetable[4] = 5;
	wavetable[5] = 95;
}

// Problem 15:
// Fill in the function below to initialize a DMA channel so that:
//  - The peripheral output is the CCR register for timer 3, channel 3.
//  - The input is the memory region for wavetable.
//  - The count 6.
//  - Ensure it is copying from memory to the peripheral.
//  - You must determine the values for the MSIZE and PSIZE fields.
//  - Circular copying is enabled.
//  - The DMA channel is triggered by the timer 3 channel 3 event.
// You must determine which DMA channel to use!
void problem15(void) {
	RCC->AHBENR |= RCC_AHBENR_DMA1EN; //enable clock
	DMA1_Channel2->CCR &= ~DMA_CCR_EN; //Disable DMA
	DMA1_Channel2->CNDTR |= 6; //count = 6
	DMA1_Channel2->CMAR |= (uint32_t)wavetable; //From wavetable
	DMA1_Channel2->CPAR |= (uint32_t)(&(TIM3->CCR3)); //to tim3 channel3
	DMA1_Channel2->CCR |= DMA_CCR_DIR | DMA_CCR_CIRC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_EN; //mem to periph
}

// Problem 16:
// Fill in the function below to update timer 3 so that:
//  - It triggers a DMA channel on a channel 3 event.
// You should assume Timer 3 is already running as it was in problem 3.
//
// If you run
//     problem2(); problem3(); problem14(); problem15(); problem16();
// You should see a distinctive variable duty-cycle square wave output on PC8.
void problem16(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->DIER |= TIM_DIER_UDE | TIM_DIER_CC3DE; //update dma enable
	TIM3->CR2 &= ~TIM_CR2_MMS;
	TIM3->CR2 |= TIM_CR2_MMS_1; //MMS: 010 For update mode
	TIM3->CR1 |= TIM_CR1_CEN;
}

// Problem 17:
// Fill in the function below to set up the ADC so that:
//  - It uses the high-speed internal clock.
//  - It can read analog values from PC0.
void problem17(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER |= GPIO_MODER_MODER0; //Enable PC0 in Analog mode
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; //enable adc
	RCC->CR2 |= RCC_CR2_HSI14ON; //High speed shit
	ADC1->CHSELR |= ADC_CHSELR_CHSEL10; //select channel 10 for PC0
}

// Problem 18:
// Fill in the function below to set up SPI channel 1 so that:
//  - It is first disabled.
//  - Its pins are set to use PA4 through PA7.
//  - The baud rate is 6mbit/sec.
//  - Clock polarity is "1 when idle".
//  - The second clock transition is the first data capture edge.
//  - It is set for Master mode.
//  - It has bidirectional data mode enabled.
//  - Output mode is enabled for bidirectional mode.
//  - Software slave management is enabled.
//  - The internal slave select is currently disabled.
//  - Frames are sent LSB first.
//  - It has a data word size of 9 bits.
//  - It is enabled once all other fields are set.
void problem18(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //GPIO Clock
	//GPIOA->MODER |= ~(3<<8|3<<10|3<<12|3<<14);
	GPIOA->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5
			| GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOA->MODER |= GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1
			| GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
	GPIOA->AFR[0] |= ~(GPIO_AFRL_AFRL4 | GPIO_AFRL_AFRL5 | GPIO_AFRL_AFRL6
			| GPIO_AFRL_AFRL7); //Set to AF0
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //SPI channel 1 Clock
	SPI1->CR1 &= ~SPI_CR1_SPE; //SPI disabled
	SPI1->CR1 &= ~(SPI_CR1_BR_2 | SPI_CR1_BR_0);//Baud rate, master mode
	SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_BR_1;   //Software slave mgmt
	SPI1->CR2 |= SPI_CR2_SSOE | SPI_CR2_DS_3; //Slave select
	SPI1->CR1 &= ~SPI_CR1_SSI; //Internal Slave select disabled
	SPI1->CR2 &= ~(SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0);
}

// Problem 19:
// Fill in the function below to set up I2C2
//  - It uses PB10 and PB11 for external I/O.
//  - It uses the 8MHz internal "HSI" clock
//  - The analog noise filter is turned on.
//  - No interrupts are enabled.
//  - Clock stretching is disabled.
//  - It uses the recommended timing register values for 400 kHz operation.
//  - The "own address 1" and "own address 2" are both disabled.
//  - It uses 7-bit addresses.
//  - Auto end is enabled.
//  - NACK is set.
void problem19(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN; //GPIO clock
	GPIOB->MODER |= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11); //Cleared
	GPIOB->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1; //Set
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH2 | GPIO_AFRH_AFRH3); //clear AFRH
	GPIOB->AFR[1] |= (1<<8 | 1<<12); //set AFR to 0001 for AFR10 and 11
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN; //I2C Clock
	RCC->CR |= RCC_CR_HSION; //HSI clock
	I2C2->CR1 &= ~(I2C_CR1_PE | I2C_CR1_ANFOFF); //Analog noise filter on
	I2C2->CR1 |= I2C_CR1_NOSTRETCH; //Clock stretch disabled
	I2C2->TIMINGR &= ~I2C_TIMINGR_PRESC;  //PRESC=0 //0000 0000 0011 0001 0000 0011 0000 1001
	I2C2->TIMINGR |= 9 | 3<<8 | 1<<16 | 3<<20; //SCLL=0x9
	I2C2->OAR1 &= ~I2C_OAR1_OA1; //Own address 1 disabled
	I2C2->OAR2 &= ~I2C_OAR2_OA2; //Own address 2 disabled
	I2C2->CR2 |= I2C_CR2_SADD; //7 bit mode
	I2C2->CR2 &= ~I2C_CR2_ADD10; //7 bit mode
	I2C2->CR2 |= I2C_CR2_AUTOEND | I2C_CR2_NACK; //Nack enabled
	I2C2->CR1 |= I2C_CR1_PE; //enable peripheral
}

// Problem 20:
// Fill in the function below to set up USART2 so that:
//  - It uses PA2 and PA3 for TxD and RxD.
//  - It is first disabled.
//  - It uses 16x oversampling.
//  - The baud rate is 48000.
//  - It uses an 8 bits word.
//  - It uses 1 stop bit.
//  - It uses odd parity.
//  - It is enabled.
void problem20(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //GPIO clock enabled
	GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3); // Cleared
	GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1; // Set to AF
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL2 | GPIO_AFRL_AFRL3); // Cleared
	GPIOA->AFR[0] |= 1 << 8 | 1 << 12; // Set PA2 and 3
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // USART Clock enabled
	USART2->CR1 &= USART_CR1_UE & ~(USART_CR1_M | USART_CR1_OVER8); // disable usart
	USART2->CR2 &= ~(USART_CR2_STOP_1 | USART_CR2_STOP_0); //1 stop bit
	USART2->CR1 |= USART_CR1_PCE | USART_CR1_PS; //Odd parity
	USART2->BRR = 48000000/48000; //Baud rate clock default rate 48MHz / desired rate
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}
// Just look at all the things you learned how to do in this class.