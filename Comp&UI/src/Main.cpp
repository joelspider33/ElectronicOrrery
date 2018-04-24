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
I2C i2c(PTE25,PTE24);						// 100KHz default
Serial pc(USBTX, USBRX); 				// tx, rx
DigitalOut sun(PTC12);					// Sun toggle
DigitalOut powerLED(PTC3);			// Power LED
DigitalOut busyLED(PTC2);				// Busy LED
DigitalOut errorLED(PTA2);			// Error LED
Serial bluetooth(PTC17,PTC16);	// Bluetooth Serial Interrupt
Timeout timeout;

// Global Variables
struct planet PlanetArray[8];
int date[5];
float setAngles[8];
struct SSID_struct{
	char ID[32];
	char length;
	char encryption;
};


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	pc.printf("Hello World!\n");
	// Initialisation
	powerLED=1;
	busyLED=1;
	errorLED=0;
	lcdReset();														// LCD Display Setup
	splashScreen();
	calibration();												// Allows user to Calibrate Touchscreen
	bluetooth.attach(&bluetooth_ISR);
	// Initialise Variables
	init();
	pc.printf("Comp&UI Module Initialised\n");
	Menu_DrawMainMenu();

  while (1) {
		sleepUntilTouch();
		if 			 (isTouchInside(20,220,50,90)){
			DateSelection();
			Menu_DrawMainMenu();
		} else if(isTouchInside(20,220,100,140)){
			EngineeringMode();
			Menu_DrawMainMenu();
		} else if(isTouchInside(20,220,150,190)){
			RemoteServerControl();
			Menu_DrawMainMenu();
		} else if(isTouchInside(20,220,200,240)){
			Settings();
			Menu_DrawMainMenu();
		}
	}
}
/*----------------------------------------------------------------------------
  End of Main function
 *----------------------------------------------------------------------------*/

void init(){
	getPlanetPos(PlanetArray,2000,1,1,0);
	date[0] = 2000;
	date[1] = 1;
	date[2] = 1;
	date[3] = 0;
	date[4] = 0;
	for (int i=0;i<8;i++){
		setAngles[i] = PlanetArray[i].lon;
	}
	pos.flag=0;
}
void splashScreen(){
	lcdClear();
	lcdDrawCircle(120,160, 20, Yellow, 1);	// Sun
	for(int i=0; i<8; i++){
		lcdDrawCircle(120,160, 30+i*10, LightGrey, 0);	// Planet Orbits
	}
	lcdDrawCircle(150,160, 5, Maroon, 1);	// Mercury
	lcdDrawCircle(148,132, 7, Orange, 1);	// Venus
	lcdDrawCircle(120,110, 10, Green, 1);		// Earth
	lcdDrawCircle(78,118, 8, Red, 1);			// Mars
	lcdDrawCircle(50,160, 14, Magenta, 1);	// Jupiter
	lcdDrawCircle(63,217, 12, White, 1);		// Saturn
	lcdDrawCircle(63,217, 15, White, 0);	// Saturn Ring
	lcdDrawCircle(120,250, 8, Blue, 1);		// Uranus
	lcdDrawCircle(191,231, 7, Navy, 1);		// Neptune
	lcdPrintString(120,30,"ORRERY",arial_14pt,White,1);
	lcdPrintString(120,280,"Developed By:",arial_10pt,White,1);
	lcdPrintString(120,300,"J.Webb, J.Berriman, A.Patel",arial_10pt,White,1);
	sleepUntilTouch();
}

