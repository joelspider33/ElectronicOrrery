/*----------------------------------------------------------------------------
 * Name:    Main.cpp
 * Purpose: Computation and User Interface Main cpp file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		20/12/2017
 *----------------------------------------------------------------------------*/

// Pre-Processor #INCLUDES
#include "Main.h"
#include "mbed.h"
#include "Menu.h"
#include "lcd.h"
#include "Planetary_Positioning.h"

// Pre-Processor #DEFINES
#define MechAddr 			0x09 // with r/w bit = 0)
#define InternetAddr 	0x10 // with r/w bit = 0)
#define CompUIAddr 		0x11 // with r/w bit = 0)

// API Declarations
I2C i2c(PTE25,PTE24);			// 100KHz default
Serial pc(USBTX, USBRX); 	// tx, rx
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



/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	pc.printf("Begin Main: ");
	// Initialisation
	errorLED(0);
	lcdReset();														// LCD Display Setup
	calibration();												// Allows user to Calibrate Touchscreen
	// Initialise Main Variables
	struct planet PlanetArray[8];	// Contains lon,lat and distance(r)
	int date[5] = {2000,1,1,0,0};						// YY,MM,DD,hh,mm
	init(PlanetArray);										// Set all variables to default values
	pc.printf("Initialisation Complete\n");

  while (1) {
		pc.printf("Start Main Loop\n");
		Menu_DrawMainMenu();
		sleepUntilTouch();
		if(isTouchInside(20,220,50,90)){
			DateSelection(PlanetArray,date);
		} else if(isTouchInside(20,220,100,140)){
			// Menu_EngineeringMode();
		} else if(isTouchInside(20,220,150,190)){
			Menu_RemoteServerControl();
		} else if(isTouchInside(20,220,200,240)){
			Menu_Settings();
		}
	}
}
/*----------------------------------------------------------------------------
  End of Main function
 *----------------------------------------------------------------------------*/

void init(struct planet PlanetArray[8]){
   getPlanetPos(PlanetArray,2000,1,1,0);	// Initialise Planet Positions to J2000.0 Epoch
}

void DateSelection(struct planet PlanetArray[8], int date[5]){
	Menu_DrawDateSelection(PlanetArray,date);
	pos.flag = 0;
	while(!isTouchInside(0,50,0,30)){
		if(isTouchInside(50,170,20,90)){
			ChangeDate(date);
			getPlanetPos(PlanetArray,date[0],date[1],date[2],(float)date[4] + (date[5]/24.0));
			Menu_DrawDateSelection(PlanetArray,date);
		} else if(isTouchInside(180,220,50,90)){
			float angles[8];
			for (int i=0;i<8;i++){
				angles[i] = PlanetArray[i].lon;
			}
			SetAngles(angles);
		}
		sleepUntilTouch();
	}
	return;
}

void ChangeDate(int date[5]){
	Menu_DrawChangeDate();
	uint8_t datenum = 0;
 	char dateFormat[] = "YYYY/MM/DD hh:mm";
 	char str[] = "_YYY/MM/DD hh:mm";
 	while(!isTouchInside(0,50,0,30)){
		lcdDrawRect(20,50,220,90,LightGrey,1);
		lcdPrintString(120,70,str,arial_10pt,White,1);
    sleepUntilTouch();
		if 			(isTouchInside(90,150,250,300)) {	str[datenum] = '0';}
		else if (isTouchInside(20,80,200,240))  {	str[datenum] = '1';}
		else if (isTouchInside(90,150,200,240)) {	str[datenum] = '2';}
		else if (isTouchInside(160,220,200,240)){	str[datenum] = '3';}
		else if	(isTouchInside(20,80,150,190))  {	str[datenum] = '4';}
		else if	(isTouchInside(90,150,150,190)) {	str[datenum] = '5';}
		else if	(isTouchInside(160,220,150,190)){	str[datenum] = '6';}
		else if	(isTouchInside(20,80,100,140))  {	str[datenum] = '7';}
		else if (isTouchInside(90,150,100,140)) {	str[datenum] = '8';}
		else if (isTouchInside(160,220,100,140)){	str[datenum] = '9';}
		// BackSpace
		else if (isTouchInside(20,80,250,300)){
			str[datenum] = dateFormat[datenum];
			if(datenum == 5 || datenum == 8 || datenum == 11 || datenum == 14){
				datenum-=2;
			} else if(datenum==0){
			} else{
				datenum--;
			}
			str[datenum]='_';
		}
		// Confirm
		else if (isTouchInside(160,220,250,300)){
			if(datenum == 15){	// Complete Date
				char* ptr;
        ptr = &str[0];
        int dateTemp[5];  // [YY,MM,DD,hh,mm]
        for(int i=0; i<5; i++){
					  dateTemp[i] = strtol(ptr,&ptr,10);		// Store Year
          ptr++;
        }
				if(checkDate(dateTemp)){
          lcdDrawRect(20,35,220,49,Black,1);
						lcdPrintString(20,35,"CORRECT DATE",arial_10pt,Green,0);
					for(int i=0;i<5;i++){
						date[i] = dateTemp[i];
					}
          return;
				}	else{	// Wrongly Formatted Date
					lcdDrawRect(20,35,220,49,Black,1);
					lcdPrintString(20,35,"NOT VALID DATE",arial_10pt,Red,0);
				}
			} else{		// Incomplete Date
				lcdDrawRect(20,35,220,49,Black,1);
				lcdPrintString(20,35,"INCOMPLETE DATE",arial_10pt,Red,0);
			}
		}
		// Move Pointer values
		if(str[datenum]>('0'-1) && str[datenum]<('9'+1)){
			if(datenum == 3 || datenum == 6 || datenum == 9 || datenum == 12){
				datenum+=2;
				str[datenum]='_';
			} else if(datenum == 15){
				// Do nothing if at end
			} else{
				datenum++;
				str[datenum]='_';
			}

		}
	}
}



