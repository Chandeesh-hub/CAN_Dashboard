/*
 * File:   message_handler.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 11:01 AM
 */

#include <xc.h>
#include <string.h>
#include "message_handler.h"
#include "msg_id.h"
#include "can.h"
#include "clcd.h"

extern unsigned char flag;
uint16_t msg_id;
unsigned char data[8];
unsigned char len;


void handle_speed_data(uint8_t *data, uint8_t len)
{
    //Implement the speed function
    data[len]='\0';
    
    // Print the data in clcd
    clcd_print(data,LINE2(0));
}

void handle_gear_data(uint8_t *data, uint8_t len) 
{
    //Implement the gear function
    data[len]='\0';
    
    // Print the data in clcd
    clcd_print(data,LINE2(3));
}

void handle_rpm_data(uint8_t *data, uint8_t len) 
{
    //Implement the rpm function
    data[len]='\0';
    
    // Print the data in clcd
    clcd_print(data,LINE2(6));
}

void handle_engine_temp_data(uint8_t *data, uint8_t len) 
{
    //Implement the temperature function
}

void handle_indicator_data(uint8_t *data, uint8_t len) 
{
    //Implement the indicator function
    data[len]='\0';

    // check the data is '0'
    if(data[0]=='0')
    {
        // Turn off all indicator and print off in clcd
        RIGHT_IND_OFF();
        LEFT_IND_OFF();
        
        // set flag as 0
        flag=0;
        clcd_print("OFF",LINE2(11));
    }
    
    // check the data is '1'
    if(data[0]=='1')
    {
        // Turn off left indicator and print --> arrow in clcd
        LEFT_IND_OFF();
        
        // set flag as 1
        flag=1;
        clcd_print("-->",LINE2(11));
    }
    
    // check the data is '2'
    if(data[0]=='2')
    {
        // Turn off left indicator and print <-- arrow in clcd
        RIGHT_IND_OFF();
        
        // set flag as 2
        flag=2;
        clcd_print("<--",LINE2(11));
    }
}
    
void process_canbus_data() 
{   
    //process the CAN bus data
    
    // Receive the data from ECU1 and ECU2
    can_receive(&msg_id,data,&len);
    data[len]='\0';
    
    // Check message id is speed
    if(msg_id==SPEED_MSG_ID)
    {
        // call function speed to display speed
        handle_speed_data(data,len);
    }

    // Check message id is gear
    if(msg_id==GEAR_MSG_ID)
    {
        // call function gear to display gear
        handle_gear_data(data,len);
    }
    
    // Check message id is rpm
    if(msg_id==RPM_MSG_ID)
    {
        // call function rpm to display rpm
        handle_rpm_data(data,len);
    }
    
    // Check message id is indicator
    if(msg_id==INDICATOR_MSG_ID)
    {
        // call function indicator to display indicator
        handle_indicator_data(data,len);
    }
}