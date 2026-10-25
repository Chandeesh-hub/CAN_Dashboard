/*
 * File:   config.c
 * Author: chandeesh
 *
 * Created on 18 October, 2026, 9:31 PM
 */

#include "config.h"

void led_config()
{
    TRISB=TRISB & 0x3C;
    PORTB=0x00;
}

void ssd_config()
{
    TRISD=0x00;
    PORTD=0x00;
    
    TRISA=TRISA & 0xF0;
    PORTA=PORTA & 0xF0;
}

void display(unsigned char *digit)
{
    for(int i=0;i<4;i++)
    {
        PORTD=digit[i];
        PORTA=PORTA | (1<<i);
        
        for(int i=0;i<1000;i++);
    }
}