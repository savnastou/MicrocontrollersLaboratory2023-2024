.include "m328PBdef.inc"

.def output=r17     ;helper output variable
.def flag=r16       ;flag variable
;.equ time1=16*10
.equ time2=16*1

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1
;reti

reset:
    ldi r28,low(RAMEND)  ;initialise stack pointer
    out SPL,r28
    ldi r28,high(RAMEND)
    out SPH,r28

    ser r26              ;set PORTB as output
    out DDRB,r26

    clr r26
    out DDRD,r26         ;set PORTD as input

    ;Interrupt on rising edge of INT1 pin
    ldi r28,(1<<ISC11)|(1<<ISC10)
    sts EICRA,r28

    ;Enable the INT1 interrupt (PD3)
    ldi r28,(1<<INT1)
    out EIMSK,r28

    ;Initialise flag, output, timer to zero
    clr flag
    clr output
    clr r24
    clr r25
    clr r28
    clr r29

main:
    sei                     ;Sets the Global Interrupt Flag of SREG
    mov r20,r28             ;check if timer is zero
    or r20,r29              ;if r20=0, timer is zero
    breq leds_off           ;if timer is zero, leds_off

    sbiw r28,1              ;timer is above zero so decrease it by 1

    cpi r28,low(2500)
    brne end                ;if 8 lower bits of timer equal to 2.5 sec, check for high else skip
    cpi r29,high(2500)
    brne end                ;if not equal jump skip
    ldi output,0x01         ;for the remaining secs, only LED0 is on
    rjmp end

leds_off:
    clr output              ;set helper variable to zero
    clr flag                ;set flag to zero again (in case PC returns from an interrupt)

end:
    out PORTB,output        ;ouput helper variable to LEDs
    ldi r24,low(time2)      ;delay 1 msec
    ldi r25,high(time2)
    rcall wait_x_msec
    rjmp main

ISR1:

debouncing_check:
                            ;spinthrismos
    ;ldi r28,(1<<INTF1)
    ;out EIFR,r28
    ;
    ;ldi r24,low(time1)
    ;ldi r25,high(time1)
    ;rcall wait_x_msec
    ;in r28,EIFR
    ;cpi r28,0x02
    ;breq debouncing_check

    ldi r28,low(3000)       ;reset timer to 3 sec
    ldi r29,high(3000)

    cpi flag,1              ;did this interrupt occur when another interrupt was served?
    brne zero               ;if no, jump to zero

    ser output              ;if yes, turn all LEDs on
    rjmp end2

zero:
    ldi output,0x01         ;turn only LED0 on
end2:
    ldi flag,1              ;PC is in an interrupt routine so flag <- 1
    sei                     ;interrupts can be received while PC is in interrupt routine
    reti

wait_x_msec:
                            ;lasts 1000 cycles = 1 msec
                            ;total group delay 996 cycles
delay_inner:
    ldi r23,249             ; (1 cycle)
loop_inn:
    dec r23                 ; 1 cycle
    nop                     ; 1 cycle
    brne loop_inn           ; 1 or 2 cycles

    sbiw r24,1              ; 2 cycles
    brne delay_inner        ; 1 or 2 cycles
    ret                     ;4 cycles
