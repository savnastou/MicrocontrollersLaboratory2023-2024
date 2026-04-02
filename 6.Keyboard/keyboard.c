#include <avr/io.h>
#include <util/delay.h>

#include "twi.c"

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