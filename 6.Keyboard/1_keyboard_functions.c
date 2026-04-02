#include <avr/io.h>
#include <util/delay.h>

#include "keyboard.c"

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
