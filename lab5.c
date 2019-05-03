#include "stm32f0xx.h"
#include "stm32f0_discovery.h"

void micro_wait(int);

void prob1(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~(3<<18);
  GPIOC->MODER |= 0x40000;
  for(;;) {
    GPIOC->ODR = 0x200; // Turn on only PC8 (Blue LED)
    micro_wait(1000000);
    GPIOC->ODR = 0; // Turn off only PC8 (Blue LED)
    micro_wait(1000000);
  }
}

void prob2(void) {
  // Enable Port C
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~(3<<18);
  GPIOC->MODER |=  2<<18;
  // Set the alternate function for PC8
  // PC0-7 are on AFR[0], 8-15 are on AFR[1]
  GPIOC->AFR[1] &= ~0xf;
  GPIOC->AFR[1] |= 1;
  // Enable the system clock for timer 3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  // Counting direction: 0=up, 1=down
  TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
  // Set prescaler output to 1kHz (8MHz/8000)
  TIM3->PSC = 48000 - 1;
  // Auto-reload 1000
  TIM3->ARR = 1000 - 1;
  // Any value between 0 and 1000.
  TIM3->CCR4 = 500;
  // Channel 3 of the timer is configured in CCMR2.
  // Set the bits to select toggle mode (011)
  TIM3->CCMR2 &= ~TIM_CCMR2_OC4M_2; // Turn off bit 2.
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;
  // Enable output for channel 3 active-high output
  TIM3->CCER |= TIM_CCER_CC4E;
  // Enable timer 3
  TIM3->CR1 |= TIM_CR1_CEN;
  while(1)
    asm("wfi");
}

void prob3(void) {
  // Enable Port C
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~(3<<18 | 3<<16 | 3<<14 | 3<<12);
  GPIOC->MODER |=  2<<18;
  GPIOC->MODER |=  2<<16;
  // Set the alternate function for PC8
  // PC0-7 are on AFR[0], 8-15 are on AFR[1]
  GPIOC->AFR[1] &= ~0xf;
  GPIOC->AFR[1] |= 1;
  // Enable the system clock for timer 3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  // Counting direction: 0=up, 1=down
  TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
  // Set prescaler output to 1kHz (8MHz/8000)
  TIM3->PSC = 8000 - 1;
  // Auto-reload 1000
  TIM3->ARR = 1000 - 1;
  // Any value between 0 and 1000.
  // Channel 3 of the timer is configured in CCMR2.
  // Set the bits to select toggle mode (011)
  TIM3->CCMR2 &= ~(TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC3M_2); // Turn off bit 2.
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
  // Enable output for channel 3 active-high output
  TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E;
  // Enable timer 3
  TIM3->CR1 |= TIM_CR1_CEN;
  while(1)
    asm("wfi");
}

void prob4(void) {
  // Enable Port C
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~(3<<18 | 3<<16 | 3<<14 | 3<<12);
  GPIOC->MODER |=  2<<18;
  GPIOC->MODER |=  2<<16;
  GPIOC->MODER |=  2<<14;
  GPIOC->MODER |=  2<<12;
  // Set the alternate function for PC8
  // PC0-7 are on AFR[0], 8-15 are on AFR[1]
  GPIOC->AFR[1] &= ~0xf;
  GPIOC->AFR[1] |= 1;
  GPIOC->AFR[0] &= ~0xf;
  GPIOC->AFR[0] |= 1;
  // Enable the system clock for timer 3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  // Counting direction: 0=up, 1=down
  TIM3->CR1 &= ~TIM_CR1_DIR; // clear it to count up
  TIM3->CCR1 = 1;
  TIM3->CCR2 = 250;
  TIM3->CCR3 = 500;
  TIM3->CCR4 = 750;
  // Set prescaler output to 1kHz (8MHz/8000)
  TIM3->PSC = 48000 - 1;
  // Auto-reload 1000
  TIM3->ARR = 1000 - 1;
  // Any value between 0 and 1000.
  // Channel 3 of the timer is configured in CCMR2.
  // Set the bits to select toggle mode (011)
  TIM3->CCMR2 &= ~(TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC3M_2); // Turn off bit 2.
  TIM3->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
  TIM3->CCMR1 &= ~(TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2M_2); // Turn off bit 2.
  TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_0;
  // Enable output for channel 3 active-high output
  TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E | TIM_CCER_CC1E | TIM_CCER_CC2E;
  // Enable timer 3
  TIM3->CR1 |= TIM_CR1_CEN;
  while(1)
    asm("wfi");
}

void display(int x) {
    x &= 0xf; // Only look at the low 4 bits.


    //    _____
    //   |  A  |
    //  F|     |B
    //   |_____|
    //   |  G  |
    //  E|     |C
    //   |_____|
    //      D
    static const char output[16] = {
            //GFEDCBA
            0b1000000, // 0: _FEDCBA 1000000
            0b1111001, // 1: ____CB_ 1111001
            0b0100100, // 2: G_ED_BA
            0b0110000, // 3: G__DCBA
            0b0011001, // 4: GF__CB_
            0b0010010, // 5: GF_DC_A
            0b0000010, // 6: GFEDC_A
            0b1111000, // 7: ____CBA
            0b0000000, // 8: GFEDCBA
            0b0011000, // 9: GF__CBA
            0b0001000, // a: GFE_CBA
            0b0000011, // b: GFEDC__
            0b1000110, // c: _FED__A
            0b0100001, // d: G_EDCB_
            0b0000110, // e: GFED__A
            0b0001110, // f: GFE___A
    };
    GPIOC->ODR = output[x];

    // You need to look up the value of output[x] here
    // and then somehow assign it to the ODR.
}

