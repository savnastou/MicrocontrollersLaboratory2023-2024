    .include "m328PBdef.inc"

    .equ FOSC_MHZ=16        ;processor speed (in MHz)
    .equ DEL_mS=20          ;mS
    .equ F1=FOSC_MHZ*DEL_mS

main:
    ldi r24, low(F1-1)      ;r24 & r25 contain the delay minus 1 cycle
    ldi r25, high(F1-1)
    ser r22
    out DDRD,r22            ;PORTD as output
    out PORTD,r22           ;leds on
    rcall wait_x_msec       ;delay to make the change in leds visible
    clr r22                 ;
    out PORTD,r22           ;leds off
    ldi r24, low(F1-1)      ;initial value at r24,r25 after first rcall
    ldi r25, high(F1-1)     ;
    rcall wait_x_msec       ;delay to make the change in leds visible
    rjmp main               ;


wait_x_msec:
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
