.include "m328PBdef.inc"
.def DC_VALUE=r16
.def input=r17
.def temp=r18
.equ time2=16*1
.equ time3=16*100

    clr r26
    out DDRD,r26                            ;set PORTD as input (buttons)
    ldi r26,0b00111111
    out DDRB,r26                            ;set PORTB as input (buttons)

    ldi r26,(1<<WGM10)|(1<<COM1A1)
    sts TCCR1A,r26
    ldi r26,(1<<WGM12)|(1<<CS11)            ;CLK/8
    sts TCCR1B,r26                          ;Fast PWM, 8 bit

main:
    ldi ZL,LOW(2*DC_values)                 ;initialize Z pointer to array address (first element)
    ldi ZH,HIGH(2*DC_values)
    adiw r30,0x06                           ;go to array address with value 125
    lpm DC_VALUE,Z                          ;load value to DC_VALUE

loop2:
    in input,PIND
    ror input
    ror input                               ;check if PD1=0 (pushed)
    brcc debounc1                           ;if PD1 pushed jump to debouncing check for PD1
    ror input                               ;check if PD2=0 (pushed)
    brcc debounc2                           ;if PD2 pushed jump to debouncing check for PD2
    rjmp skip                               ;no button pushed

    PD1:
    cpi DC_VALUE,245                        ;if Duty Cycle 98% go to skip
    breq skip
    adiw r30,1
    lpm DC_VALUE,Z                          ;if not, increase Duty Cycle by 8%
    rjmp skip

    PD2:
    cpi DC_VALUE,5                          ;if Duty Cycle 2% go to skip
    breq skip
    sbiw r30,1                              ;if not, decrease Duty Cycle by 8%
    lpm DC_VALUE,Z

skip:
    sts OCR1AL,DC_VALUE                     ;ouput to PB1 LED
    rjmp loop2                              ;repeat infinitely

debounc1:
    ldi r24,low(time2)                      ;delay 5 msec
    ldi r25,high(time2)
    rcall wait_x_msec
    in r19,PIND
    ror r19
    ror r19                                 ;check if PD1 still pushed
    brcc debounc1                           ;if yes, stall
    rjmp PD1                                ;it's not pushed anymore so jump to according functionality

debounc2:
    ldi r24,low(time2)                      ;delay 5 msec
    ldi r25,high(time2)
    rcall wait_x_msec
    in r24,PIND
    ror r24
    ror r24
    ror r24                                 ;check if PD1 still pushed
    brcc debounc2                           ;if yes, stall
    rjmp PD2                                ;it's not pushed anymore so jump to according functionality

wait_x_msec:
                                            ;lasts 1000 cycles = 1 msec
                                            ;total group delay 996 cycles
delay_inner:
    ldi r23,249                             ; (1 cycle)
loop_inn:
    dec r23                                 ; 1 cycle
    nop                                     ; 1 cycle
    brne loop_inn                           ; 1 or 2 cycles

    sbiw r24,1                              ; 2 cycles
    brne delay_inner                        ; 1 or 2 cycles
    ret                                     ;4 cycles

DC_values: .db 5,25,45,65,85,105,125,145,165,185,205,225,245  ;each value matches to a percentage from 2% to 98%
