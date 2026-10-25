/*
 * File:   ecu2_sensor.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 10:55 AM
 */

#include "ecu2_sensor.h"
#include "adc.h"
#include "can.h"
#include "msg_id.h"
#include "config.h"

// Declare variable
#define _XTAL_FREQ 20000000
extern unsigned char ssd[10];
extern unsigned char flag;
extern volatile IndicatorStatus prev_ind_status, cur_ind_status;
unsigned int delay=0;

uint16_t get_rpm()
{
    //Implement the rpm function
    
    // Function call to get adc value from channel 4
    uint16_t rpm=read_adc(RPM_ADC_CHANNEL);
    
    // Convert the ADC value to rpm value (0 - 6000)
    rpm=(rpm/10.23)*60;
    
    // Update the rpm into index array as character
    unsigned char index[4]={(rpm/1000)+'0',((rpm/100)%10)+'0',((rpm/10)%10)+'0',(rpm%10)+'0'};
    

    // Transmit the message id, RPM and no.of byte 
    can_transmit(RPM_MSG_ID,index,4);
    __delay_ms(80);
    
    /***
     * 
     * FOR LOOP BACK MODE
     * 
    uint16_t msg_id;
    unsigned char data[5];
    unsigned char len=0; 
    can_receive(&msg_id,data,&len);
    data[4]='\0';

    unsigned char digit[4];
    if(msg_id==RPM_MSG_ID)
    {
        
        digit[0]=ssd[(data[0]-'0')];
        digit[1]=ssd[(data[1]-'0')];
        digit[2]=ssd[(data[2]-'0')];
        digit[3]=ssd[(data[3]-'0')];
        
        display(digit);
    }
     * 
    *******/
    
    return rpm;
    
}

uint16_t get_engine_temp()
{
    //Implement the engine temperature function
    
    uint16_t temp=read_adc(ENG_TEMP_ADC_CHANNEL);
    
    temp=(temp/10.23)*(500UL/1023);
    
    return temp;
}

IndicatorStatus process_indicator()
{
    
    // Set the index array as 0,1,2
    unsigned index[4]={'0','1','2'};
    index[3]='\0';
    
    
    // Transmit the message id, indicator and no.of byte 
    can_transmit(INDICATOR_MSG_ID,&index[flag],1);
    __delay_ms(80);
    
    /****
     * 
     * FOR LOOP BACK MODE
     * 
    uint16_t msg_id;
    unsigned char data[8];
    unsigned char len=0;
    
    can_receive(&msg_id,data,&len);
    data[len]='\0';
     * 
    *****/
    
    // check the flag if 0 turn off both indicator
    if(flag==0)
    {
        RIGHT_IND_OFF();
        LEFT_IND_OFF();
    }
    
    // Check the flag if 1 blink the right indicator
    if(flag==1)
    {
            delay++;
            if(delay==3)
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
    
        // Check the flag if 2 blink the left indicator
        if(flag==2)
        {
            delay++;
            if(delay==3)
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
   
    //Implement the indicator function
    unsigned char key=read_digital_keypad(STATE_CHANGE);
    
    // Check if the switch1 is pressed return left
    if(key==SWITCH1)
    {
        return e_ind_left;
    }
    
    // Check if the switch1 is pressed return off
    else if(key==SWITCH2)
    {
        return e_ind_off;
    }
    
    // Check if the switch3 is pressed return right
    else if(key==SWITCH3)
    {
        return e_ind_right;
    }
    
    // return 4 when all switch is released or switch 4 pressed
    return 0x04;
}