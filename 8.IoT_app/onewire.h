// onewire.h

uint8_t temp_fun, trans_bit;

int one_wire_reset()
{
    int pd_state;
    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(480);                 //480 usec reset pulse

    DDRD &= ~(1 << PIND4) ;        //set PD4 as input
    PORTD &= ~(1 << PIND4);        //disable pull-up
    _delay_us(100);                 //wait 100 usec for connected devices

    pd_state = PIND;                //read PORTD
    _delay_us(380);                 //wait for 380 usec

    pd_state &= (0x10);

    if(pd_state==0x10) return 0;   //if device not detected(PD4=1) return 0
    else return 1;                  //else return 1
}

uint8_t one_wire_receive_bit()
{
    uint8_t rec_bit;
    int pd_state;

    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(2);                   //time slot 2 usec

    DDRD &= ~(1 << PIND4) ;        //set PD4 as input
    PORTD &= ~(1 << PIND4);
    _delay_us(10);                  //wait 10 usec

    pd_state = PIND & (0x10);

    if(pd_state==0x10) rec_bit = 1;

    else rec_bit = 0;

    _delay_us(49);
    return rec_bit;                 //return received bit
}

void one_wire_transmit_bit()
{
    DDRD |= (1 << PIND4);           //set PD4 as output
    PORTD &= ~(1 << PIND4);
    _delay_us(2);

    if(trans_bit==0x01) PORTD |= (1 << PIND4);
    else PORTD &= ~(1 << PIND4);

    _delay_us(58);

    DDRD &= ~(1 << PIND4) ;        //set PD4 as input
    PORTD &= ~(1 << PIND4);        //disable pull-up
    _delay_us(1);

    return;
}

void one_wire_receive_byte()
{
    uint8_t rec_bit;
    uint8_t temp=0x01;
    for(int i=1; i<=8; i++)        //8 repetitions
    {
        rec_bit = one_wire_receive_bit();
        if (rec_bit==1) {
            temp_fun |= (temp);
        }
        temp <<= 1;                 //next bit
    }
    return;
}

void one_wire_transmit_byte()
{
    for(int i=0; i<=7; i++)        //8 repetitions
    {
        trans_bit = temp_fun &(0x01);
        one_wire_transmit_bit();
        temp_fun >>= 1;
    }
    return;
}

uint16_t read_temp()
{
    uint8_t term_conv;
    uint16_t temper = 0x0000, temper2;
    int dev_con, dev_con2;

    dev_con = one_wire_reset();             //for first 8 bits
    if(dev_con){                            //if succesful
        temp_fun = 0xCC;                    //skip selection of device
        one_wire_transmit_byte();
        temp_fun = 0x44;                    //start measurement
        one_wire_transmit_byte();

        _delay_ms(750);
        term_conv = one_wire_receive_bit(); //wait to receive bit
        while(term_conv==0)
            term_conv = one_wire_receive_bit();

        dev_con2 = one_wire_reset();        //for last 8 bits

        if(dev_con2){                       //if succesful
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

float read_temp_float()
{
    uint16_t temper_bin;
    float temper_dec;

    temper_bin = read_temp();
    if (temper_bin==0x8000)
    {
        return 1;
    }
    else
    {
        temper_dec = temper_bin*0.0625;
        return temper_dec;
    }
}
