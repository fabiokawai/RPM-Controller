#include <tm4c123gh6pm.h>
#include <inttypes.h>

uint32_t time = 80;
//uint32_t time = 80; //4000000Hz / (500Hz*100%)
//50000 = 1s

uint32_t delay = 0;

uint32_t pwmPercentage = 99;

//Variaveis relacionadas aos botoes
uint32_t btn1 = 0;
uint32_t btn2 = 0;
uint32_t fixBug = 0;
uint32_t motorSwitch = 1;
uint32_t motorDirection = 1;
uint32_t motorValue;


//PF1 -
//PF2 +
//PF3 Inverte
//PF4 On/Off

void timerInterrupt()
{


    motorValue = GPIO_PORTA_DATA_R;
    if (motorSwitch == 1)
    { // Se o motor está ligado
        switch (motorDirection)
        {
        case 0:
            if (!(GPIO_PORTA_DATA_R & 4)) // Se sinal estiver LOW
            {
                delay = 100 - pwmPercentage;
                GPIO_PORTA_DATA_R = 4; // Seta HIGH
            }
            else // Se sinal estiver HIGH
            {
                delay = pwmPercentage;
                GPIO_PORTA_DATA_R = 0; // Seta LOW
            }
            break;
        case 1:
            if (!(GPIO_PORTA_DATA_R & 8)) // Se sinal estiver LOW
            {
                delay = 100 - pwmPercentage;
                GPIO_PORTA_DATA_R = 8; // Seta HIGH
            }
            else // Se sinal estiver HIGH
            {
                delay = pwmPercentage;
                GPIO_PORTA_DATA_R = 0; // Seta LOW
            }
        }
    }
    else
    {
        GPIO_PORTA_DATA_R = 0;
    }
    NVIC_ST_RELOAD_R = delay * time;
}

int getDisplayValue(int number)
{
    uint32_t value = 0;
    switch (number)
    {
    case 0:
        value = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5;
        break;
    case 1:
        value = 1 << 1 | 1 << 2;
        break;
    case 2:
        value = 1 << 0 | 1 << 1 | 1 << 3 | 1 << 4 | 1 << 6;
        break;
    case 3:
        value = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 6;
        break;
    case 4:
        value = 1 << 1 | 1 << 2 | 1 << 5 | 1 << 6;
        break;
    case 5:
        value = 1 << 0 | 1 << 2 | 1 << 3 | 1 << 5 | 1 << 6;
        break;
    case 6:
        value = 1 << 0 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6;
        break;
    case 7:
        value = 1 << 0 | 1 << 1 | 1 << 2;
        break;
    case 8:
        value = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5 | 1 << 6;
        break;
    case 9:
        value = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 5 | 1 << 6;
        break;
    }
    return value;
}

void showDisplay1(int decimal)
{
    GPIO_PORTB_DATA_R = getDisplayValue(decimal);
}
void showDisplay2(int unity)
{
    int value = getDisplayValue(unity);
    if (value & 64)
    {
        value ^= 64;

        GPIO_PORTC_DATA_R = 64;
    }
    else
    {
        GPIO_PORTC_DATA_R = 0;
    }
    GPIO_PORTE_DATA_R = value;
}

void checkButtons()
{
    //PORTF1
    if ((GPIO_PORTF_DATA_R & 2) == 0)
    {
        btn1 = 1;
        pwmPercentage -= 1;
        if (pwmPercentage < 1)
        {
            pwmPercentage = 99;
        }
        while (fixBug < 100)
        {
            if ((GPIO_PORTF_DATA_R & 2) != 0)
            {
                fixBug++;
            }
            else
            {
                fixBug = 0;
            }
        }
    }
    else
    {
        btn1 = 0;
    }
    //PORTF2
    if ((GPIO_PORTF_DATA_R & 4) == 0)
    {
        btn2 = 1;
        pwmPercentage += 1;
        if (pwmPercentage > 99)
        {
            pwmPercentage = 1;
        }
        while (fixBug < 100)
        {
            if ((GPIO_PORTF_DATA_R & 4) != 0)
            {
                fixBug++;
            }
            else
            {
                fixBug = 0;
            }
        }
    }
    else
    {
        btn2 = 0;
    }
    //PORTF3
    if ((GPIO_PORTF_DATA_R & 8) == 0)
    {
        motorDirection ^= 1;
        while (fixBug < 100)
        {
            if ((GPIO_PORTF_DATA_R & 8) != 0)
            {
                fixBug++;
            }
            else
            {
                fixBug = 0;
            }
        }
    }
    //PORTF4
    if ((GPIO_PORTF_DATA_R & 16) == 0)
    {
        motorSwitch ^= 1;
        while (fixBug < 100)
        {
            if ((GPIO_PORTF_DATA_R & 16) != 0)
            {
                fixBug++;
            }
            else
            {
                fixBug = 0;
            }
        }
    }

    fixBug = 0;
}

int main(void)
{
    uint32_t count; //declara variável count

    SYSCTL_RCGCGPIO_R = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 4 | 1 << 5; //habilita PORTA, PORTB, PORTC, PORTE e PORTF

    GPIO_PORTB_DATA_R = 0; //Zera data do PORTB
    GPIO_PORTC_DATA_R = 0; //Zera data do PORTC
    GPIO_PORTE_DATA_R = 0; //Zera data do PORTE

    GPIO_PORTB_DEN_R = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5
            | 1 << 6; //Digital Enable PORTB 0,1,2,3,4,5,6
    GPIO_PORTC_DEN_R = 1 << 6; //Digital Enable PORTC 6
    GPIO_PORTE_DEN_R = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5; //Digital Enable PORTE 0,1,2,3,4,5

    GPIO_PORTB_DIR_R = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5
            | 1 << 6; //Pull-up PORTC PORTB 0,1,2,3,4,5,6
    GPIO_PORTC_DIR_R = 1 << 6; //Pull-up PORTC PORTC 6
    GPIO_PORTE_DIR_R = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 1 << 5; //Pull-up PORTE PORTB 0,1,2,3,4,5

    GPIO_PORTA_DIR_R = 1 << 2 | 1 << 3; //Seta direção do PORTA 2 e 3 como alta
    GPIO_PORTA_DATA_R = 0; //Zera data do PORTA
    GPIO_PORTA_DEN_R = 1 << 2 | 1 << 3; //Digital Enable PORTA 2 e 3

    GPIO_PORTF_DATA_R = 0; //Zera data do PORTF
    GPIO_PORTF_DEN_R = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4; //Digital Enable PORTF 1, 2, 3 e 4
    GPIO_PORTF_PUR_R = 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4; //Pull-up PORTF 1, 2, 3 e 4

    NVIC_ST_RELOAD_R = 1; //ciclos p/ acionar Systick Timer
    NVIC_ST_CTRL_R = 3;

    while (1)
    {
        checkButtons();
        int unity = pwmPercentage % 10;
        int decimal = pwmPercentage / 10;

            showDisplay1(decimal);
            showDisplay2(unity);

    }

    return 0;
}
