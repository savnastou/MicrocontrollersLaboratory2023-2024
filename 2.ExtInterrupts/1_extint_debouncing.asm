.include "m328PBdef.inc"

.equ DEL_mS3=5
.equ FOSC_MHZ=16
.equ F1=FOSC_MHZ*DEL_mS3
.equ DEL_mS=500
.equ DEL_NU=FOSC_MHZ*DEL_mS
.DEF int_count=r22

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1

reset:
    ldi r21,LOW(RAMEND)
    out SPL,r21
    ldi r21,HIGH(RAMEND)
    out SPH,r21

    ldi r20,(1<<ISC11)|(1<<ISC10)  ;interrupt on rising edge of INT1 pin
    sts EICRA,r20

    ldi r20,(1<<INT1)              ;enable int1
    out EIMSK,r20

    sei

    clr int_count                  ;initialise interrupt count
    ser r26                        ;initalise output
    out DDRB,r26                   ;set PORTB as output--LEDs for counting
    out DDRC,r26                   ;set PORTC as output--LEDs for interrupt
    clr r26
    out PORTC,r26
    out DDRD,r26                   ;set PORTD as input (buttons)

loop1:
    clr r26

loop2:
    out PORTB,r26                  ;output count and wait:
    ldi r24,low(DEL_NU)
    ldi r25,high(DEL_NU)           ;regs 24-25 contain delay
    rcall delay_mS
    inc r26                        ;increase count
    cpi r26,16
    breq loop1                     ;if r26=16 jump to start else to loop2
    rjmp loop2

ISR1:

debouncing_check:
    ldi r24,(1<<INTF1)
    out EIFR,R24
    ldi r24,low(F1-1)              ;check every 5ms
    ldi r25,high(F1-1)
    rcall delay_mS
    in r24,EIFR
    ror r24
    ror r24
    brcs debouncing_check

                                   ;first check button:
    in r18,PIND
    rol r18                        ;rotate left +carry
    rol r18
    brcc button_pushed             ;if CY=0(button pushed) jump to button_pushed
    inc int_count                  ;else increase count

    cpi int_count,32               ;check if interrupt count=32
    brne skip_reset                ;if not, jump to skip_reset
    clr int_count                  ;reset counter
skip_reset:
    out PORTC,int_count            ;leds PC4-PC0
button_pushed:
    sei
    pop r25
    pop r24
    reti


delay_mS:
                                   ;total group delay 996 cycles
delay_inner:
    ldi     r23,249                ; (1 cycle)
loop_inn:
    dec r23                        ; 1 cycle
    nop                            ; 1 cycle
    brne loop_inn                  ; 1 or 2 cycles

    sbiw r24,1                     ; 2 cycles
    brne delay_inner               ; 1 or 2 cycles
    ret                            ;4 cycles
