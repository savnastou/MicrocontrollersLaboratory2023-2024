#define F_CPU 16000000UL
#include <avr/cpufunc.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <math.h>
#include <xc.h>
#include <string.h>
#include <util/delay.h> // _delay_ms(), _delay_us

#include "twi.h"
#include "uart.h"
#include "lcd.h"

////////////////////////////////
uint8_t temp_fun, trans_bit;

char *temp_string;

void lcd_print(const char* string)  //print a string to lcd screen
{                                   //calling functions from "lcd.h"
    int i=0;
    while (string[i]!='\0'){
        lcd_data(string[i]);
        i++;
    }
}

void transmit_string(const char *string) //transmit a string through uart
{                                        //calling functions from "uart.h"
    int i=0;
    while(string[i]!='\0')
    {
        usart_transmit(string[i]);
        i++;
    }
}

char *receive_string(){ //receive a string through uart
    char *string;       //calling functions from "uart.h"
    int i=0;
    do{
        string[i]=usart_receive();
        i++;
    }
    while(string[i-1]!='\n');
    return string;
}

// ---- Exercise 8.2: delay_keypad (added before main) ----
void delay_keypad()
{
    char character;
    character=keypad_to_ascii();
    if(character=='3') flag=true;
    if(character=='#') flag=false;
}

// ---- Exercise 8.3: delay_keypad with loop (added before main) ----
// void delay_keypad()
// {
//     char character;
//     for(int i=1; i<=250; i++)
//     {
//         _delay_ms(1);
//         character=keypad_to_ascii();
//         if(character=='3') flag=true;
//         if(character=='#') flag=false;
//     }
// }

int main(){
    char *result;

    twi_init();                                     //initialising twi, calling functions from "twi.h"
    PCA9555_0_write(REG_CONFIGURATION_0, 0x00);    //Set EXT_PORT0 as output, calling functions from "twi.h"
    lcd_init();                                     //initialising lcd screen, calling functions from "lcd.h"
    usart_init(103);                                //initialising UART, calling functions from "uart.h"

    lcd_clear_display();                            //clear lcd from previously loaded code

    // -------zht1-------
    const char* temp_string = "ESP:connect\n";
    transmit_string(temp_string);                   //send to UART: connect ESP to network

    result = receive_string();                      //read from UART whether successful or not

    if(result[1]=='S') lcd_print("1.Success");     //if string received is "Success", then print "1.Success" to lcd
    else lcd_print("1.Fail");                       //else if string received is "Fail", then print "1.Fail" to lcd
    _delay_ms(1000);                               //1 sec delay for us to see

    lcd_clear_display();                            //clear lcd for next print

    transmit_string("ESP:url:\"http://192.168.1.250:5000/data\"\n"); //send to UART: the url to transmit to
    result=receive_string();                        //read from UART whether successful or not

    if(result[1]=='S') lcd_print("2.Success");     //if string received is "Success", then print "2.Success" to lcd
    else lcd_print("2.Fail");                       //else if string received is "Fail", then print "2.Fail" to lcd
    _delay_ms(1000);                               //1 sec delay for us to see
    lcd_clear_display();                            //clear lcd for next print

    // -------zht2-------
    while(1){

        const char* string1= "ESP:payload:[{\"name\": \"temperature\",\"value\":\"";
        const char* string2 = "\"},{\"name\": \"pressure\",\"value\":\"";
        const char* string3 = "\"},{\"name\":\"team\",\"value\": \"S3\"},{\"name\": \"status\",\"value\":\"";
        const char* string4= "\"}\n";
        char string_num[5],string_num2[5];

        float temper_dec, pressure;

        //measure temperature
        temper_dec=read_temp_float()+ 9; ; //must add value to get to 36 degrees

        //measure pressure
        ADCSRA |=(1 << ADSC);               //A conversion starts
        while ((ADCSRA & 0b01000000)!=0){} //while ADCS flag==1 wait

        pressure = ADC*20.0/1024;

        const char* pressur="CHECKPRESSURE";
        const char* temperatur="CHECKTEMP    ";
        const char* nurse_call="NURSECALL    ";

        if (flag==false) {
            strcpy(status,"OK");

            if(pressure > 12 || pressure <4) strcpy(status,pressur);
            else if(temper_dec < 34 || temper_dec > 37) strcpy(status,temperatur);
        }

        // state of nurse call
        if(flag){
            strcpy(status,nurse_call);
        }

        char string_fin[strlen(string1)+5+strlen(string2)+5+strlen(string3)+strlen(status)+strlen(string4)];

        //temperature to payload
        int dec = ((int)temper_dec/10)%10;
        string_num[0]=dec+'0';
        lcd_data(string_num[0]);

        int mon = (int)temper_dec%10;
        string_num[1]=mon+'0';
        lcd_data(string_num[1]);

        string_num[2]='.';
        lcd_data(string_num[2]);
        int dekata = (int)(temper_dec*10)%10;
        string_num[3]=dekata+'0';
        lcd_data(string_num[3]);
        string_num[4]='\0';

        lcd_data(' ');

        //pressure to payload
        dec = ((int)pressure/10)%10;
        string_num2[0]=dec+'0';
        lcd_data(string_num2[0]);

        mon = (int)pressure%10;
        string_num2[1]=mon+'0';
        lcd_data(string_num2[1]);

        string_num2[2]='.';
        lcd_data(string_num2[2]);
        dekata = (int)(pressure*10)%10;
        string_num2[3]=dekata+'0';
        lcd_data(string_num2[3]);
        string_num2[4]='\0';

        lcd_command(0xc0);
        lcd_print(status);

        _delay_ms(250);
        lcd_clear_display();

        delay_keypad();

        //transmit payload
        strcpy(string_fin,string1);
        strcat(string_fin,string_num);

        strcat(string_fin,string2);

        strcat(string_fin,string_num2);
        strcat(string_fin,string3);
        strcat(string_fin,status);
        strcat(string_fin,string4);
        transmit_string(string_fin);

        // -------zht3 additions-------
        receive_string(result);                             //read from UART whether successful or not
        if(result[1]=='S') lcd_print("3.Success");         //if successful print "3.Success"
        else lcd_print("3.Fail");

        delay_keypad();
        lcd_clear_display();

        const char* string_trans= "ESP:transmit\n";
        transmit_string(string_trans);

        receive_string(result);
        lcd_print(result);

        delay_keypad();
        lcd_clear_display();
    }
}
