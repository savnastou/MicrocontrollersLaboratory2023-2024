#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "twi.c"

void main(void) {
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0xF0); //Set EXT_PORT0 0-3 as outputs

    PCA9555_0_write(REG_CONFIGURATION_1, 0xF0); //Set EXT_PORT1 4-7 as inputs and EXT_PORT1 0 as output
                                                 //leds as output

    char input,output;

    while(1){
        output=0;
        input=PCA9555_0_read(REG_INPUT_1);
        if ((input& 0x10) == 0x00) output=output|(0x01);   //ypothetoume antistrofhs logikhs dhladh FF
        if ((input& 0x20) == 0x00) output=output|(0x02);   //otan den patietal tipota
        if ((input& 0x40) == 0x00) output=output|(0x04);
        if ((input& 0x80) == 0x00) output=output|(0x08);
        PCA9555_0_write(REG_OUTPUT_0, output);
    }
}
