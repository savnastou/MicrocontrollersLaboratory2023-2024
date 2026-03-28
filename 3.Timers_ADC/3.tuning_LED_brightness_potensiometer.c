#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <util/delay.h>

void main(void) {
    int k=0;                //k->mode
    TCCR1A = (1 << WGM10) | (1 << COM1A1);
    TCCR1B = (1 << WGM12) | (1 << CS10);
    ADMUX=0b01100000;       //Vref=5V, ADC1, left adjust
    ADCSRA=0b10000111;      //ADEN=1, ADSC=0

    DDRB=0b00111111;
    DDRD = 0b00000000;      //buttons of PORTD

    float PWM_2;
    int DC_VALUE;
    int j=0;
    int PWM_1[13]={5,26,46,67,87,108,125,148,169,189,210,230,251};

    j=6;
    DC_VALUE = PWM_1[j];
    OCR1AL= DC_VALUE;

    while(1){

        int input=PIND;
        if((PIND & 0x40)==0x00){    //if user pushes PD6 (mode1)
            k=1;
        }
        if((PIND & 0x80)==0x00){    //if user pushes PD7 (mode2)
            k=2;
        }

        if(k==1){                   //mode1
            if ((PIND & 0x02)==0x00){   //if user pushes PD1

                while ((PIND & 0x02)==0x00) {
                    _delay_ms(5);       //debouncing
                }
                j++;
                if(j<=12)
                    DC_VALUE = PWM_1[j];
                else j=12;
            }

            if ((PIND & 0x04)==0x00){   //if user pushes PD2

                while((PIND & 0x04)==0x00){
                    _delay_ms(5);
                }
                j--;
                if(j>=0)
                    DC_VALUE = PWM_1[j];
                else j=0;
            }

            OCR1A = DC_VALUE;
        }

        else if (k==2){             //mode2
            ADCSRA=ADCSRA|1<<ADSC;              //start ADC conversion
            while ((ADCSRA&0b01000000)!=0){}    //while conversion hasn't finished wait
            DC_VALUE=ADCH;                      //keep 8 most important bits of ADC
            OCR1AL=DC_VALUE;
        }

        input=PIND;
    }
}
