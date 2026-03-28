.include "m328PBdef.inc"

.equ FOSC_MHZ = 16          ; Microcontroller operating frequency in MHz

.EQU PD3 = 3
.EQU PD2 = 2
.DEF mon=r16
.DEF dek=r17
.DEF ek=r21
.DEF temp2=r18
.DEF temp=r22
.DEF argL=r24               ; used as argument
.DEF argH=r25               ; used as argument

.def ADC_L = r19
.def ADC_H = r20

.org 0x0
rjmp reset

.org 0x2A                   ;ADC Conversion Complete Interrupt
rjmp ISR_ADC

reset:
    ldi temp, LOW(RAMEND)   ;Initialize stack pointer
    out SPL, temp
    ldi temp, HIGH(RAMEND)
    out SPH, temp

    ser temp
    out DDRD, temp          ; init PORTD (connected to lcd) as output
    out DDRB, temp          ; set PORTB as output

    clr temp
    out DDRC, temp          ;Set PORTC as input

    ; REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0010 => select ADC2(pin PC2),
    ; ADLAR=1 => Left adjust the ADC result
    ldi temp, 0b01100010
    sts ADMUX, temp

    ; ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    ; ADIE=1 => enable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ldi temp, 0b10001111
    sts ADCSRA, temp
    sei                     ; enable global interrupts

    rcall lcd_init          ; init lcd display
    ldi argL, low(2*FOSC_MHZ)
    ldi argH, high(2*FOSC_MHZ)     ; wait 2 msec
    rcall wait_msec

main:
    sei
    lds temp, ADCSRA
    ori temp, (1<<ADSC)     ; Set ADSC flag of ADCSRA
    sts ADCSRA, temp        ; in order to start conversion

    ldi r24, low(100*FOSC_MHZ)     ; wait 1 sec
    ldi r25, high(100*FOSC_MHZ)
    rcall wait_msec

    rcall lcd_clear_display
    mov r24,mon
    rcall lcd_data          ; send one byte of the integer part to the lcd's controller

    ldi r24, 0x2E           ; display dot
    rcall lcd_data

    mov r24,dek
    rcall lcd_data          ; send one byte of 1st decimal to the lcd's controller

    mov r24,ek
    rcall lcd_data          ; send one byte of 2nd decimal to the lcd's controller

    ldi r24, low(100*FOSC_MHZ)     ; wait 1 sec
    ldi r25, high(100*FOSC_MHZ)
    rcall wait_msec

    ldi r24, low(1000*FOSC_MHZ)
    ldi r25, high(1000*FOSC_MHZ)
    rcall wait_usec

    rjmp main               ; loop forever

write_2_nibbles:
    push r24                ; save r24(LCD Data)
    in r25, PIND            ; read PIND
    andi r25, 0x0f
    andi r24, 0xf0          ; r24[3:0] Holds previus PORTD[3:0]
    add r24, r25            ; r24[7:4] <-- LCD_Data_High_Byte
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3

    pop r24                 ; Recover r24(LCD_Data)
    swap r24
    andi r24, 0xf0          ; r24[3:0] Holds previus PORTD[3:0]
    add r24, r25            ; r24[7:4] <-- LCD_Data_Low_Byte
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3
    ret

lcd_data:
    sbi PORTD, PD2          ; LCD_RS=1(PD2=1), Data
    rcall write_2_nibbles   ; send data
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ret

lcd_command:
    cbi PORTD, PD2          ; LCD_RS=0(PD2=0), Instruction
    rcall write_2_nibbles   ; send Instruction
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ret

lcd_clear_display:
    ldi r24, 0x01           ; clear display command
    rcall lcd_command
    ldi r24, low(5)
    ldi r25, high(5)        ; Wait 5 mSec
    rcall wait_msec
    ret

