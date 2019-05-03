.cpu cortex-m0
.thumb
.syntax unified
.fpu softvfp

// External interrupt for pins 0 and 1 is IRQ 5.
.equ EXTI0_1_IRQn,5

// SYSCFG constrol registers
.equ SYSCFG, 0x40010000
.equ EXTICR1, 0x8
.equ EXTICR2, 0xc
.equ EXTICR3, 0x10
.equ EXTICR4, 0x14

// External interrupt control registers
.equ EXTI, 0x40010400
.equ IMR, 0
.equ EMR, 0x4
.equ RTSR, 0x8
.equ FTSR, 0xc
.equ SWIER, 0x10
.equ PR, 0x14

// Variables to register things for pin 0
.equ EXTI_RTSR_TR0, 1
.equ EXTI_IMR_MR0, 1
.equ EXTI_PR_PR0, 1

// NVIC control registers...
.equ NVIC, 0xe000e000
.equ ISER, 0x100
.equ ICER, 0x180
.equ ISPR, 0x200
.equ ICPR, 0x280

// SysTick counter variables...
.equ SYST, 0xe000e000
.equ CSR, 0x10
.equ RVR, 0x14
.equ CVR, 0x18

// GPIO Variables
.equ  RCC,      0x40021000
.equ  AHBENR,   0x14
.equ  GPIOCEN,  0x00080000
.equ  GPIOBEN,  0x00040000
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  MODER,    0x00
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  PC0,      0x01
.equ  PC1,      0x04
.equ  PC2,      0x10
.equ  PC3,      0x40
.equ  PIN8,     0x00000100

//=======================================================
// Your translation of
// unsigned int fibonacci(unsigned int n) {
//   if (n < 2)
//      return n;
//   return fibonacci(n-1) + fibonacci(n-2);
// }
//
.global fibonacci
fibonacci:
    push {r4, r5, lr}
    cmp r0, #2
    bge recur15
    adds r0, #0
    pop {r4, r5, pc}

recur15:
    subs r4, r0, #1
    movs r0, r4
    bl fibonacci
    movs r5, r0
    subs r0, r4, #1
    bl fibonacci
    adds r5, r0
    movs r0, r5
    pop {r4, r5, pc}

