#include <stdbool.h>
#include <xc.h>
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile int timer;
bool fwta;
unsigned char output;

ISR(INT1_vect)
{
    timer=3000; //new interrupt just occured so we reset timer
    //sei();
    if(fwta){   //if new interrupt happened during the previous interrupt
        output=0xFF; //turn on all lights
    }
    else { //interrupt happened when lights were off
        output=0x01; //only LED0 on
    }
    fwta=1; //we're in an interrupt so flag <- 1
}

void main(void) {
    //reset:
    EICRA=(1<<ISC11)|(1<<ISC10);  //interrupt on rising edge
    EIMSK=(1<<INT1);              //enable INT1 interrupt
    sei();

    DDRB=0xFF;          //portB as output
    PORTB=0x00;         //initialise LEDs to zero

    DDRD=0x00;          //portD as input
    PORTD=0x00;

    timer=0;    //counts how many ms we have until LEDs turn off
    output=0;   //helper variable for setting LEDs
    fwta=0;     //flag variable
                //set to zero because we're in main

    while(1){
        if(timer>0){
            timer--;
        }
        if (timer==2500){ //for the 2.5 sec left only LED 0 has to be on
            output=0x01;
        }
        if(timer==0){ //3 secs have passed so LEDs turn off
            output=0;
            fwta=0;
        }
        _delay_ms(1);
        PORTB=output;   //update LEDs every 1 ms
    }
}
