// keyboard.h

uint8_t row[4];
unsigned char ascii[16] = {'*', '0', '#', 'D', '7', '8', '9', 'C', '4', '5', '6', 'B', '1', '2', '3', 'A'};

uint8_t scan_row(int i) {           //takes the row as parameter and returns state of row in the 4MSBs
    if(i == 0) PCA9555_0_write(REG_OUTPUT_1, 0x0E);    //set row that has been called to low
    else if(i == 1) PCA9555_0_write(REG_OUTPUT_1, 0x0D);
    else if(i == 2) PCA9555_0_write(REG_OUTPUT_1, 0x0B);
    else if(i == 3) PCA9555_0_write(REG_OUTPUT_1, 0x07);
    _delay_us(100);
    return ((0xF0)&PCA9555_0_read(REG_INPUT_1));
}

void scan_keypad(){     //takes as argument the 4 rows of 8 bits (inputs in 4 MSBs) and returns current state of keypad
    for (int i=0; i<4; i++){
        row[i]= scan_row(i);
    }
}

uint16_t convert(){     //takes as parameter the table of rows and converts it to a 16-bit number
    uint16_t a,b,c,d;
    a=((uint16_t)row[0]);
    a>>=4;
    b=((uint16_t)row[1]);
    c=((uint16_t)row[2]);
    c<<=4;
    d=((uint16_t)row[3]);
    d<<=8;
    uint16_t buttons;
    buttons=a|b|c|d;
    return buttons;
}

uint16_t prev_buttons=0xFFFF;

int scan_keypad_rising_edge(){  //returns number of pressed button form 0 to 15 or -1 if no button pressed
    uint16_t buttons,different;
    int i=0;
    scan_keypad();
    buttons=convert();
    different=buttons^prev_buttons;     //bitwise XOR
    prev_buttons=buttons;
    if(different==0) {
        return -1;                      //if xor->rising edge NOT detected ->return -1
    }

    if (buttons==0xFFFF) return -1;    //rising edge only
    while (different!=0x01){
        different=different>>1;
        i++;
    };

    return i;
}

int pressed(){
    int i=0;
    scan_keypad();
    uint16_t buttons=convert();
    uint16_t pressed;
    pressed=buttons^(0xFF);
    if (pressed==0) return -1;
    while (pressed!=0x01){
        pressed=pressed>>1;
        i++;
    };
    return i;
}

char keypad_to_ascii() {
    int button_pressed=scan_keypad_rising_edge();
    if(button_pressed == -1) return 0;      //return 0 for error
    return ascii[button_pressed];
}