//=======================================================
// Your implementation of a SysTick interrupt handler.
// This is an interrupt service routine.
// Increment the value of the global variable tick_counter
// Display that value with a call to display_digit().
//
.type SysTick_Handler, %function
.global SysTick_Handler
SysTick_Handler:
	push {lr}
	ldr r0, =tick_count
	ldr r1, [r0, #0]
	adds r1, #1
	str r1, [r0, #0]
	mov r0, r1
	//bl display_digit
	pop {pc}

//=======================================================
// Initialize the SysTick counter.
// You should set the SYST_RVR (reset value register)
// so an exception occurs once per 100 milliseconds.
//
// Then set the SYST_CSR (control status register) so
// that it uses the CPU clock as the clock source, enable
// the SysTick exception request, and enable the counter.
//
.global init_systick
init_systick:
    push {lr}
	ldr r3, =SYST
	ldr r2, =4800000
	str r2, [r3, #RVR] // reset
	movs r2, #7
	str r2, [r3, #CSR] // Enable
    pop {pc} // Student should remove this.

//=======================================================
// OR the value EXTI_RTSR_TR0 into the EXTI_RTSR
// (rising trigger selection register).
// This will tell the EXTI system to flag an interrupt
// on the rising edge of Pin 0.
//
.global init_rtsr
init_rtsr:
	ldr r1,=EXTI
	ldr r2,[r1,#RTSR]
	ldr r3,=EXTI_RTSR_TR0
	orrs r3,r2
	str r3,[r1,#RTSR]
	mov pc, lr
//=======================================================
// OR the value EXTI_IMR_MR0 into EXTI_IMR
// (Interrupt mask register).
// This will unmask the external interrupt for Pin 0.
//
.global init_imr
init_imr:
	push {lr}
	ldr r1,=EXTI
	ldr r2,[r1,#IMR]
	ldr r3,=EXTI_IMR_MR0
	orrs r3,r2
	str r3,[r1,#IMR]
	pop {pc}
//=======================================================
// Write (1 << EXTI0_1_IRQn) into the NVIC_ISER
// (Interrupt set enable register).
// (This value is '1' shifted left by EXTI0_1_IRQn bits.)
// This action will enable the external interrupt for pins 0 and 1.
//
.global init_iser
init_iser:
	push {lr}
	movs r0,#1
	ldr r1,=EXTI0_1_IRQn
	lsls r0,r1
	ldr r3,=NVIC
	ldr r4,=ISER
	str r0,[r3,r4]
	pop {pc}
//=======================================================
// The interrupt handler for Pins 0 and 1.
// The handler should increment the global variable named
// 'button_presses' and call display_digit with that value.
// Then it should write EXTI_PR_PR0 to the EXTI_PR register to
// clear the interrupt.
//
// Optionally, you may also call micro_wait() for a
// while to debounce the button press.
//
.type EXTI0_1_IRQHandler, %function
.global EXTI0_1_IRQHandler
EXTI0_1_IRQHandler:
	push {lr}
	movs r3, #0
	ldr r0, =SYST
	str r3, [r0, #CSR]
	ldr r2,=EXTI_PR_PR0
	ldr r0,=EXTI
	str r2,[r0,#PR]
	pop {pc}

//=======================================================
// "bit bang" the value of data bit by bit from LSB to
// MSB into PC2, while also generating a bit banged serial
// clock. Bit banging is embedded speak for toggling the
// output depending on the current bit. As an example
// consider 0b1001 0010, bit banging this value will set
// PC2 output to the following values logic 0, logic 1,
// logic 0, logic 0, logic 1, logic 0, logic 0, logic 1.
// This is bit banging from LSB to MSB.
.global send_data
send_data:
	push {r4, r5, r6, r7, lr}
	movs r7, r0
	movs r6, #0

for_loop:
	cmp r6, #16
	beq done_for
	lsrs r7, #1
	bcs if_block
	b else_block

if_block:
	ldr r0,=GPIOC
	movs r1, #2
	bl setpin
	b loop_back

else_block:
	ldr r0,=GPIOC
	movs r1, #2
	bl clrpin
	b loop_back

loop_back:
	ldr r0,=GPIOC
	movs r1, #0
	bl setpin
	movs r0, #200
	bl nano_wait
	ldr r0,=GPIOC
	movs r1, #0
	bl clrpin
	movs r0, #200
	bl nano_wait
	adds r6, #1
	b for_loop

done_for:
	pop {r4, r5, r6, r7, pc}

//===========================================================
// Set the state of a single output pin to be high.
// Do not affect the other bits of the port.
// Parameter 1 is the GPIOx base address.
// Parameter 2 is the bit number of the pin.
// No expected return value.
.global setpin
setpin:
    push    {r3, lr}
    /* Student code goes here */
	ldr r2, [r0, #ODR]
	movs r3, #1
	lsls r3, r1
	orrs r2, r3
	str r2, [r0, #ODR]
    /* End of student code*/
    pop     {r3, pc}

//===========================================================
// Set the state of a single output pin to be low.
// Do not affect the other bits of the port.
// Parameter 1 is the GPIOx base address.
// Parameter 2 is the bit number of the pin.
// No expected return value.
.global clrpin
clrpin:
    push    {lr}
    /* Student code goes here */
	ldr r2, [r0, #ODR]
	movs r3, #1
	lsls r3, r1
	ldr r4, =0xFFFF
	eors r3, r4
	ands r2, r3
	str r2, [r0, #ODR]
    /* End of student code*/
    pop     {pc}