.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

.global main
main:

//******************************************************************************
// Code Segment 1:
//
// Description:
//    For this section you will write an assembly program to calculate
//    the length of a string. Remember that a string ends with '\0'
//    whose value is 0. The first character of the string is stored at
//    the address 'str' (i.e. label). You will need to read each
//    character one byte at a time (Hint: use ldrb) and compare it with 0.
//    If it is not zero, increment the length of the string. Store the
//    result in r0. Type in your code under codeSegment1. You will most
//    likely use a while loop for this program whose structure would resemble:
//
//    loop1: Check for string termination
//           Branch out of loop if done
//           Perform action
//           Branch back to loop1
//    done1:
//
// Useful Instructions:
//    beq, cmp, adds, ldrb, movs, ldr, b
//
//*****************************************************************************

codeSegment1:
	// Student code goes here
    ldr r0, =str
    movs r2, #0

loop1:
    ldrb r1, [r0, r2]
    cmp r1, #0
    beq done1
    adds r2, #1
    b loop1

done1:
    bkpt
	// End of student code

.data
str: .string "test string"

//*****************************************************************************
// Code Segment 2:
//
// Description:
//    For this section your will read the word at 'bitPattern' and
//    identify the location of the most significant bit. For example if
//    value at bitPattern is 0x00010002 the most bit is at bit sixteen,
//    so the result of this should be 16 Store the result in r0. Type in
//    your code under codeSegment2.
//
//    You will most likely use a while loop for this program whose structure
//    would resemble:
//
//    loop2: Check for loop termination
//           Branch out of loop if done
//           Perform action
//           Branch back to loop2
//    done2:
//
// Useful Instructions:
//    bcs, lsls, subs, adds, movs, ldr, b
//
//*****************************************************************************

.text
codeSegment2:
	// Student code goes here
    ldr r0, =bitPattern
    ldr r1, [r0]
    movs r2, #31

loop2:
    lsls r1, r1, #1
    bcs done2
    subs r2, #1
    b loop2

done2:
    bkpt
	// End of student code

.data
.align 4
bitPattern: .word 0x00A01001

//*****************************************************************************
// Code Segment 3:
//
// Description:
//   For this section you will write a simple program to read the elements
//   of an array from 'src', of size given by 'arrSize', and copy
//   that to 'dest' in reverse order. The last element of 'src'
//   array with be the first in 'dest' and first in 'src' will be
//   the last element in 'dest'.  The 'arrSize' stores the size
//   of the 'src' array in bytes. Type your program under codeSegment3.
//   Note use ldrb to copy elements.
//
//   Hint: For this program you will need to maintain two counters, one for
//   source and the other for destination, in your loop you will increment one
//   and decrement the other while copying the content from src to dest.
//
// Useful Instructions:
//   ldr, movs, subs, cmp, ldrb, strb, adds, b, beq
//
//*****************************************************************************
.text
codeSegment3:
    // Student code goes here
    ldr r0, =arrSize
    ldr r1, [r0] //arrSize
    ldr r6, =dest
    ldr r3, [r6] //dest
    ldr r0, =src
    ldr r2, [r0]
    movs r4, #0

loop3:
    cmp r4, #13
    beq done3
    ldrb r2, [r0, r4] //src
    strb r2, [r6, r1] //copy to dest
    adds r4, #1
    subs r1, #1
    b loop3

done3:
    bkpt
    // End of student code

.data
.align 4
arrSize: .word 13
src:     .word 0xDEADBEEF
         .word 0xABADCAFE
         .word 0xBAADF00D
         .word 0xCAFED00D
         .word 0xDEADC0DE
.align 4
dest:    .space 13

//*****************************************************************************
// Code Segment 4:
//
// Description:
//    Read a byte from 'char' assume that the stored value is
//    alphabetic, and check if the character is uppercase or lowercase
//    Hint: ASCII values of lowercase alphabets are greater than 0x60.
//    If the character is lowercase, set the value at label 'lower'
//    to 0xFF else set it to 0x00. Type your code under codeSegment4.
//
// Useful Instructions:
//    ldr, movs, bge, strb, b
//
//*****************************************************************************

.text
codeSegment4:
    // Student code goes here
	ldr r0, =char
	ldr r1, [r0] //var
	cmp r1, #96
	bge check4
	movs r1, #0
	strb r1, [r0]
	b done4

check4:
	movs r1, #255
	ldr r0, =lower
	strb r1, [r0]
    // End of student code

done4:
	bkpt

.data
.align 4
char:   .string "B"
.align 4
lower:  .space 1


//*****************************************************************************
// Code Segment 5:
//
// Description:
//     Given an array of whose length is stored at "len", iterate through the
//     array at "arr", to find the maximum value in the array, assume all
//     the values are unsigned 32 bit integers. Store the maximum value at
//     "max". Caution, "len" is the number of bytes and NOT the number of
//     elements in the array.
//
// Useful Instructions:
//     ldr, movs, bge, bgt, b, adds, cmp
//
//*****************************************************************************

.text
codeSegment5:
    // Student code goes here
	ldr r0, =len
	ldr r1, [r0] //len
	ldr r0, =arr
	movs r2, #0 //max variable
	movs r3, #0 //offset
	movs r4, #0 //counter

loop5:
	cmp r4, r1
	beq done5
	ldrb r3, [r0, r4] //arr
	cmp r3, r2
	bge alter
	adds r4, #4
	b loop5

alter:
	movs r2, r3
	adds r4, #4	b loop5

done5:
	ldr r0, =max
	strb r2, [r0]
	bkpt
	// End of student code

.data
.align 4
len:     .word 20
arr:     .word 0x01
         .word 0x06
         .word 0x03
         .word 0x04
         .word 0x09
.align 4
max:    .space 4

//*****************************************************************************
//
//    Infinite loop
//
//*****************************************************************************
.text
inf_loop:
    nop
    b inf_loop
// We never reach this point.