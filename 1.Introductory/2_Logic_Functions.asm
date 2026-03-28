    .include "m328PBdef.inc"
    .DEF A=r16          ;renaming registers
    .DEF B=r17
    .DEF C=r18
    .DEF D=r19
    .DEF F0=r20
    .DEF F1=r21
    .DEF temp=r22       ;defining temporary variable

start:
    ldi A,0x45          ;initialising variables
    ldi B,0x23
    ldi C,0x21
    ldi D,0x01
    ldi r23,5           ;initialising iterations counter

main:
    com A               ;A'
    com B               ;B'
    com C               ;C'

    mov temp,A
    and temp,B          ;A'*B'
    and temp,C          ;A'*B'*C'
    or temp,D           ;A'*B'*C'+D

    com temp            ;(A'*B'*C'+D)'=F0

    mov F0,temp

    ;gia thn F1
    com C               ;C
    com D               ;D'
    mov temp, A
    or temp,C           ;A'+C
    mov F1, temp
    mov temp, B
    or temp,D           ;B'+D'
    and F1,temp         ;F1

    com A               ;A
    com B               ;B
    com D               ;D

    inc A               ;increasing each variable by the stated constant
    ldi temp,2
    add B,temp
    ldi temp,4
    add C,temp
    ldi temp,5
    add D,temp

    dec r23             ;decreasing iterations counter by 1
    brne main           ;if iterations counter not zero, go to the next iteration
    rjmp start          ;the program repeats from the beginning (initial state of variables)
