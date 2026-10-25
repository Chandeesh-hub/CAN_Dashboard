
/* 
 * File:   config.h
 * Author:  chandeesh
 * Comments:
 * Revision history: 
 */


#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> 

void led_config();
void ssd_config();
void display(unsigned char *);


// Set MACRO for 0 to  9 in SSD
#define ZERO 0xE7
#define ONE 0x21
#define TWO 0xCB
#define THREE 0x6B
#define FOUR 0x2D
#define FIVE 0x6E
#define SIX 0xEE
#define SEVEN 0x23
#define EIGHT 0xEF
#define NINE 0x6F

#endif	/* XC_HEADER_TEMPLATE_H */