// Takes array of 8 Floating point angles as input
void SetAngles(float* angles){
   lcdClear();
   Menu_Topbar();
   Menu_Data(Black);
   lcdPrintString(120,160,"Setting Angles",arial_14pt,White,1);
	 // Convert Floats to Bytes
   char bytearray[33];
	 bytearray[0] = 0xFF;  						// Set config packet for all 1's so all planets move
   uint8_t* p = (uint8_t *)&angles;	// Create pointer looking for Bytes with address at start of float array
   for(uint8_t i=1; i<33;i++){			// For all bytes 1-33
     bytearray[i] = p[i];						// Copy bytes over in order to bytearray
   }
   char failure = i2c.write(MechAddr<<1,bytearray,33);
   lcdClear();
   Menu_Topbar();
   if(failure){
     lcdPrintString(120,160,"Transmission Failed",arial_14pt,White,1);
     lcdPrintString(120,200,"Mechatronics Module Not Responding",arial_10pt,White,1);
     lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
     pos.flag = 0;
     while(pos.flag == 0){
       sleep();
     }
     pos.flag = 0;
   }
 }

 void SetDemoMode(void){
   lcdClear();
   Menu_Topbar();
   Menu_Data(Black);
   lcdPrintString(120,160,"Setting Demo Mode",arial_14pt,White,1);
   char bytearray[2];
   char failure = i2c.write(MechAddr<<1,bytearray,2);
   lcdClear();
   Menu_Topbar();
   if(failure){
     lcdPrintString(120,160,"Transmission Failed",arial_14pt,White,1);
     lcdPrintString(120,200,"Mechatronics Module Not Responding",arial_10pt,White,1);
     lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
     pos.flag = 0;
     while(pos.flag == 0){
       sleep();
     }
     pos.flag = 0;
   }
 }

void sleepUntilTouch(){
	pos.flag = 0;
	while(pos.flag==0){
		sleep();
	}
	pc.printf("Touch Position: X = %i , Y = %i\n",pos.x,pos.y);
}

bool isTouchInside(int x1,int x2,int y1,int y2){
	if(pos.x > x1 && pos.x < x2 && pos.y > y1 && pos.y < y2 && pos.flag){
		return 1;
		pos.flag = 0;
	} else{ return 0; }
}

// Date checking function
bool checkDate(int date[5]){
 if(date[0]>=0 && date[0]<=9999){
	 if(date[1]>=1 && date[1]<=12){
		 if		 ((date[2]>0 && date[2]<=31) && (date[1]==1 || date[1]==3 || date[1]==5 || date[1]==7 || date[1]==8 || date[1]==10 || date[1]==12)){}
		 else if((date[2]>0 && date[2]<=30) && (date[1]==4 || date[1]==6 || date[1]==9 || date[1]==11)){}
		 else if((date[2]>0 && date[2]<=28) && (date[1]==2)){}
		 else if( date[2]==29 && date[1]==2 && (date[0]%400==0 ||(date[0]%4==0 && date[0]%100!=0))){}	// Leap years every 4 years but not every 100 years except for every 400 years
		 else return 0;	// Not a valid date
	 }else	return 0; // Not a valid month
 }else	return 0; // Not a valid year

 if(date[3]>=0 && date[3]<=24){
	 if(date[4]>=0 && date[4]<60){
		 return 1;		// All conditions met, return success
	 }
 }
 return 0; // Not a valid hour
}
