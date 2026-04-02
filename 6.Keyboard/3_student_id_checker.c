#include <avr/io.h>
#include <util/delay.h>

#include "keyboard.c"
#include "lcd.c"

void check_num(char character_1, char character_2) {
    if ((character_1=='5')&&(character_2=='3')) //correct team number (53)
    {
        PORTB=0b00111111;
        _delay_ms(4000);    //leds on for 4 sec
        PORTB=0x00;
        _delay_ms(1000);    //extra delay 1 sec to fill 5 secs asked
    }
    else for (int i=0; i<10; i++) // 10 x ((2 x 250) ms) = 5 secs
    {
        PORTB=0b00111111;
        _delay_ms(250);     //leds on for 250 ms
        PORTB=0x00;
        _delay_ms(250);     //leds off for 250 ms
    }
}

int main() {
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 4-7 as inputs and EXT_PORT1 0-3 as outputs

    DDRD=0xFF;
    DDRB=0xFF;              // set portB as output for leds
    PORTB=0x00;             //leds initially off
    char character1,character2;
    lcd_init();
    lcd_clear_display();    //to clear screen from previous exercise
    _delay_us(2000);

    while(1) {
        character1=keypad_to_ascii();
        while(character1==0) {
            character1=keypad_to_ascii();
        }

        _delay_ms(10); // for debouncing

        character2=keypad_to_ascii();

        while(character2==0) {
            character2=keypad_to_ascii();
        }

        _delay_ms(10); // for debouncing

        check_num(character1,character2);
    }
}
