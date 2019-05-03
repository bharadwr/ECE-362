#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdint.h>
#include <stdio.h>

// These are function pointers.  They can be called like functions
// after you set them to point to other functions.
// e.g.  cmd = bitbang_cmd;
// They will be set by the stepX() subroutines to point to the new
// subroutines you write below.
void (*cmd)(char b) = 0;
void (*data)(char b) = 0;
void (*display1)(const char *) = 0;
void (*display2)(const char *) = 0;

// Prototypes for subroutines in support.c
void generic_lcd_startup(void);
void clock(void);
void step1(void);
void step2(void);
void step3(void);
void step4(void);
void step6(void);

// This array will be used with dma_display1() and dma_display2() to mix
// commands that set the cursor location at zero and 64 with characters.
//
uint16_t dispmem[34] = {
        0x080 + 0,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x080 + 64,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
        0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220, 0x220,
};

//=========================================================================
// Subroutines for step 2.
//=========================================================================
void spi_cmd(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = b;
}

void spi_data(char b) {
    while((SPI2->SR & SPI_SR_TXE) != SPI_SR_TXE);
    SPI2->DR = 0x200 | b;
}

void spi_init_lcd(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= 0x8A000000;
	RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN;

	SPI2 -> CR1 |=SPI_CR1_MSTR | SPI_CR1_BR;
	SPI2 -> CR1 |= SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE;
	SPI2 -> CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | SPI_CR2_DS_3 | SPI_CR2_DS_0;
	SPI2 -> CR1 |= SPI_CR1_SPE;

	nano_wait(100000000);
	cmd(0x38);
	cmd(0x0c);
	cmd(0x01);
	nano_wait(6200000);
	cmd(0x02);
	cmd(0x06);
}

//===========================================================================
// Subroutines for step 3.
//===========================================================================

// Display a string on line 1 using DMA.
void dma_display1(const char *s) {
	cmd(0x80); // put the cursor on the beginning of the first line.
	int x;
	for(x=0; x<16; x++){
	   if (s[x]){
		   dispmem[x+1] = s[x] | 0x200;
	   }
	   else{
	     break;
	   }
	}
	for( ; x<16; x++)
	   dispmem[x+1] = 0x220;
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel5 -> CCR &= ~DMA_CCR_EN;
	DMA1_Channel5 -> CMAR = (uint32_t)(dispmem);
	DMA1_Channel5 -> CPAR = (uint32_t)&(SPI2->DR);
	DMA1_Channel5 -> CNDTR = sizeof dispmem/2;
	DMA1_Channel5 -> CCR |= DMA_CCR_DIR;
	DMA1_Channel5 -> CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0;
	DMA1_Channel5 -> CCR &= ~DMA_CCR_PL;
	DMA1_Channel5-> CCR |= DMA_CCR_EN;

	SPI2->CR2 |= SPI_CR2_TXDMAEN;
}
//===========================================================================
// Subroutines for Step 4.
//===========================================================================

void dma_spi_init_lcd(void) {
	spi_init_lcd();
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel5 -> CCR &= ~DMA_CCR_EN;
	DMA1_Channel5 -> CMAR = (uint32_t)(dispmem);
	DMA1_Channel5 -> CPAR = (uint32_t)&(SPI2->DR);
	DMA1_Channel5 -> CNDTR = sizeof dispmem/sizeof dispmem[0];//34
	DMA1_Channel5 -> CCR |= DMA_CCR_DIR;
	DMA1_Channel5 -> CCR |= DMA_CCR_MINC | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0| DMA_CCR_CIRC;
	DMA1_Channel5 -> CCR &= ~DMA_CCR_PL;
	DMA1_Channel5 -> CCR |= DMA_CCR_EN;
	SPI2->CR2 |= SPI_CR2_TXDMAEN;
}

// Display a string on line 1 by copying a string into the
// memory region circularly moved into the display by DMA.
void circdma_display1(const char *s) {
	int x;
	for(x = 0; x < 16; x++){
		if (s[x]) {
			dispmem[x+1] = s[x] | 0x200;
		}
		else{
			break;
		}
	}
	for(; x < 16 ; x++)
		dispmem[x+1] = 0x220;
}

//===========================================================================
// Display a string on line 2 by copying a string into the
// memory region circularly moved into the display by DMA.
void circdma_display2(const char *s) {
	int x;
	for(x = 0; x < 16; x++){
		if (s[x] != '\0'){
			dispmem[x+18] = s[x] | 0x200;
		}
		else{
			break;
		}
	}
	for(; x < 16 ; x++)
		dispmem[x+18] = 0x220;
}

//===========================================================================
// Subroutines for Step 6.
//===========================================================================


void init_tim2(void) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->CR1 &= ~TIM_CR1_DIR;
	TIM2->PSC = 4800 - 1;
	TIM2->ARR = 1000 - 1;
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC1M_2) ;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;// | TIM_CCMR1_OC1PE;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC ->ISER[0] = 1 << TIM2_IRQn;
}

void TIM2_IRQHandler(void) {
	TIM2->SR = ~TIM_SR_UIF;
	clock();
}

int main(void)
{
    //step1();
    //step2();
    //step3();
    //step4();
     step6();
}