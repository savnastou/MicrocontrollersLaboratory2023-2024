;2.2 A)
.include "m328PBdef.inc"
.equ FOCS_MHZ=16
.equ DEL_ms=1000
.equ DEL_NU=FOCS_MHZ*DEL_ms

    ldi r24,low(RAMEND)  ;initialise stack pointer
    out SPL,r24
    ldi r24,high(RAMEND)
    out SPH,r24

    ser r26
    out DDRC,r26

loop1:
    clr r26
loop2:
    out PORTC,r26
    ldi r24,low(DEL_NU)
    ldi r25,high(DEL_NU)
    rcall delay_ms
    inc r26
    cpi r26,32
    breq loop1
    rjmp loop2

delay_ms:
    ldi r23,249
loop_inn:
    dec r23
    nop
    brne loop_inn
    sbiw r24,1
    brne delay_ms
    ret
