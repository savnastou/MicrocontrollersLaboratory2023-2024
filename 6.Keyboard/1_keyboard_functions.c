/*
 * Exercise 6.1
 * Note: TWI/I2C functions from Exercise 5.1
 * (twi_init, twi_write, twi_start, twi_stop, PCA9555_0_write, PCA9555_0_read, κλπ.)
 * are ommited.
 */

#include <avr/io.h>
#include <util/delay.h>

// ascii lookup table for keypad
char ascii[] = { ... }; // provided by assignment

uint8_t row[4]; // global array holding state of each row

// α) scan_row: takes the row as parameter and returns state of row in the 4MSBs
uint8_t scan_row(int i) {
    if(i == 0) PCA9555_0_write(REG_OUTPUT_1, 0x0E);   //set row that has been called to low
    else if(i == 1) PCA9555_0_write(REG_OUTPUT_1, 0x0D);
    else if(i == 2) PCA9555_0_write(REG_OUTPUT_1, 0x0B);
    else if(i == 3) PCA9555_0_write(REG_OUTPUT_1, 0x07);
    _delay_us(100);
    return ((0xF0)&PCA9555_0_read(REG_INPUT_1));       //read buttons of corresponding line
                                                       //exclude 4 LSBs which match to rows
}

// β) scan_keypad: run scan_row() for all rows
void scan_keypad() {
    for (int i=0; i<4; i++) {
        row[i] = scan_row(i);
    }
}

// convert: convert 8-bit row table to 16-bit variable "buttons"
uint16_t convert() {
    uint16_t a,b,c,d;
    a=((uint16_t)row[0]);
    a>>=4;              //row 0 at bits 0-3
    b=((uint16_t)row[1]); //row 1 at bits 4-7
    c=((uint16_t)row[2]);
    c<<=4;              //row 2 at bits 8-11
    d=((uint16_t)row[3]);
    d<<=8;              //row 3 at bits 12-15
    uint16_t buttons;
    buttons=a|b|c|d;
    return buttons;
}

uint16_t prev_buttons=0xFFFF;  //initialisation at "no buttons pressed"

// γ) scan_keypad_rising_edge: returns number of pressed button from 0 to 15
int scan_keypad_rising_edge() {
    uint16_t buttons,different;
    int i=0;
    scan_keypad();              //read current state of buttons
    buttons=convert();          //to 16-bit variable "buttons"
    different=buttons^prev_buttons; //bitwise xor
    prev_buttons=buttons;
    if(different==0) {          //if keyboard state is same, return -1
        return -1;              //if xor=0->rising edge NOT detected ->return -1
    }
                                //rising edge only
    if (buttons==0xFFFF) return -1; //if no button pressed, return -1
    while (different!=0x01) {   //find number of pressed button from 0 to 15
        different=different>>1;
        i++;
    };

    return i;                   //returns number of pressed button from 0 to 15 or -1 if no button pressed
}

// δ) keypad_to_ascii: match number of pressed button with corresponding keyboard char
char keypad_to_ascii() {
    int button_pressed=scan_keypad_rising_edge();
    if(button_pressed == -1) return 0;   //return 0 for error
    return ascii[button_pressed];
}

// ε) pressed: checks which button is currently pressed (not rising edge)
int pressed() {
    int i=0;
    scan_keypad();
    uint16_t buttons=convert();
    uint16_t pressed;
    pressed=buttons^(0xFF);
    if (pressed==0) return -1;  //if no button pressed, return -1
    while (pressed!=0x01) {     //else find number of pressed button
        pressed=pressed>>1;
        i++;
    };
    return i;
}

// ε) LED: lights the correct LED based on which keypad button is held down
void LED() {
    char output,char_pressed;
    //here we do not want the rising edge-we want the button while pressed

    int pressed_but=pressed();
    if (pressed_but!=-1) char_pressed=ascii[pressed_but];
    else return;        //no button pressed so return
    output=0x00;        //choose led according to button pressed
    if (char_pressed=='1') output=0x01;
    else if (char_pressed=='5') output=0x02;
    else if (char_pressed=='9') output=0x04;
    else if (char_pressed=='D') output=0x08;
    PORTB=output;
}

int main() {
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00); //Set EXT_PORT0 as output
    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 4-7 as inputs and EXT_PORT1 0-3 as outputs

    DDRB=0xFF;          // set portB as output for leds
    PORTB=0x00;         //leds initially off
    while(1){
        LED();
    }
}
