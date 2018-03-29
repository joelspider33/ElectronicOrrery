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


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	// Initialisation
	errorLED(0);
	lcdReset();														// LCD Display Setup
	calibration();												// Allows user to Calibrate Touchscreen
	// Initialise Main Variables
	struct planet PlanetArray[8];					// Contains lon,lat and distance(r)
	int date[5] = {2000,1,1,0,0};					// YY,MM,DD,hh,mm
	init(PlanetArray);										// Set all variables to default values
	float setAngles[8];
	for (int i=0;i<8;i++){
		setAngles[i] = PlanetArray[i].lon;
	}

  while (1) {
		Menu_DrawMainMenu();
		sleepUntilTouch();
		if(isTouchInside(20,220,50,90)){
			DateSelection(PlanetArray,date,setAngles);
		} else if(isTouchInside(20,220,100,140)){
			EngineeringMode(setAngles);
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

void DateSelection(struct planet PlanetArray[8], int date[5], float setAngles[8]){
	Menu_DrawDateSelection(PlanetArray,date);
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();
		if(isTouchInside(50,170,20,90)){
			ChangeDate(date);
			double time = (double)(date[4]) + ((double)(date[5])/24.0);
			getPlanetPos(PlanetArray,date[0],date[1],date[2],time);
			Menu_DrawDateSelection(PlanetArray,date);
		} else if(isTouchInside(180,220,50,90)){
			float angles[8];
			for (int i=0;i<8;i++){
				angles[i] = PlanetArray[i].lon;
			}
			if (SetAngles(angles)){
				for (int i=0; i<8; i++){
					setAngles[i] = angles[i];
				}
			}
			Menu_DrawDateSelection(PlanetArray,date);
		}
	}
	return;
}
void EngineeringMode(float setAngles[8]){
	Menu_DrawEngineeringMode(setAngles);
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();
		for (int i=0; i<8; i++){
			if (isTouchInside(125,220,100+i*20,120+i*20)){
				ChangeAngle(i,setAngles);
				Menu_DrawEngineeringMode(setAngles);
				pos.flag=0;
				break;
			}
		}
		if(isTouchInside(20,220,270,310)){
			SetDemoMode();
			Menu_DrawEngineeringMode(setAngles);
		}
	}
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
					dateTemp[i] = strtol(ptr,&ptr,10);		// Store Date String as ints
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
void ChangeAngle(int p,float setAngles[8]){
	Menu_DrawChangeAngle();
	uint8_t num = 0;
 	char Format[] = "000.0";
 	char str[] = "_00.0";
 	while(!isTouchInside(0,50,0,30)){
		lcdDrawRect(20,50,220,90,LightGrey,1);
		lcdPrintString(120,70,str,arial_10pt,White,1);
    sleepUntilTouch();
		if 			(isTouchInside(90,150,250,300)) {	str[num] = '0'; num++;}
		else if (isTouchInside(20,80,200,240))  {	str[num] = '1'; num++;}
		else if (isTouchInside(90,150,200,240)) {	str[num] = '2'; num++;}
		else if (isTouchInside(160,220,200,240)){	str[num] = '3'; num++;}
		else if	(isTouchInside(20,80,150,190))  {	str[num] = '4'; num++;}
		else if	(isTouchInside(90,150,150,190)) {	str[num] = '5'; num++;}
		else if	(isTouchInside(160,220,150,190)){	str[num] = '6'; num++;}
		else if	(isTouchInside(20,80,100,140))  {	str[num] = '7'; num++;}
		else if (isTouchInside(90,150,100,140)) {	str[num] = '8'; num++;}
		else if (isTouchInside(160,220,100,140)){	str[num] = '9'; num++;}
		// BackSpace
		else if (isTouchInside(20,80,250,300)){
			str[num] = Format[num];
			if(num == 4){
				num-=2;
			} else if(num==0){
			} else{
				num--;
			}
			str[num]='_';
		}
		// Confirm
		else if (isTouchInside(160,220,250,300)){
			if(num<4){
				str[num] = Format[num];
			}
			float angle = atof(str);
			if(angle<360.0 && angle>=0.0){
				lcdDrawRect(20,35,220,49,Black,1);
				lcdPrintString(20,35,"Correct Angle",arial_10pt,Green,0);
				setAngles[p] = angle;
				return;
			} else{
				lcdDrawRect(20,35,220,49,Black,1);
				lcdPrintString(20,35,"Must Satisfy: 0<=angle<360",arial_10pt,Red,0);
			}
		}
		// Move Pointer values
		if(num>4){
			num=4;
		} else if (num == 3){
			num++;
			str[num] = '_';
		} else{
			str[num] = '_';
		}
	}
}

bool SetAngles(float* angles){
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
     sleepUntilTouch();
		 pos.flag=0;
		 return 0;
   }
	 return 1;
 }
bool SetDemoMode(void){
	lcdClear();
	Menu_Topbar();
	Menu_Data(Black);
	lcdPrintString(120,160,"Setting Demo Mode",arial_14pt,White,1);
	char bytearray[2] = {0,0};
	char failure = i2c.write(MechAddr<<1,bytearray,2);
	if(failure){
		lcdClear();
		Menu_Topbar();
		lcdPrintString(120,160,"Transmission Failed",arial_14pt,White,1);
		lcdPrintString(120,200,"Mechatronics Module Not Responding",arial_10pt,White,1);
		lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
		sleepUntilTouch();
		pos.flag=0;
		return 0;
	}
	return 1;
}

void sleepUntilTouch(){
	pos.flag = 0;
	while(pos.flag==0){
		sleep();
	}
}
bool isTouchInside(int x1,int x2,int y1,int y2){
	if(pos.x > x1 && pos.x < x2 && pos.y > y1 && pos.y < y2 && pos.flag){
		return 1;
		pos.flag = 0;
	} else{ return 0; }
}
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
