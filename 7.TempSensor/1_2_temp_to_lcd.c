#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <util/delay.h> // _delay_ms(), _delay_us

#include "lcd.c"
#include "onewire.c"

// ================================
// Temperature reading function
// ================================

uint16_t read_temp() {
    uint8_t term_conv;
    uint16_t temper = 0x0000, temper2;
    int dev_con, dev_con2;

    dev_con = one_wire_reset();

    if(dev_con) {                       //if successful
        temp_fun = 0xCC;                //skip selection of device
        one_wire_transmit_byte();
        temp_fun = 0x44;                //start measurement
        one_wire_transmit_byte();

        _delay_ms(750);
        term_conv = one_wire_receive_bit();
        while(term_conv==0)             //wait to receive bit
            term_conv = one_wire_receive_bit();

        dev_con2 = one_wire_reset();

        if(dev_con2) {                  //if successful
            temp_fun = 0xCC;
            one_wire_transmit_byte();
            temp_fun = 0xBE;
            one_wire_transmit_byte();

            temp_fun = 0x00;
            one_wire_receive_byte();
            temper = (uint16_t)temp_fun;    //8 LSBs
            temp_fun = 0x00;
            one_wire_receive_byte();
            temper2 = (uint16_t)temp_fun;   //8 MSBs
            temper2 <<= 8;
            temper |= temper2;
            return temper;                  //temper contains all 16 bits
        }
        else return 0x8000;                 //in case of failure return 0x8000
    }
    else return 0x8000;                     //in case of failure return 0x8000
}

// ================================
// Main
// ================================

int main() {
    uint16_t temper_bin, temp_sign;
    float temper_dec;

    while(1)
    {
        temper_bin = read_temp();       //check for thermometer
        DDRD=0xFF;
        lcd_init();                     //initialise LCD
        lcd_clear_display();
        lcd_command(0x80);
        _delay_us(100);

        if (temper_bin==0x8000)         //if not found print message
        {
            lcd_data('N');
            lcd_data('O');
            lcd_data(' ');
            lcd_data('D');
            lcd_data('E');
            lcd_data('V');
            lcd_data('I');
            lcd_data('C');
            lcd_data('E');
        }
        else                            //else
        {
            temp_sign = temper_bin & (0xF000);          //find sign of temperature
            if (temp_sign==0xF000) {                    //if negative
                lcd_data('-');
                temper_bin = ~(temper_bin) + 0x0001;    //compliment to get absolute value
            }

            temper_dec = temper_bin*0.0625;             //convert to temperature

            if(temper_dec >= 100) {                     //print the right digits
                uint16_t ek = (uint16_t)temper_dec/100; //ekatontades
                lcd_data(ek+0x30);
                _delay_ms(2);
            }

            if(temper_dec >= 10) {                      //dekades
                uint16_t dec = ((uint16_t)temper_dec/10)%10;
                lcd_data(dec+0x30);
                _delay_ms(2);
            }

            uint16_t mon = (uint16_t)temper_dec%10;    //monades
            lcd_data(mon+0x30);
            _delay_ms(2);

            lcd_data('.');

            uint16_t dekata = (uint16_t)(temper_dec*10)%10;  //dekata
            lcd_data(dekata+0x30);
            _delay_ms(2);

            uint16_t ekatosta = (uint16_t)(temper_dec*100)%10; //ekatosta
            lcd_data(ekatosta+0x30);
            _delay_ms(2);

            uint16_t xiliosta = (uint16_t)(temper_dec*1000)%10; //xiliosta
            lcd_data(xiliosta+0x30);
            _delay_ms(2);

            lcd_data(223);              //print sign of degrees
            _delay_ms(2);

            lcd_data('C');              //print sign of Celsius
            _delay_ms(2);
        }
    }
}