lcd_init:
    ldi r24, low(200)
    ldi r25, high(200)      ; Wait 200 mSec
    rcall wait_msec
    ldi r24, 0x30           ; command to switch to 8 bit mode
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ldi r24, 0x30           ; command to switch to 8 bit mode
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ldi r24, 0x30           ; command to switch to 8 bit mode
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    cbi PORTD, PD3
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ldi r24, 0x20           ; command to switch to 4 bit mode
    out PORTD, r24
    sbi PORTD, PD3          ; Enable Pulse
    nop
    nop
    nop
    cbi PORTD, PD3
    ldi r24, 250
    ldi r25, 0              ; Wait 250uSec
    rcall wait_usec
    ldi r24, 0x28           ; 5x8 dots, 2 lines
    rcall lcd_command
    ldi r24, 0x0c           ; dislay on, cursor off
    rcall lcd_command

    rcall lcd_clear_display
    ldi r24, 0x06           ; Increase address, no display shift
    rcall lcd_command
    ret

wait_msec:
    push r24                ; 2 cycles
    push r25                ; 2 cycles
    ldi r24, low(999)       ; 1 cycle
    ldi r25, high(999)      ; 1 cycle
    rcall wait_usec         ; 998.375 usec
    pop r25                 ; 2 cycles
    pop r24                 ; 2 cycles
    nop                     ; 1 cycle
    nop                     ; 1 cycle
    sbiw r24, 1             ; 2 cycles
    brne wait_msec          ; 1 or 2 cycles
    ret                     ; 4 cycles

wait_usec:
    sbiw r24, 1             ; 2 cycles (2/16 usec)
    call delay_8cycles      ; 4+8=12 cycles
    brne wait_usec          ; 1 or 2 cycles
    ret

delay_8cycles:
    nop
    nop
    nop
    ret

; interrupt routine for ADC
ISR_ADC:
    cli                     ; deactivate interrupts inside interrupt routine
    push r24                ; save r24
    push r25                ; save r25
    push temp               ; save temp
    in temp, SREG           ; save SREG
    push temp

    lds ADC_L,ADCL          ; Read ADC result(Left adjusted)
    lds ADC_H,ADCH

    mov temp, ADC_H         ; hold copy of ADC data
    mov temp2, ADC_L

    ; we want to multiply ADC data with 5
    ; in order to do this shift ADC 2 times to the right
    ; and add that to the original ADC data
    ; keep the 3 MSB (the carry and the 2MSB of temp)

    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    andi temp, 0x3F         ; isolate desired bits
    andi temp2, 0xF0
    add temp2, ADC_L
    adc temp, ADC_H
    ; now the carry and the 2 MSB of temp holds
    ; the integer part of the Vin
    mov ADC_L, temp2        ; hold copy of the values
    mov ADC_H, temp
    rol temp
    rol temp
    rol temp
    andi temp, 0x07         ; now the 3LSB of temp hold
                            ; the integer part of the division
    ; lcd code for 0: 0x30, for 1:0x31...
    subi temp, -(0x30)      ; addi temp, 0x30
    mov mon, temp           ; argument to lcd_data

    ;1st decimal
    andi ADC_H, 0x3F        ; remove the integer part of Vin
    mov temp2, ADC_L        ; fetch copy of the values
    mov temp, ADC_H

    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    andi temp, 0x0F         ; isolate desired bits
    andi temp2, 0xFC
    add temp2, ADC_L
    adc temp, ADC_H
    mov ADC_L, temp2        ; hold copy of the values
    mov ADC_H, temp

    ror temp
    ror temp
    ror temp
    andi temp, 0x0F
    subi temp, -(0x30)      ; addi temp, 0x30
    mov dek, temp           ; argument to lcd_data

    ; 2nd decimal
    andi ADC_H, 0x07        ; remove the integer part of Vin
    mov temp2, ADC_L        ; fetch copy of the values
    mov temp, ADC_H

    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    ror temp                ; rotate ADC_H, ADC_H(0)->C
    ror temp2               ; rotate ADC_L, C->ADC_L(7)
    andi temp, 0x0F         ; isolate desired bits
    andi temp2, 0xFF
    add temp2, ADC_L
    adc temp, ADC_H
    mov ADC_L, temp2        ; hold copy of the values
    mov ADC_H, temp

    andi temp, 0x0F
    subi temp, -(0x30)      ; addi temp, 0x30
    mov ek, temp            ; argument to lcd_data

    sei                     //enable interrupts
    pop temp
    out SREG, temp          ; restore SREG
    pop temp                ; restore temp
    pop r25                 ; restore r25
    pop r24                 ; restore r24
    reti                    ; return to callee
