#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdlib.h>

int up_down = 0;
extern autotest(void);

void tim1_init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA -> MODER &= ~(3<<16);
  GPIOA -> MODER |= 2<<16;
  GPIOA -> MODER &= ~(3<<18);
  GPIOA -> MODER |= 2<<18;
  GPIOA -> MODER &= ~(3<<20);
  GPIOA -> MODER |= 2<<20;
  GPIOA->AFR[1] &= ~0xfff;
  GPIOA->AFR[1] |= 0x222;
  RCC->APB1ENR |= RCC_APB2ENR_TIM1EN;
  TIM1->CR1 &= ~TIM_CR1_DIR;
  TIM1->CR1 &= 0x39F;
  TIM1->PSC = 48 - 1;
  TIM1->ARR = 10000 - 1;
  TIM1->CCR1 = TIM1->CCR2 = TIM1->CCR3 = 9900;
  TIM1->BDTR |= TIM_BDTR_MOE;
  TIM1->CCMR1 &= ~TIM_CCMR1_OC1M_0;
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
  TIM1->CCMR1 &= ~TIM_CCMR1_OC2M_0;
  TIM1->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
  TIM1->CCMR2 &= ~TIM_CCMR2_OC3M_0;
  TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
  TIM1->CCER |= TIM_CCER_CC1E;
  TIM1->CCER |= TIM_CCER_CC2E;
  TIM1->CCER |= TIM_CCER_CC3E;
  TIM1->CCMR1 |= 0x800 | 0x8;
  TIM1->CCMR2 |= 0x800 | 0x8;
  TIM1->CR1 |= TIM_CR1_CEN;
  TIM1->DIER |= TIM_DIER_UIE;
}

void tim2_init() {
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2->CR1 &= 0x39F;
  TIM2->PSC = 480 - 1;
  TIM2->ARR = 1000 - 1;
  TIM2->CCR1 = TIM2->CCR2 = TIM2->CCR3 = 50;
  TIM2->DIER |= 0b1;
  NVIC->ISER[0] = 1 << TIM2_IRQn;
  TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler() {
  TIM2->SR &= ~TIM_SR_UIF;
  if (up_down == 1) {
      TIM1->CCR1 = (TIM1->CCR1 * 262) >> 8;
      TIM1->CCR2 = (TIM1->CCR2 * 262) >> 8;
      TIM1->CCR3 = (TIM1->CCR3 * 262) >> 8;
      if (TIM1->CCR1 >= 9900 | TIM1->CCR2 >= 9900 | TIM1->CCR3 >= 9900)
        up_down = -1;
  }
  else if (up_down == -1) {
        TIM1->CCR1 = (TIM1->CCR1 * 251) >> 8;
        TIM1->CCR2 = (TIM1->CCR2 * 251) >> 8;
        TIM1->CCR3 = (TIM1->CCR3 * 251) >> 8;
        if (TIM1->CCR1 <= 600 | TIM1->CCR2 <= 600 | TIM1->CCR3 <= 600)
          up_down = 1;
    }
}

int main(void) {
  autotest();
  tim1_init();
  tim2_init();
  while(1) asm("wfi");
  return 0;
}