void init_display(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  // Setting PC0-7 as output
  GPIOC->MODER &= ~0xffff;
  GPIOC->MODER |=  0x5555;
  // Writing 1 to last 7 bits
  GPIOC->ODR |= 0xFF;
}

void test_display(void) {
    init_display();
    // Enable Port C

    int x;
    for(;;)
        for(x=0; x<16; x++) {
            display(x);
            micro_wait(500000);
        }
}

int count = 0;
void increment(void) {
    count += 1;
    display(count);
}

void TIM2_IRQHandler() {
  increment();
  // Reading CCRx clears CCxIF int. flag.
  int discard __attribute__((unused));
  discard = TIM2->CCR1;
  discard = TIM2->CCR2;
}

void prob6(void) {
  init_display();
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
  GPIOC -> MODER &= ~(0b11111111000000000000);
  GPIOC -> MODER |= 0b1010101000000000000;
  GPIOA -> MODER &= ~(3);
  GPIOA -> MODER |= 2;
  GPIOA -> AFR[0] &= ~15;
  GPIOA -> AFR[0] |= 2;

  display(0);

  RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2 -> PSC = 1 - 1;
  TIM2 -> ARR = 0xffffffff;

  TIM2 -> CCMR1 &= ~TIM_CCMR1_CC1S;
  TIM2 -> CCMR1 |= TIM_CCMR1_CC1S_0;
  TIM2 -> CCMR1 &= ~TIM_CCMR1_IC1F;
  TIM2 -> CCMR1 &= ~TIM_CCMR1_IC1PSC;

  TIM2 -> CCER &= ~(TIM_CCER_CC1P|TIM_CCER_CC1NP);
  TIM2 -> CCER |= TIM_CCER_CC1E;
  TIM2 -> DIER |= TIM_DIER_CC1IE;
  TIM2 -> CR1 |= TIM_CR1_CEN;
  NVIC -> ISER[0] = 1<<TIM2_IRQn;

  for(;;){
    asm("wfi");
  }
}

void prob7(void) {
  init_display();
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
  GPIOC -> MODER &= ~(255<<12);
  GPIOC -> MODER |= 85<<12;

  GPIOA -> MODER &= ~(3<<(2*1));
  GPIOA -> MODER |= 2<<(2*1);
  GPIOA -> AFR[0] &= ~(0xf<<(4*1));
  GPIOA -> AFR[0] |= 0x2<<(4*1);

  GPIOA -> PUPDR &= ~((0x3)<<2);
  GPIOA -> PUPDR |= (0x2) <<2;

  display(0);

  RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
  TIM2 -> PSC = 1 - 1;
  TIM2 -> ARR = 0xffffffff;

  TIM2 -> CCMR1 &= ~TIM_CCMR1_CC2S;
  TIM2 -> CCMR1 |= TIM_CCMR1_CC2S_0;
  TIM2 -> CCMR1 &= ~TIM_CCMR1_IC2F;
  TIM2 -> CCMR1 &= ~TIM_CCMR1_IC2PSC;

  TIM2 -> CCMR1 |= TIM_CCMR1_IC2F_3 | TIM_CCMR1_IC2F_2 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_IC2F_0;

  TIM2 -> CCER &= ~(TIM_CCER_CC2P|TIM_CCER_CC2NP);
  TIM2 -> CCER |= TIM_CCER_CC2E;
  TIM2 -> DIER |= TIM_DIER_CC2IE;
  TIM2 -> CR1 |= TIM_CR1_CEN;

  TIM2->CR1 &= ~TIM_CR1_CKD;
  TIM2->CR1 |= TIM_CR1_CKD_1;

  NVIC -> ISER[0] = 1<<TIM2_IRQn;

  for(;;){
    asm("wfi");
  }
}

void TIM3_IRQHandler() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->MODER &= ~(3<<14);
  GPIOC->MODER |=  1<<14;

  GPIOC->ODR ^= 1<<7;

  // Reading CCRx clears CCxIF int. flag.
  int discard __attribute__((unused));
  discard = TIM_SR_UIF;
}

void prob8(void) {
  RCC  -> APB1ENR |= RCC_APB1ENR_TIM3EN;
  TIM3 -> PSC = 48000 - 1;
  TIM3 -> ARR = 500 - 1;
  TIM3 -> DIER |= TIM_DIER_UIE;
  // Enable tim3
  TIM3 -> CR1 |= TIM_CR1_CEN;
  NVIC -> ISER[0] = 1 << TIM3_IRQn;
  prob7();
}

int main(void) {
    //prob1();
    //prob2();
	//prob3();
    //prob4();
    //test_display();
    //prob6();
    //prob7();
    prob8();
}