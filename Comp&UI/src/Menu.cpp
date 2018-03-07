/*----------------------------------------------------------------------------
 * Name:    Menu.cpp
 * Purpose: Menu cpp file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		07/03/2018
 *----------------------------------------------------------------------------*/

#include "Menu.h"



 uint8_t stateChange(uint8_t state){
 	/*	STATES
 	0 - Main Menu
 	1 - Date Selection
 	2 - Engineering Mode
 	3 - Wifi Settings
 	4 - Settings
 	5 - Change Date
 	6 - Change Angle
 	*/
 	switch (state){
 		case 0:	// Main menu
 			if			(pos.x>20 && pos.x<220 && pos.y>50  && pos.y<90) 	{			state = 1;			}		// Time&Date Selection Button
 			else if (pos.x>20 && pos.x<220 && pos.y>100 && pos.y<140)	{			state = 2;			}		// Engineering Mode Button
 			else if (pos.x>20 && pos.x<220 && pos.y>150 && pos.y<190)	{			state = 3;			}		// Wifi Settings Button
 			else if (pos.x>20 && pos.x<220 && pos.y>200 && pos.y<240)	{			state = 4;			}		// Settings Button
 			break;
 		case 1:	// Date selection
 			if			(pos.x>0 && pos.x<50 && pos.y>0 && pos.y<30)			{			state = 0;			}	// Back Button
 			else if	(pos.x>20 && pos.x<220 && pos.y>50 && pos.y<90)		{			state = 5;			}	// Change Date selection (Busy State)
 			break;
 		case 2:	// Engineering Mode
 			if			(pos.x>0 && pos.x<50 && pos.y>0 && pos.y<30)			{			state = 0;			} // Back Button
 			else if (pos.x>125 && pos.x<225 && pos.y>50 && pos.y<260)	{			state = 6;			}	// Change Angle Button (Busy State)
 			break;
 		case 3:	// Wifi Settings
 			if			(pos.x>0 && pos.x<50 && pos.y>0 && pos.y<30)			{			state = 0;			}	// Back Button
 			break;
 		case 4:	// Settings
 			if			(pos.x>0 && pos.x<50 && pos.y>0 && pos.y<30)			{			state = 0;			} // Back Button
 			break;
 	}
 	return state;
 }

 // State 0
 void Menu_StartScreen(){
 	lcdClear();
 	Menu_Topbar();
 	lcdPrintString(120,5,"Electronic Orrery",arial_14pt,Black,1);	// Title
 	lcdDrawRect(20,50,220,90,Green,1);													// Option 1 - Select Date
 	lcdPrintString(120,62,"Select Date",arial_14pt,Black,1);
 	lcdDrawRect(20,100,220,140,Green,1);												// Option 2 - Engineering Mode
 	lcdPrintString(120,112,"Engineering Mode",arial_14pt,Black,1);
 	lcdDrawRect(20,150,220,190,Green,1);												// Option 3 - Wifi Settings
 	lcdPrintString(120,165,"Wifi Settings",arial_14pt,Black,1);
 	lcdDrawRect(20,200,220,240,Green,1);												// Option 4 - Settings
 	lcdPrintString(120,212,"Settings",arial_14pt,Black,1);
 }

 // State 1
 void Menu_SelectDate(){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(50,5,"Date Selection",arial_14pt,Black,0);			// Title
 	lcdDrawRect(20,50,220,90,Green,1);													// Date Box
 	lcdDrawRect(20,100,220,300,LightGrey,1);										// Planet Stats Box
 	lcdPrintString(25,145,"Planet   Long    Lat   Dist",arial_10pt,Black,0);
 	lcdPrintString(25,155,"Mercury",arial_10pt,Black,0);					// Text
 	lcdPrintString(25,165,"Venus",arial_10pt,Black,0);
 	lcdPrintString(25,175,"Earth",arial_10pt,Black,0);
 	lcdPrintString(25,185,"Mars",arial_10pt,Black,0);
 	lcdPrintString(25,195,"Jupiter",arial_10pt,Black,0);
 	lcdPrintString(25,205,"Saturn",arial_10pt,Black,0);
 	lcdPrintString(25,215,"Uranus",arial_10pt,Black,0);
 	lcdPrintString(25,225,"Neptune",arial_10pt,Black,0);
 }

 // State 2
 void Menu_EngineeringMode(){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(40,5,"Engineering Mode",arial_14pt,Black,0);		// Title
 	lcdDrawRect(20,50,115,260,LightGrey,1);											// Planet Box
 	lcdDrawRect(125,50,220,260,Green,1);												// Angle Box
 	lcdDrawRect(20,270,220,310,Green,1);												// Demo Mode Button
 	lcdPrintString(25,60,"Planet",arial_14pt,Black,0);
 	lcdPrintString(25,80,"Mercury",arial_10pt,Black,0);						// Text
 	lcdPrintString(25,100,"Venus",arial_10pt,Black,0);
 	lcdPrintString(25,120,"Earth",arial_10pt,Black,0);
 	lcdPrintString(25,140,"Mars",arial_10pt,Black,0);
 	lcdPrintString(25,160,"Jupiter",arial_10pt,Black,0);
 	lcdPrintString(25,180,"Saturn",arial_10pt,Black,0);
 	lcdPrintString(25,200,"Uranus",arial_10pt,Black,0);
 	lcdPrintString(25,220,"Neptune",arial_10pt,Black,0);

 	lcdPrintString(80,280,"Demo Mode",arial_14pt,Black,0);
 }

 // State 3
 void Menu_WifiSettings(){

 }

 // State 4
 void Menu_Settings(){

 }

 // State 5
 void Menu_ChangeDate(){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(50,5,"Change Date",arial_14pt,Black,0);		// Title
 	Menu_Numpad();
 	uint8_t datenum = 0;
 	uint8_t loop = 1;
 	char dateFormat[] = "YYYY/MM/DD hh:mm";
 	char str[] = "_YYY/MM/DD hh:mm";
 	while(loop){
 		sleep();
 		if(pos.flag){
 			pos.flag = 0;
 			if(pos.x>20 && pos.x<80){
 				if			(pos.y>100 && pos.y<140){	str[datenum] = 7;	}
 				else if	(pos.y>150 && pos.y<190){	str[datenum] = 4;	}
 				else if (pos.y>200 && pos.y<240){	str[datenum] = 1;	}
 				else if (pos.y>250 && pos.y<300){
 					str[datenum] = dateFormat[datenum];
 					if(datenum == 5 || datenum == 8 || datenum == 11 || datenum == 14){
 						datenum-=2;
 					} else{
 						datenum--;
 					}
 					str[datenum] = '_';
 				}
 			}
 			if(pos.x>90 && pos.x<150){
 				if			(pos.y>100 && pos.y<140){	str[datenum] = 8;	}
 				else if	(pos.y>150 && pos.y<190){	str[datenum] = 5;	}
 				else if (pos.y>200 && pos.y<240){	str[datenum] = 2;	}
 				else if (pos.y>250 && pos.y<300){	str[datenum] = 0;	}
 			}
 			if(pos.x>160 && pos.x<220){
 				if			(pos.y>100 && pos.y<140){	str[datenum] = 9;	}
 				else if	(pos.y>150 && pos.y<190){	str[datenum] = 6;	}
 				else if (pos.y>200 && pos.y<240){	str[datenum] = 3;	}
 				else if (pos.y>250 && pos.y<300){
 					if(datenum == 15){	// Complete Date
 						char* endptr;
 						int YY = strtol(str,&endptr,10);		// Store Year
 						int MM = strtol(endptr,&endptr,10);	// Store Month
 						int DD = strtol(endptr,&endptr,10);	// Store Day
 						int hh = strtol(endptr,&endptr,10);	// Store Hour
 						int mm = strtol(endptr,&endptr,10);	// Store Minute
 						if(checkDate(YY,MM,DD,hh,mm)){
 							/* ############## WORKING HERE ############## */
 						}	else{	// Wrongly Formatted Date
 							lcdDrawRect(20,35,220,49,White,1);
 							lcdPrintString(20,35,"NOT VALID DATE",arial_10pt,Red,0);
 						}
 					} else{		// Incomplete Date
 						lcdDrawRect(20,35,220,49,White,1);
 						lcdPrintString(20,35,"INCOMPLETE DATE",arial_10pt,Red,0);
 					}
 				}
 			}
 			if(str[datenum]>('0'-1) && str[datenum]<('9'+1)){
 				if(datenum == 3 || datenum == 6 || datenum == 9 || datenum == 12){
 					datenum+=2;
 				} else if(datenum == 15){
 					// Do nothing if at end
 				} else{
 					datenum++;
 				}
 			}
 		}
 	}
 }

 // State 6
 void Menu_ChangeAngle(){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(50,5,"Change Angle",arial_14pt,Black,0);		// Title
 	lcdDrawRect(20,50,220,90,LightGrey,1);									// Date + Time
 	Menu_Numpad();
 }


 /* ---------- Bitmaps and Common Drawing Functions ----------*/
 void Menu_Topbar(){
 	lcdDrawRect(0,0,240,30,Blue,1);
 }

 void Menu_Wifi(int colour){
 	const uint16_t WifiBitmap[] =
 	{	8,13,		7,12,		8,12,		9,12,		8,11,		6,10,		7,9,		8,9,
 		9,9,		10,10,	4,8, 		5,7,		6,6, 		7,6,	 	8,6,	 	9,6,
 		10,6, 	11,7, 	12,8, 	2,6,	 	3,5, 		4,4, 		5,3, 		6,3,
 		7,3, 		8,3, 		9,3, 		10,3, 	11,3, 	12,4, 	13,5,		14,6,
 		0,4,		1,3, 		2,2, 		3,1, 		4,0, 		5,0, 		6,0, 		7,0,
 		8,0, 		9,0, 		10,0, 	11,0, 	12,0, 	13,1, 	14,2, 	15,3,
 		16,4};
 	for(int i=0; i<98; i+=2){
 		lcdSetPixel(WifiBitmap[i]+215,WifiBitmap[i+1]+7,colour);
 	}
 }

 void Menu_Data(int colour){
 	const uint16_t DataBitmap[] =
 	{	3,14, 	3,13, 	3,12, 	3,11, 	3,10, 	3,9, 		3,8, 		3,7,
 		3,6, 		3,5, 		3,4, 		0,3, 		1,3, 		2,3, 		3,3, 		4,3,
 		5,3, 		6,3, 		1,2, 		2,2, 		3,2, 		4,2, 		5,2, 		2,1,
 		3,1, 		4,1, 		3,0, 		9,0, 		9,1, 		9,2, 		9,3, 		9,4,
 		9,5, 		9,6, 		9,7, 		9,8, 		9,9, 		9,10, 	6,11, 	7,11,
 		8,11, 	9,11, 	10,11, 	11,11, 	12,11, 	7,12, 	8,12, 	9,12,
 		10,12, 	11,12, 	8,13, 	9,13, 	10,13, 	9,14};
 	for(int i=0; i<108; i+=2){
 		lcdSetPixel(DataBitmap[i]+195,DataBitmap[i+1]+7,colour);
 	}
 }

 void Menu_Back(){
 	lcdDrawRect(0,0,50,30,Green,1);
 	lcdPrintString(5,5,"Back",arial_10pt,White,0);
 }

 void Menu_Numpad(void){
 	lcdDrawRect(20,100,80,140,Green,1);
 	lcdDrawRect(90,100,150,140,Green,1);
 	lcdDrawRect(160,100,220,140,Green,1);
 	lcdDrawRect(20,150,80,190,Green,1);
 	lcdDrawRect(90,150,150,190,Green,1);
 	lcdDrawRect(160,150,220,190,Green,1);
 	lcdDrawRect(20,200,80,240,Green,1);
 	lcdDrawRect(90,200,150,240,Green,1);
 	lcdDrawRect(160,200,220,240,Green,1);
 	lcdDrawRect(20,250,80,300,Green,1);
 	lcdDrawRect(90,250,150,300,Green,1);
 	lcdDrawRect(160,250,220,300,Green,1);
 	lcdPrintChar(50,110,'7',Black,arial_14pt);
 	lcdPrintChar(120,110,'8',Black,arial_14pt);
 	lcdPrintChar(190,110,'9',Black,arial_14pt);
 	lcdPrintChar(50,160,'4',Black,arial_14pt);
 	lcdPrintChar(120,160,'5',Black,arial_14pt);
 	lcdPrintChar(190,160,'6',Black,arial_14pt);
 	lcdPrintChar(50,210,'1',Black,arial_14pt);
 	lcdPrintChar(120,210,'2',Black,arial_14pt);
 	lcdPrintChar(190,210,'3',Black,arial_14pt);
 	lcdPrintString(45,270,"<--",arial_14pt,Black,0);
 	lcdPrintChar(120,270,'0',Black,arial_14pt);
 	lcdPrintString(170,270,"Confirm",arial_10pt,Black,0);
 }

 // Date checking function
 uint8_t checkDate(int year, int month, int day, int hour, int minute){
 	if(year>=0 && year<=9999){
 		if(month>=1 && month<=12){
 			if		 ((day>0 && day<=31) && (month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)){}
 			else if((day>0 && day<=30) && (month==4 || month==6 || month==9 || month==11)){}
 			else if((day>0 && day<=28) && (month==2)){}
 			else if( day==29 && month==2 && (year%400==0 ||(year%4==0 && year%100!=0))){}	// Leap years every 4 years but not every 100 years except for every 400 years
 			else return 0;	// Not a valid date
 		}else	return 0; // Not a valid month
 	}else	return 0; // Not a valid year

 	if(hour>=0 && hour<=24){
 		if(minute>=0 && minute<60){
 			return 1;		// All conditions met, return success
 		}
 	}
 	return 0; // Not a valid hour
 }
