;Enable the INT0 interrupt (PD2)
ldi r24,(1<<INT0)
out EIMSK,r24

sei                     ;Sets the Global Interrupt Flag of SREG

loop1:
    clr r26
loop2:
    out PORTC,r26

    ldi r24,low(DEL_NU1)
    ldi r25,high(DEL_NU1)
    rcall delay_ms

    inc r26

    cpi r26,32
    breq loop1
    rjmp loop2

ISR0:
    push r25    ;regs 24-25 will be used to count debouncing time,
    push r24    ;so we push them in the stack
    push r26

debouncing_check:
    ldi r24,(1<<INTF0)
    out EIFR,r24
    ldi r24,low(DEL_NU2)
    ldi r25,high(DEL_NU2)
    rcall delay_ms
    in r24,EIFR
    cpi r24,0x01
    breq debouncing_check

    ldi r26,0x05    ;buttons left to check
    clr count       ;count of pressed buttons
    in r24,PINB     ;input from buttons B
loop:
    dec r26         ;button has been checked->lower button counter
    ror r24
    brcs npress     ;if CY=1 (button not pressed) skip increase of count
    inc count       ;else increase count of pressed buttons
npress:
    cpi r26,0x00    ;if end of buttons jump to finish
    breq finish
    rjmp loop       ;else jump to loop to move to the next button

finish:
    out PORTC,count
    ldi r24,low(DEL_NU1)
    ldi r25,high(DEL_NU1)
    rcall delay_ms

    pop r26
    pop r24
    pop r25

    reti

delay_ms:
    ldi r23,249
loop_inn:
    dec r23
    nop
    brne loop_inn
    sbiw r24,1
    brne delay_ms
    ret
