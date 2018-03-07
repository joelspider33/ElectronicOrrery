/*----------------------------------------------------------------------------
 * Name:    Main.h
 * Purpose: Computation and User Interface main h file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		18/01/2018
 *----------------------------------------------------------------------------*/

#ifndef MAIN_HEADER_FILE
#define MAIN_HEADER_FILE

#include <mbed.h>
#include "Math.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "lcd.h"
#include "Planetary_Positioning.h"
#include "Menu.h"

uint8_t I2CSendAngles(char *);

#endif