void DateSelection(){
	Menu_DrawDateSelection(PlanetArray,date);
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();
		if(isTouchInside(20,170,40,70)){
			ChangeDate();
			double time = (double)(date[4]) + ((double)(date[5])/24.0);
			getPlanetPos(PlanetArray,date[0],date[1],date[2],time);
			Menu_DrawDateSelection(PlanetArray,date);
		} else if(isTouchInside(180,220,40,70)){
			float angles[8];
			for (int i=0;i<8;i++){
				angles[i] = PlanetArray[i].lon;
			}
			if (SetAngles(angles,0xFF)){
				for (int i=0; i<8; i++){
					setAngles[i] = angles[i];
				}
			}
			Menu_DrawDateSelection(PlanetArray,date);
		} else if(isTouchInside(20,220,70,90)){
			readCurrentDate();
			Menu_DrawDateSelection(PlanetArray,date);
		}
	}
	return;
}
void EngineeringMode(){
	Menu_DrawEngineeringMode(setAngles);
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();
		for (int i=0; i<8; i++){
			if (isTouchInside(125,220,100+i*20,120+i*20)){
				ChangeAngle(i);
				SetAngles(setAngles,0x1<<i);
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

void RemoteServerControl(){
	Menu_DrawRemoteServerControl();
	char poll = 1;;
	char failure = i2c.write(InternetAddr<<1,&poll,1);
	if(failure){
		I2CFailedInternet();
		return;
	}
	char SSIDno;
	i2c.read(InternetAddr<<1,&SSIDno,1);	// Read length of SSID's
	struct SSID_struct SSID[20];
	for(int i=0;i<SSIDno;i++){
		i2c.read(InternetAddr<<1,SSID[i].ID,32);
		i2c.read(InternetAddr<<1,&SSID[i].length,1);
		i2c.read(InternetAddr<<1,&SSID[i].encryption,1);
	}
	char n=0;	// Scrolling index
	for(int i=n;i<n+7;i++){
		lcdPrintString(70,110+20*i,SSID[i].ID,arial_10pt,White,1);
		switch(SSID[i].encryption){
			case 1:
				lcdPrintString(170,110+20*i,"Open",arial_10pt,White,1);
				break;
			case 2:
				lcdPrintString(170,110+20*i,"WEP",arial_10pt,White,1);
				break;
			case 3:
				lcdPrintString(170,110+20*i,"WPA",arial_10pt,White,1);
				break;
			case 4:
				lcdPrintString(170,110+20*i,"WPA2",arial_10pt,White,1);
				break;
			default:
				lcdPrintString(170,110+20*i,"-",arial_10pt,White,1);
		}
	}
	lcdPrintString(120,290,"Awaiting Password",arial_10pt,White,1);
	Menu_UpArrow(220,100);
	Menu_DownArrow(220,240);
	char redraw = 0;
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();

		if(isTouchInside(220,240,100,120) && n+7<SSIDno){
			n++;
			redraw=1;
		}
		if(isTouchInside(220,240,240,260) && n>0){
			n--;
			redraw=1;
		}

		for(int i=0; i<8; i++){
			if (isTouchInside(20,220,100+i*20,120+i*20)){
				char password[32];	// Passwords up to 32 characters allowed
				Keyboard(password);
				Menu_DrawRemoteServerControl();
				lcdPrintString(120,290,"Checking Password",arial_10pt,White,1);
				i2c.write(InternetAddr<<1,SSID[i+n].ID,32);
				i2c.write(InternetAddr<<1,password,32);
				wait_us(5);
				i2c.read(InternetAddr<<1,&failure,1);
				char* ptr;
				if(!failure){	// 0x00 failure, 0x01 success
					lcdDrawRect(20,280,220,300,LightGrey,1);
					lcdPrintString(120,290,"Incorrect Password",arial_10pt,White,1);
				} else{	// If connection is succesful then continuously poll.
					lcdDrawRect(20,280,220,300,LightGrey,1);
					lcdPrintString(120,290,"Polling Remote Server",arial_10pt,White,1);
					while(!isTouchInside(0,50,0,30)){
						timeout.attach(&nullISR,0.5);
						sleep();
						poll = 0;
						lcdDrawRect(20,280,220,300,LightGrey,1);
						i2c.write(InternetAddr<<1,&poll,1);
						i2c.read(InternetAddr<<1,&poll,1);
						switch(poll){
							case 0:
								lcdPrintString(120,290,"Polling Remote Server",arial_10pt,White,1);
								break;
							case 1:
								lcdPrintString(120,290,"Server Input Date",arial_10pt,White,1);
								char datestr[16];
								i2c.read(InternetAddr,datestr,16);
								int dateTemp[5];  // [YY,MM,DD,hh,mm]
								ptr = &datestr[0];
								for(int i=0; i<5; i++){
									dateTemp[i] = strtol(ptr,&ptr,10);		// Store Date String as ints
									ptr++;
								}
								if(checkDate(dateTemp)){
									for(int i=0; i<5; i++){
										date[i] = dateTemp[i];
									}
									lcdDrawRect(20,280,220,300,LightGrey,1);
									lcdPrintString(120,290,"Converting Date to Planet Positions",arial_10pt,White,1);
									double time = (double)(date[4]) + ((double)(date[5])/24.0);
									getPlanetPos(PlanetArray,date[0],date[1],date[2],time);
									lcdDrawRect(20,280,220,300,LightGrey,1);
									lcdPrintString(120,290,"Setting Planet Positions",arial_10pt,White,1);
									float angles[8];
									for (int i=0;i<8;i++){
										angles[i] = PlanetArray[i].lon;
									}
									if (SetAngles(angles,0xFF)){
										for (int i=0; i<8; i++){
											setAngles[i] = angles[i];
										}
									}
								}
								lcdDrawRect(20,280,220,300,LightGrey,1);
								lcdPrintString(120,290,"Incorrect Date",arial_10pt,White,1);
								break;
							case 2:
								lcdPrintString(120,290,"Server Input Angles",arial_10pt,White,1);
								float angles[8];
								ptr = (char*)angles;
								i2c.read(InternetAddr<<1,ptr,32);
								failure = 0;
								for(int i =0; i<8; i++){
									if(angles[i]>=360 || angles[i]<0){
										failure = 1;
									}
								}
								if(!failure){
									if(SetAngles(angles,0xFF)){
										for(int i=0;i<8;i++){
											setAngles[i] = angles[i];
										}
										lcdDrawRect(20,280,220,300,LightGrey,1);
										lcdPrintString(120,290,"Succesfully Set Angles",arial_10pt,White,1);
									} else{
										lcdDrawRect(20,280,220,300,LightGrey,1);
										lcdPrintString(120,290,"Failed to Set Angles",arial_10pt,White,1);
									}
								} else{
									lcdDrawRect(20,280,220,300,LightGrey,1);
									lcdPrintString(120,290,"Set angles not within 0<x<360",arial_10pt,White,1);
								}
								break;
							case 3:
								lcdPrintString(120,290,"Server Input Demo Mode",arial_10pt,White,1);
								i2c.read(InternetAddr<<1,&poll,1);
								if (SetDemoMode()){
									lcdDrawRect(20,280,220,300,LightGrey,1);
									lcdPrintString(120,290,"Succesfully Set Demo",arial_10pt,White,1);
								} else{
									lcdDrawRect(20,280,220,300,LightGrey,1);
									lcdPrintString(120,290,"Failed to Set Demo",arial_10pt,White,1);
								}
								break;
						}
					}
				}
				redraw=1;
			}
		}

		if(redraw){
			redraw=0;
			for(int i=n;i<n+7;i++){
				lcdPrintString(70,110+20*i,SSID[i].ID,arial_10pt,White,1);
				switch(SSID[i].encryption){
					case 1:
						lcdPrintString(170,110+20*i,"Open",arial_10pt,White,1);
						break;
					case 2:
						lcdPrintString(170,110+20*i,"WEP",arial_10pt,White,1);
						break;
					case 3:
						lcdPrintString(170,110+20*i,"WPA",arial_10pt,White,1);
						break;
					case 4:
						lcdPrintString(170,110+20*i,"WPA2",arial_10pt,White,1);
						break;
					default:
						lcdPrintString(170,110+20*i,"-",arial_10pt,White,1);
				}
			}
		}
	}
}

void Settings(){
	Menu_DrawSettings();
	while(!isTouchInside(0,50,0,30)){
		sleepUntilTouch();
		if (isTouchInside(20,220,50,90)){	// Calibration Button
			calibration();
			Menu_DrawSettings();
		} else if (isTouchInside(20,220,100,140)){	// Toggle Sun Button
			sun = !sun;
			if(sun){
				lcdDrawRect(190,110,210,130,Yellow,1);
			}else{
				lcdDrawRect(190,110,210,130,Black,1);
			}
		} else if (isTouchInside(20,220,150,190)){	// Reset PLanets
			lcdClear();
			Menu_Topbar();
			lcdPrintString(120,160,"Resetting Planets",arial_14pt,White,1);
			init();
			lcdPrintString(120,200,"Please Align Planet Ring Channel Indicator",arial_10pt,White,1);
			lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
			sleepUntilTouch();
			Menu_DrawSettings();
		}
	}
}

void ChangeDate(){
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
void ChangeAngle(int p){
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
void Keyboard(char* str){
	Menu_DrawKeyboard();
	const char KeyboardABC[37] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
	const char Keyboardabc[37] = "1234567890qwertyuiopasdfghjklzxcvbnm";
	const char Keyboardsym[37] = "!\"#$%&\'()*+-,./\\:;<>=\?@[]^_`|{}~    ";
	const char* keyboard;
	keyboard = Keyboardabc;
	int num=0;
	char buffer[63];
	buffer[0] = '_';
	buffer[1] = '\0';
	while(!isTouchInside(0,50,0,30)){
		lcdDrawRect(30,90,220,110,LightGrey,1);
		lcdPrintString(30,90,buffer,arial_10pt,White,0);
		sleepUntilTouch();
		if(num<63){
			for(int i=0;i<10;i++){
				if (isTouchInside(20+20*i,40+20*i,150,170)){
					buffer[num] = keyboard[i];
					num++;
				} else if (isTouchInside(20+20*i,40+20*i,170,190)){
					buffer[num] = keyboard[i+10];
					num++;
				}
			}
			for(int i=0;i<9;i++){
				if(isTouchInside(30+20*i,50+20*i,190,210)){
					buffer[num] = keyboard[i+20];
					num++;
				}
			}
		}
		for(int i=0;i<7;i++){
			if(isTouchInside(50+20*i,70+20*i,210,230)){
				buffer[num] = keyboard[i+29];
				num++;
			}
		}
		if(isTouchInside(20,50,210,230)){	// Caps Lock
			if(keyboard==Keyboardabc){
				keyboard=KeyboardABC;
				DrawKeyboard(1);
			} else if(keyboard==KeyboardABC){
				keyboard=Keyboardabc;
				DrawKeyboard(0);
			}
		}
		if(isTouchInside(20,50,230,250)){	// Symbols
			if(keyboard==Keyboardsym){
				keyboard=Keyboardabc;
				DrawKeyboard(0);
			} else if (keyboard==Keyboardabc||keyboard==KeyboardABC){
				keyboard=Keyboardsym;
				DrawKeyboard(2);
			}
		}
		if(isTouchInside(190,220,210,230)){	// Backspace
			if(num>0){
				num--;
			}
		}
		if(isTouchInside(20,220,260,300) && num>0){	// Confirm Box
			for(int i=0;i<num;i++){
				str[i] = buffer[i];
			}
			return;
		}
		buffer[num]='_';
		buffer[num+1]='\0';
	}
}
bool SetAngles(float* angles,char identifier){
   lcdClear();
   Menu_Topbar();
   Menu_Data(Black);
   lcdPrintString(120,160,"Setting Angles",arial_14pt,White,1);
	 // Convert Floats to Bytes
   char bytearray[33];
	 bytearray[0] = identifier;  			// Set config packet so identified planets move
   uint8_t* p = (uint8_t *)angles;	// Create pointer looking for Bytes with address at start of float array
   for(uint8_t i=1; i<33; i++){			// For all bytes 1-33
     bytearray[i] = p[i-1];					// Copy bytes over in order to bytearray
   }
   char failure = i2c.write(MechAddr<<1,bytearray,33);
   lcdClear();
   Menu_Topbar();
   if(failure){
		 I2CFailedMechatronics();
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
		I2CFailedMechatronics();
		return 0;
	}
	return 1;
}
void sleepUntilTouch(){
	busyLED=0;
	pos.flag = 0;
	while(pos.flag==0){
		sleep();
	}
	busyLED=1;
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
bool readCurrentDate(){
	lcdClear();
	lcdPrintString(120,160,"Retrieving Date",arial_14pt,White,1);
	char bytearray[16] = {2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char failure = i2c.write(InternetAddr<<1,bytearray,1);
	if(failure){
		I2CFailedInternet();
		return 0;
	}
	failure = i2c.read(InternetAddr<<1,bytearray,16);
	if(failure){
		I2CFailedInternet();
		return 0;
	}
	char* ptr;
	ptr = &bytearray[0];
	int dateTemp[5];  // [YY,MM,DD,hh,mm]
	for(int i=0; i<5; i++){
		dateTemp[i] = strtol(ptr,&ptr,10);		// Store Date String as ints
		ptr++;
	}
	if(checkDate(dateTemp)){
		for(int i=0; i<5; i++){
			date[i] = dateTemp[i];
		}
		return 1;
	}
	return 0;
}
void I2CFailedInternet(){
	errorLED=1;
	lcdClear();
	Menu_Topbar();
	lcdPrintString(120,160,"Transmission Failed",arial_14pt,White,1);
	lcdPrintString(120,200,"Internet Module Not Responding",arial_10pt,White,1);
	lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
	sleepUntilTouch();
	errorLED=0;
	pos.flag=0;
}
void I2CFailedMechatronics(){
	errorLED=1;
	lcdClear();
	lcdPrintString(120,160,"Transmission Failed",arial_14pt,White,1);
	lcdPrintString(120,200,"Mechatronics Module Not Responding",arial_10pt,White,1);
	lcdPrintString(120,220,"Touch to continue",arial_10pt,White,1);
	sleepUntilTouch();
	errorLED=0;
	pos.flag=0;
}
void bluetooth_ISR(){}
void nullISR(){}
