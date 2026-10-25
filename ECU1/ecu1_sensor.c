/*
 * File:   ecu1_sensor.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 10:36 AM
 */

#include "ecu1_sensor.h"
#include "clcd.h"
#include "adc.h"
#include "can.h"
#include "msg_id.h"
#include "matrix_keypad.h"

extern unsigned char gear;
extern unsigned char gear_array[9][4];

uint16_t get_speed()
{
    // Implement the speed function
    
    // Read the value from adc through channel 4
    uint16_t value=read_adc(SPEED_ADC_CHANNEL);
    
    // Convert the ADC value into speed between (0 - 99)
    value = (value/10.33);
    
    unsigned char speed[3];
    
    // Update the speed into the char array
    speed[0]=(value/10) + '0';
    speed[1]=(value%10) + '0';
    speed[2]='\0';
    
    
    // Transmit the message id, speed and no.of byte 
    can_transmit(SPEED_MSG_ID,speed,2);
    __delay_ms(80);
    
    /******
     * 
     * FOR LOOP BACK MODE
     * 
     * 
        uint16_t msg_id=0;
        unsigned char data[3];
        unsigned int len=0;
        can_receive(&msg_id,data,&len);
        data[len] = '\0';

        if(msg_id==SPEED_MSG_ID)
        {
            clcd_print("CAN",LINE1(3));
            clcd_print(data,LINE2(3));
        }
     * 
    ***********/
    
    return value;
}

unsigned char get_gear_pos()
{
    // Implement the gear function
    static unsigned char gear=0;
    
    // Get the key from switch
    unsigned char key=readkey(STATE_CHANGE);
    
    
    if(key==1)
    {
        // If key is 1 increment the gear untill to max_gear
        gear++;
        if(gear>6)
        {
            gear=MAX_GEAR;
        }
    }
    else if(key==2)
    {
        // If key is 2 decrement the gear untill to 0
        if(gear<1)
        {
            gear=0;
        }
        else
        {
            gear--;
        }
    }
    else if(key==3)
    {
        // If key is 3 collison happen
        gear=8;
    }
    
    
    // Transmit the message id, gear and no.of byte 
    can_transmit(GEAR_MSG_ID,gear_array[gear],4);
    __delay_ms(80);
    
    /******
     * 
     * FOR LOOP BACK MODE
     * 
    unsigned char gear1[4];
    uint16_t msg_id;
    unsigned char len=0;
    
    can_receive(&msg_id,gear1,&len);
    
    if(msg_id==GEAR_MSG_ID)
        clcd_print(gear1,LINE2(10));
     * 
    ************/
    
    
    return gear;
}