#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <util/delay.h> // _delay_ms(), _delay_us()
#include <stdbool.h>

#include "lcd.c"

void main(void)
{
    //ADC initialisations
    ADMUX=0b01000011;       //Vref=5V, ADC3, right adjust
    ADCSRA=0b10000111;      //ADEN=1, ADSC=0, prescale 128

    DDRD = 0xFF;
    DDRB = 0xFF;            //LEDs

    lcd_init();
    _delay_ms(100);

    float Vgas, a[6]={1,1.666,2.333,3,3.666,4.333};  //limits for LED switching
    int output;

    while(1)
    {
        ADCSRA |=(1 << ADSC); //A conversion starts

        while ((ADCSRA & 0b01000000)!=0){} //while ADCS flag==1 wait

        Vgas = ADC*5.0/1024;  //conversion of digital ADC output to analog voltage in range 0-5 V

        lcd_clear_display();

        if (Vgas >= 1)
        {
            lcd_data('G');
            lcd_data('A');
            lcd_data('S');
            lcd_data(' ');
            lcd_data('D');
            lcd_data('E');
            lcd_data('T');
            lcd_data('E');
            lcd_data('C');
            lcd_data('T');
            lcd_data('E');
            lcd_data('D');

            //select which LED to turn on
            if(Vgas<=a[1]) {
                output=1;
            }
            else if (Vgas<=a[2]){
                output=2;
            }
            else if (Vgas<=a[3]){
                output=4;
            }
            else if (Vgas<=a[4]){
                output=8;
            }
            else if (Vgas<=a[5]){
                output=16;
            }
            else if (Vgas> a[5]){
                output=32;
            }
            PORTB = output;
            _delay_ms(1000);
            PORTB = 0x00;
            _delay_ms(1000);
        }
        else
        {
            PORTB = 0x00;
            lcd_data('C');
            lcd_data('L');
            lcd_data('E');
            lcd_data('A');
            lcd_data('R');
            _delay_ms(100);
        }
    }
}
