
/* 
 * File:   matrix_keypad.h
 * Author:  chandeesh
 * Comments:
 * Revision history: 
 */


#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> 

#define LEVEL					     	0
#define STATE_CHANGE			       	1

#define ROW1  RB5
#define ROW2  RB6
#define ROW3  RB7
#define COL1  RB1
#define COL2  RB2
#define COL3  RB3
#define COL4  RB4


void matrix_config();
unsigned char scankey();
unsigned char readkey(char );



#endif	/* XC_HEADER_TEMPLATE_H */

