#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <util/delay.h>

void main(void) {

    unsigned char DC_VALUE, DC[13]={5,26,46,67,87,108,125,148,169,189,210,230,251};

    //set TMR1A in fast PWM 8 bit mode with non-inverted output
    //prescale =8
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS10);

    //ADC initialisations
    ADMUX=0b01000001;       //Vref=5V, ADC1, right adjust
    ADCSRA=0b10000111;      //ADEN=1, ADSC=0 prescaler <- 128

    DDRB = 0b11110011;      //PORTB ouput except for PD2 & PD3 that are inputs
    DDRD = 0xFF;            //output leds
    PORTD = 0x00;           //LEDs initially turned off

    float V_dc;             //input from ADC
    int output=0;           //output to LEDs
    float a[9]={0,0.625,1.25,1.875,2.5,3.125,3.75,4.375,5};    //analog inputs of ADC

    int j=6;                //initialise DC at 50% (roughly 125)
    DC_VALUE = DC[j];

    while(1)
    {
        if (~PINB & 0x04)   //PD2 pressed
        {
            while(~PINB & 0x04)
                _delay_ms(5);   //debouncing check
            j++;
            if(j<=12)
                DC_VALUE = DC[j]; // DC is not yet 98% so increase by 8%
            else j=12;  //DC is 98% so keep this value
        }

        if (~PINB & 0x08)   //PD3 pressed
        {
            while(~PINB & 0x08)
                _delay_ms(5);   //debouncing check
            j--;
            if(j>=0)
                DC_VALUE = DC[j]; // DC is not yet 2% so decrease by 8%
            else j=0; //DC is 2% so keep this value
        }

        OCR1AL = DC_VALUE; //output to PB1 LED

        ADCSRA |=(1 << ADSC); //A conversion starts

        while ((ADCSRA & 0b01000000)!=0){}  //while ADCS flag==1 wait

                                            //if ADCS flag=0 cases{
        V_dc = ADCW*5.0/1024.0;    //convert digital output of ADC into analog voltage

        if(V_dc<=a[1]) {            //turn on the right PORTD LED according to table
            output=1;
        }
        else if (V_dc<=a[2]){
            output=2;
        }
        else if (V_dc<=a[3]){
            output=4;
        }
        else if (V_dc<=a[4]){
            output=8;
        }
        else if (V_dc<=a[5]){
            output=16;
        }
        else if (V_dc<=a[6]){
            output=32;
        }
        else if (V_dc<=a[7]){
            output=64;
        }
        else if (V_dc<=a[8]){
            output=128;
        }

        PORTD = output;
        _delay_ms(100);     //ADC gets input every 100 ms
    }
}
