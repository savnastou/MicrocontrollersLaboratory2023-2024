    .include "m328PBdef.inc"

    .equ FOSC_MHZ=16        ;MHz
    .equ DEL1_mS=1500       ;mS
    .equ DEL2_mS=2000
    .equ F1=FOSC_MHZ*DEL1_mS
    .equ F2=FOSC_MHZ*DEL2_mS

reset:
    ldi r22,low(RAMEND)     ;stack pointer initialisation
    out SPL,r22
    ldi r22,high(RAMEND)
    out SPH,r22

main:
    ser r16
    out DDRD, r16           ;portD as output
    ldi r17,7               ;bit counter, indicates movement change left->right, right->left
    ldi r16,0x80            ;initial state MSB on (it's the rightmost LED)
    out PORTD,r16
    ldi r24, low(F1-1)      ;transition delay: 1.5 sec
    ldi r25, high(F1-1)
    rcall wait_x_msec

left:
    lsr r16                 ;rightmost LED is MSB so we shift right to move to lower bit, that is left
    out PORTD,r16
    ldi r24, low(F1-1)      ;transition delay: 1.5 sec
    ldi r25, high(F1-1)
    rcall wait_x_msec
    dec r17                 ;move to next bit, so decrease bit counter by 1
    brne left               ;when bit counter == 0, we reached the leftmost LED

    ldi r24, low(F2-1)      ;extra 2sec delay in leftmost LED
    ldi r25, high(F2-1)
    rcall wait_x_msec

    bclr 6                  ;for left movement  -> T=0
                            ;for right           -> T=1
                            ;we first have left movement
                            ;set the 6th bit of SREG (T flag)= 0
    ldi r17,7               ;re-initialise bit counter for right movement that follows

right:
    lsl r16                 ;leftmost LED is LSB so we shift left to move to higher bit, that is right
    out PORTD,r16
    ldi r24, low(F1-1)      ;transition delay: 1.5 sec
    ldi r25, high(F1-1)
    rcall wait_x_msec
    dec r17                 ;move to next bit, so decrease bit counter by 1
    brne right              ;when bit counter == 0, we reached the rightmost LED

    ldi r24, low(F2-1)      ;extra 2sec delay in rightmost LED
    ldi r25, high(F2-1)
    rcall wait_x_msec

    bset 6                  ;we changed direction so update T flag
    ldi r17,7               ;re-initialise bit counter for left movement that follows
    rjmp left               ;repeat left movement (and it goes on forever...)


wait_x_msec:                ;we use delay routine from "zhthmal_1.asm"
                            ;lasts 1000 cycles = 1 msec

delay_inner:
    ldi r23, 249            ; (1 cycle)
loop_inn:
    dec r23                 ; 1 cycle
    nop                     ; 1 cycle
    brne loop_inn           ; 1 or 2 cycles
                            ;1+ 248 * 4 + 3 = 249 * 4 = 996 cycles
    sbiw r24, 1             ; 2 cycles--998
    brne delay_inner        ; 1 or 2 cycles--1000
                            ;(F1-2)*(996 + 4) + 996 + 3
                            ;mexri edw 1000 kykloi epi F1-2 fores + 999 kykloi
    ldi r23,248             ;until now 1000 cycles times F1-1. We need another 1000 cycles

loop2:
    dec r23
    nop
    brne loop2              ;4*247+3=991

    nop
    nop
    ret                     ;4 cycles
                            ;total cycles after loop: 991+1+1+4=997
                            ;we need other 3 cycles to reach 1000 cycles
                            ;we get them from rcall
