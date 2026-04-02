#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include "twi.c"

void main(void) {
    DDRB=0xF0;       //initialise PB0-PB1 as inputs
    char A,B,C,D,temp1,temp2,F0,F1,output,input;
    twi_init();
    PCA9555_0_write(REG_CONFIGURATION_0, 0xFC); //Set EXT_PORT0.0 and EXT_PORT0.1 as outputs

    output=0;
    while(1){
        input=~PINB;         //switch has reverse logic
        A=input&0x01;        //isolate each switch input
        B=(input&0x02)>>1;
        C=(input&0x04)>>2;
        D=(input&0x08)>>3;
        temp1=(~A)&B;        //compute F0
        temp2=(~B)&C;
        temp2=temp2&D;
        F0=temp1|temp2;
        F0=(~F0)&0x01;
        temp1=A&C;
        temp2=B|D;
        F1=(temp1&temp2)<<1; //compute F1
        output=F0|F1;        //write 2 bytes
        PCA9555_0_write(REG_OUTPUT_0, output);
    }
}
