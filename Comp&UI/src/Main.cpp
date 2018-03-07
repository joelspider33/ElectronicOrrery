/*----------------------------------------------------------------------------
 * Name:    Main.cpp
 * Purpose: Computation and User Interface Main cpp file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		20/12/2017
 *----------------------------------------------------------------------------*/


#include "Main.h"

// DEFINES
#define MechAddr 			0x12
#define InternetAddr 	0x14
#define CompUIAddr 		0x16

// API CLASSES
// I2C Interface Setup
I2C i2c(PTE25,PTE24);			// 100KHz default
Serial pc(USBTX,USBRX);

DigitalOut rled(LED1);
DigitalOut gled(LED2);
DigitalOut bled(LED3);
void errorLED(int x){
	switch (x){
		case 0:
			rled = 1;
			bled = 1;
			gled = 1;
			break;
		case 1:
			rled = 0;
			bled = 1;
			gled = 1;
			break;
		case 2:
			rled = 1;
			bled = 0;
			gled = 1;
			break;
		case 3:
			rled = 1;
			bled = 1;
			gled = 0;
			break;
	}
}

// Function Pointer to Draw States
void (*Menu[]) (void) =	{
 Menu_StartScreen,				// 0
 Menu_SelectDate,					// 1
 Menu_EngineeringMode,		// 2
 Menu_WifiSettings,				// 3
 Menu_Settings,						// 4
 Menu_ChangeDate,					// 5
 Menu_ChangeAngle					// 6
};

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	errorLED(0);
	// Initialisation
	lcdReset();								// LCD Display Setup
	char bytearray[33];		// Angle array
	PosComp(2000,1,01,0);
	format_I2C(bytearray);
	// I2CSendAngles((char*)bytearray);

	unsigned char state = 0;
	Menu_StartScreen();
	lcdPrintString(120,280,"###### Test ######",arial_14pt,Black,1);
  while (1) {

		/*	MENU STRUCTURE
		During Menu Displays where user input will directly take you to another state, the MCU can poll Internet Module for information
		During Menu Displays where user input is interpreted such as Menu_ChangeAngle, Menu_ChangeDate and Wifi_Settings then polling is post-poned.
		*/

		sleep();
		// If a touch event is detected then call stateChange() function and if change occurs then update
		if(pos.flag){
			pos.flag = 0;
			uint8_t temp = state;
			state = stateChange(state);
			if(temp != state){
				(*Menu[state])();
			}

		}

		// Poll Internet Controller for information on every wake-up
		/* POLLING INTERNET CONTROLLER HERE */


	}
}



// I2CSendAngles takes bytearray as input and sends it to Mechatronics module
// Returns 0 if successful, non-0 if failed

uint8_t I2CSendAngles(char * bytearray){
	if(bytearray[0]){	// Rotate Planets by Angles
		return i2c.write(MechAddr,bytearray,33);
	} else{						// Demo Mode
		return i2c.write(MechAddr,bytearray,2);
	}
}
