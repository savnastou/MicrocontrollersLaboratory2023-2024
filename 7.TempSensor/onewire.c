#include <avr/io.h>
#include <util/delay.h>

uint8_t temp_fun, trans_bit;

int one_wire_reset() {
    int pd_state;
    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(480);                  //480 usec reset pulse

    DDRD &= ~(1 << PIND4);          //set PD4 as input
    PORTD &= ~(1 << PIND4);         //disable pull-up
    _delay_us(100);                  //wait 100 usec for connected devices

    pd_state = PIND;                 //read PORTD
    _delay_us(380);                  //wait for 380 usec

    pd_state &= (0x10);

    if(pd_state==0x10) return 0;    //if device not detected (PD4=1) return 0
    else return 1;                   //else return 1
}

uint8_t one_wire_receive_bit() {
    uint8_t rec_bit;
    int pd_state;

    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(2);                    //time slot 2 usec

    DDRD &= ~(1 << PIND4);          //set PD4 as input
    PORTD &= ~(1 << PIND4);
    _delay_us(10);                   //wait 10 usec

    pd_state = PIND & (0x10);

    if(pd_state==0x10) rec_bit = 1;

    else rec_bit = 0;

    _delay_us(49);
    return rec_bit;                  //return received bit
}

void one_wire_transmit_bit() {
    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(2);

    if(trans_bit==0x01) PORTD |= (1 << PIND4);
    else PORTD &= ~(1 << PIND4);

    _delay_us(58);

    DDRD &= ~(1 << PIND4);          //set PD4 as input
    PORTD &= ~(1 << PIND4);         //disable pull-up
    _delay_us(1);

    return;
}

void one_wire_receive_byte() {
    uint8_t rec_bit;
    uint8_t temp=0x01;
    for(int i=1; i<=8; i++)         //8 repetitions
    {
        rec_bit = one_wire_receive_bit();
        if (rec_bit==1) {
            temp_fun |= (temp);
        }
        temp <<= 1;                  //next bit
    }
    return;
}

void one_wire_transmit_byte() {
    for(int i=0; i<=7; i++)         //8 repetitions
    {
        trans_bit = temp_fun & (0x01);
        one_wire_transmit_bit();
        temp_fun >>= 1;
    }
    return;
}