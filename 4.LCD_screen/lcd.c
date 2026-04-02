#include <avr/io.h>
#include <util/delay.h>

void Enable_Pulse (void)
{
    PORTD |= (1 << PIND3);
    _NOP();
    _NOP();
    PORTD &= ~(1 << PIND3);
}

void write_2_nibbles (char data)
{
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

void lcd_data (char data)
{
    PORTD |= (1 << PIND2);
    write_2_nibbles (data);
    _delay_us(250);
}

void lcd_command (char cmd)
{
    PORTD &= ~(1 << PIND2);
    write_2_nibbles (cmd);
    _delay_us(250);
}

void lcd_clear_display (void)
{
    int cmd = 0x01;
    lcd_command (cmd);
    _delay_ms(5);
}

void lcd_init(void)
{
    _delay_ms(200);
    for (int i=1; i<=3; i++){
        PORTD = 0x30;   //command to switch to 8-bit mode
        Enable_Pulse();
        _delay_us(250);
    }

        PORTD = 0x20;   //command to switch to 8-bit mode
        Enable_Pulse();
        _delay_us(250);

        int cmd = 0x28;
        lcd_command (cmd);

        cmd = 0x0C;
        lcd_command (cmd);

        lcd_clear_display();

        cmd = 0x06;
        lcd_command (cmd);
}