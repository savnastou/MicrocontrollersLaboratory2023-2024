#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <util/delay.h> // _delay_ms(), _delay_us()

void Enable_Pulse (void)
{
    PORTD |= (1 << PIND3);
    _NOP();
    _NOP();
    PORTD &= ~(1 << PIND3);
}

void write_2_nibbles (char data)
{
    int input = (PIND & 0x0F);
    int temp;

    temp = (data & 0xF0);
    int result = temp | input;
    PORTD = result;
    Enable_Pulse();

    data = ((data<<4) & 0xF0);
    result = data | input;
    PORTD = result;

    Enable_Pulse();
}

void lcd_data (char data)
{
    PORTD |= (1 << PIND2);
    write_2_nibbles (data);
    _delay_us(250);
}

void lcd_command (char cmd)
{
    PORTD &= ~(1 << PIND2);
    write_2_nibbles (cmd);
    _delay_us(250);
}

void lcd_clear_display (void)
{
    int cmd = 0x01;
    lcd_command (cmd);
    _delay_ms(5);
}

void lcd_init(void)
{
    _delay_ms(200);
    for (int i=1; i<=3; i++){
        PORTD = 0x30;   //command to switch to 8-bit mode
        Enable_Pulse();
        _delay_us(250);
    }

        PORTD = 0x20;   //command to switch to 8-bit mode
        Enable_Pulse();
        _delay_us(250);

        int cmd = 0x28;
        lcd_command (cmd);

        cmd = 0x0C;
        lcd_command (cmd);

        lcd_clear_display();

        cmd = 0x06;
        lcd_command (cmd);
}

void main(void) {
    DDRD = 0xFF;    // set portD as output
    DDRC = 0x00;    // set portB as input

    // REFSn[1:0]=01 => select Vref=5V, MUXn[4:0]=0010 => select ADC2(pin PC2),
    // ADLAR=0 => Right adjust the ADC result
    ADMUX = 0b01000010;

    // ADEN=1 => ADC Enable, ADCS=0 => No Conversion,
    // ADIE=0 => disable adc interrupt, ADPS[2:0]=111 => fADC=16MHz/128=125KHz
    ADCSRA = 0b10000111;

    lcd_init();         // init lcd
    _delay_ms(2);       // wait for lcd init

    while(1) {
        ADCSRA |= (1<<ADSC); // Set ADSC flag of ADCSRA
                             // enable conversion

        while((ADCSRA & (1<<ADSC)) == (1<<ADSC));
        // wait until flags become zero
        // that means that the conversion is complete
        int adc = ADC;
        double vin = adc*5.0/1024;

        char temp = (int)vin;   // integer part of vin
        temp += 0x30;           //offset
        lcd_data(temp);

        lcd_data(0x2E); // dot

        temp = ((int)(vin*10)%10);  // 1st decimal
        temp += 0x30;
        lcd_data(temp);

        temp = ((int)(vin*100)%10); // 2nd decimal
        temp += 0x30;
        lcd_data(temp);
        _delay_ms(100);

        _delay_ms(1000);
        lcd_command(0x01);  // clear display
        _delay_us(5000);
    }
}
