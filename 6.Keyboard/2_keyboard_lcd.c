#include <avr/io.h>
#include <util/delay.h>

#include "keyboard.c"
#include "lcd.c"

int main() {
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 4-7 as inputs and EXT_PORT1 0-3 as outputs

    DDRD=0xFF;
    DDRB=0xFF;          // set portB as output for leds
    lcd_init();
    lcd_clear_display();
    _delay_us(2000);
    char character;
    while(1) {
        character=keypad_to_ascii();
        while(character==0) {   //no button pressed or same button pressed
            character=keypad_to_ascii();
        }

        lcd_clear_display();    //erase previous display
        lcd_data(character);    //display current button
    }
}
