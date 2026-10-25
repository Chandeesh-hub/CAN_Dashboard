/*
 * File:   ecu1_main.c
 * Author: chandeesh
 *
 * Created on 17 October, 2026, 10:36 AM
 */

#include "adc.h"
#include "can.h"
#include "ecu1_sensor.h"
#include "msg_id.h"
#include "matrix_keypad.h"

unsigned char gear;
unsigned char gear_array[9][4]={"ON ","NE ","G1 ","G2 ","G3 ","G4 ","G5 ","R ","CO "};

int main()
{
    // config matrixkeypad, adc, can
    matrix_config();
    init_adc();
    init_can();

    unsigned char gear_data=0;   
    uint16_t ispeed=0;

    //Call the functions
    while(1)
    {
        // function call to get speed
        ispeed=get_speed();

        
        // function call to get gear
        gear_data=get_gear_pos();

        if(gear_data==8)
        {
            gear=0;
        }
    }
}