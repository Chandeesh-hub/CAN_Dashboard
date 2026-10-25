/*
 * File:   matrix_keypad.c
 * Author: chandeesh
 *
 * Created on 18 October, 2026, 10:15 PM
 */


#include "matrix_keypad.h"

void matrix_config()
{
    TRISB = 0x1E;
    PORTB = PORTB | 0xE0;
    RBPU = 0;   // Enable PORTB pull-ups
}

unsigned char scankey()
{
    ROW1=0;
    ROW2=1;
    ROW3=1;
    
    if(COL1==0)
        return 1;
    
    ROW1=1;
    ROW2=0;
    ROW3=1;
    
    
    if(COL1==0)
        return 2;
    
    ROW1=1;
    ROW2=1;
    ROW3=0;
    ROW3=0;
    
    if(COL1==0)
        return 3;
    
    return 0xFF;
}

unsigned char readkey(char trigger)
{
    unsigned char key=scankey();
    
    if(trigger==LEVEL)
    {
        return key;
    }
    else if(trigger==STATE_CHANGE)
    {
        static char flag=0;
        if((key!=0xFF) && flag==0)
        {
            flag=1;
            return key;
        }
        else if(key==0xFF)
        {
            flag=0;
        }
    }
    return 0xFF;
}