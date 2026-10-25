/*
 * File:   main.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 11:00 AM
 */

#include <xc.h>
#include <stdint.h>
#include "can.h"
#include "clcd.h"
#include "msg_id.h"
#include "message_handler.h"


static void init_leds() {
    
    TRISB = 0x08; // Set RB2 as output, RB3 as input, remaining as output
    PORTB = 0x00;
}

static void init_config(void) {
    
    // Initialize CLCD and CANBUS and LED
    init_clcd();
    init_leds();
    init_can();
    
    // Enable Interrupts
    PEIE = 1;
    GIE = 1;
}
unsigned char flag;

void main(void) 
{
    // Initialize peripherals
    init_config();
    
    // Display the heading in line1
    clcd_print("SD EV RPM  IND  ",LINE1(0));
    
    // Declare the variable
    unsigned int delay=0,led_state=0;   
    
    while (1) 
    {
            
        // Read CAN Bus data and handle it
        process_canbus_data();

        // check flag is 0 turn off all indicator
        if(flag==0)
        {
            RIGHT_IND_OFF();
            LEFT_IND_OFF();
        }
        
        // check flag is 1 blink the right indicator
        if(flag==1)
        {
            delay++;
            if(delay==25000)
            {
                delay=0;
                led_state=!led_state;
                if(led_state)
                {
                    RIGHT_IND_ON();
                }
                else
                {
                    RIGHT_IND_OFF();
                }
            }
        }
        
        // check flag is 2 blink the left indicator
        if(flag==2)
        {
            delay++;
            if(delay==25000)
            {
                delay=0;
                led_state=!led_state;
                if(led_state)
                {
                    LEFT_IND_ON();
                }
                else
                {
                    LEFT_IND_OFF();
                }
            }
        }
    }

    return;
}
