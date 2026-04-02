#include <avr/io.h>
#include <util/delay.h>

int state;

void Enable_Pulse()
{
    state |= (1<<3);
    PCA9555_0_write(REG_OUTPUT_0, state);
    _NOP();
    _NOP();
    state &= ~(1<<3);
    PCA9555_0_write(REG_OUTPUT_0, state);
}

void write_2_nibbles(char data)
{
    //uint8_t input_pex=PCA9555_0_read(REG_INPUT_1);
    //int input = (input_pex & 0x0F);
    int input = (state & 0x0F);
    int temp;

    temp = (data & 0xF0);
    int result = temp | input;
    PCA9555_0_write(REG_OUTPUT_0, result);
    //PORTD = result;
    state=result;
    Enable_Pulse();

    data = ((data<<4) & 0xF0);
    result = data | input;
    PCA9555_0_write(REG_OUTPUT_0, result);
    //PORTD = result;
    state=result;
    Enable_Pulse();
}

void lcd_data(char data)
{
    state |= (1<<2);
    PCA9555_0_write(REG_OUTPUT_0, state);
    //PORTD |= (1 << PIND2);

    write_2_nibbles(data);
    _delay_us(250);
}

void lcd_command(char cmd)
{
    state &= ~(1<<2);
    PCA9555_0_write(REG_OUTPUT_0, state);
    //PORTD &= ~(1 << PIND2);
    write_2_nibbles(cmd);
    _delay_us(250);
}

void lcd_clear_display ()
{
    int cmd = 0x01;
    lcd_command(cmd);
    _delay_ms(5);
}

void lcd_init()
{
    _delay_ms(200);
    for (int i=1; i<=3; i++){
        //PORTD = 0x30;  //command to switch to 8-bit mode
        state=0x30;
        PCA9555_0_write(REG_OUTPUT_0, state);
        Enable_Pulse();
        _delay_us(250);
    }

        //PORTD = 0x20;  //command to switch to 8-bit mode
        state=0x20;
        PCA9555_0_write(REG_OUTPUT_0, state);
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
