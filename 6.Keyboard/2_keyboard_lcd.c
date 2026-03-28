/*
 * Exercise 6.2
 * Functions from Exercise 6.1 (scan_row, scan_keypad,
 * convert, scan_keypad_rising_edge, keypad_to_ascii) as well as TWI/I2C
 * functions from Exercise 5.1 are used. Functions "pressed" and "LED" are not used.
 * LCD functions are included below.
 */

#include <avr/io.h>
#include <util/delay.h>

void Enable_Pulse() {
    PORTD |= (1 << PIND3);
    _NOP();
    _NOP();
    PORTD &= ~(1 << PIND3);
}

void write_2_nibbles(char data) {
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

void lcd_data(char data) {
    PORTD |= (1 << PIND2);
    write_2_nibbles(data);
    _delay_us(250);
}

void lcd_command(char cmd) {
    PORTD &= ~(1 << PIND2);
    write_2_nibbles(cmd);
    _delay_us(250);
}

void lcd_clear_display() {
    int cmd = 0x01;
    lcd_command(cmd);
    _delay_ms(5);
}

void lcd_init() {
    _delay_ms(200);
    for (int i=1; i<=3; i++) {
        PORTD = 0x30;   //command to switch to 8-bit mode
        Enable_Pulse();
        _delay_us(250);
    }

    PORTD = 0x20;   //command to switch to 8-bit mode
    Enable_Pulse();
    _delay_us(250);

    int cmd = 0x28;
    lcd_command(cmd);

    cmd = 0x0C;
    lcd_command(cmd);

    lcd_clear_display();

    cmd = 0x06;
    lcd_command(cmd);
}

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
