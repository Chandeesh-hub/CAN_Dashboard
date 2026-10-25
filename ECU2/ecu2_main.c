/*
 * File:   ecu2_main.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 10:55 AM
 */

#include "ecu2_sensor.h"
#include "adc.h"
#include "can.h"
#include "msg_id.h"
#include "config.h"

// Declare the gobal variable
volatile IndicatorStatus prev_ind_status, cur_ind_status=e_ind_off;
volatile unsigned char led_state=0;
unsigned char ssd[10]={ZERO,ONE,TWO,THREE,FOUR,FIVE,SIX,SEVEN,EIGHT,NINE};

uint16_t rpm=0;
char flag=0;


int main()
{
    
    // COnfig led, ssd, adc, can, digitalkeypad
    led_config();
    ssd_config();
    init_adc();
    init_digital_keypad();
    init_can();
    
    // Set the indicator status as off
    prev_ind_status=e_ind_off;
    cur_ind_status=e_ind_off;
    
    //call the functions
    while(1)
    {
        // call the indicator function
        cur_ind_status=process_indicator();

        if(cur_ind_status==0x04)
        {
            cur_ind_status=prev_ind_status;
        }
        else
        {
            prev_ind_status=cur_ind_status;
        }
        
        // if indicator is off set flag as 0
        if(cur_ind_status==e_ind_off)
        {
            flag=0;
        }
        
        // If indicator is right set flag as 1 turn off left indicator
        else if(cur_ind_status==e_ind_right)
        {
            LEFT_IND_OFF();
            flag=1;
        }
        
        // If indicator is left set flag as 2 turn off right indicator
        else if(cur_ind_status==e_ind_left)
        {
            RIGHT_IND_OFF();
            flag=2;
        }
        
        // call Function to get rpm 
        rpm=get_rpm();
    }
}
