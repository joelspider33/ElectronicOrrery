/*----------------------------------------------------------------------------
 * Name:    Menu.cpp
 * Purpose: Menu cpp file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		07/03/2018
 *----------------------------------------------------------------------------*/

#include "Menu.h"
#include "lcd.h"
#include "stdint.h"
#include "Planetary_Positioning.h"


void Menu_DrawMainMenu(void){
  lcdClear();
  Menu_Topbar();
  lcdPrintString(120,15,"Electronic Orrery",arial_14pt,White,1);	// Title
  lcdDrawRect(20,50,220,90,DarkGreen,1);													// Option 1 - Select Date
  lcdPrintString(120,70,"Select Date",arial_14pt,White,1);
  lcdDrawRect(20,100,220,140,DarkGreen,1);												// Option 2 - Engineering Mode
  lcdPrintString(120,120,"Engineering Mode",arial_14pt,White,1);
  lcdDrawRect(20,150,220,190,DarkGreen,1);												// Option 3 - Wifi Settings
  lcdPrintString(120,170,"Wifi Settings",arial_14pt,White,1);
  lcdDrawRect(20,200,220,240,DarkGreen,1);												// Option 4 - Settings
  lcdPrintString(120,220,"Settings",arial_14pt,White,1);
}

void Menu_DrawDateSelection(struct planet PlanetArray[8], int date[5]){
  lcdClear();
  Menu_Topbar();
  Menu_Back();
  lcdPrintString(120,15,"Date Selection",arial_14pt,White,1);			// Title
  lcdDrawRect(20,50,170,90,DarkGreen,1);													// Date Box
  lcdDrawRect(180,50,220,90,DarkGreen,1);													// Set Box
  lcdPrintString(200,70,"Set",arial_14pt,White,1);                // Set Text
  lcdDrawRect(20,100,220,300,DarkGrey,1);							  // Planet Stats Box
  lcdPrintString(45,115,"Planet",arial_14pt,White,1);   // Titles
  lcdPrintString(95,115,"Long",arial_14pt,White,1);
  lcdPrintString(145,115,"Lat",arial_14pt,White,1);
  lcdPrintString(195,115,"Dist",arial_14pt,White,1);
  lcdPrintString(95,130,"deg",arial_10pt,White,1);
  lcdPrintString(145,130,"deg",arial_10pt,White,1);
  lcdPrintString(195,130,"AU",arial_10pt,White,1);
  lcdPrintString(45,150,"Mercury",arial_10pt,White,1);  // Text
  lcdPrintString(45,170,"Venus",arial_10pt,White,1);
  lcdPrintString(45,190,"Earth",arial_10pt,White,1);
  lcdPrintString(45,210,"Mars",arial_10pt,White,1);
  lcdPrintString(45,230,"Jupiter",arial_10pt,White,1);
  lcdPrintString(45,250,"Saturn",arial_10pt,White,1);
  lcdPrintString(45,270,"Uranus",arial_10pt,White,1);
  lcdPrintString(45,290,"Neptune",arial_10pt,White,1);
  // Print date
  char buffer[16];
  sprintf(buffer,"%04i/%02i/%02i %02i:%02i",date[0],date[1],date[2],date[3],date[4]);
  lcdPrintString(95,70,buffer,arial_10pt,White,1);
  // Print Planet Position Values
  for(int i=0; i<8; i++){
    char temp[6];
    sprintf(temp,"%.1f",PlanetArray[i].lon);
    lcdPrintString(95,150+i*20,temp,arial_10pt,White,1);
    sprintf(temp,"%.1f",PlanetArray[i].lat);
    lcdPrintString(145,150+i*20,temp,arial_10pt,White,1);
    sprintf(temp,"%.1f",PlanetArray[i].r);
    lcdPrintString(195,150+i*20,temp,arial_10pt,White,1);
  }
}

void Menu_DrawEngineeringMode(float setAngles[8]){
  lcdClear();
  Menu_Topbar();
  Menu_Back();
  lcdPrintString(130,15,"Engineering Mode",arial_14pt,White,1);		// Title
  lcdDrawRect(20,50,115,260,DarkGrey,1);											    // Planet Box
  lcdDrawRect(125,50,220,260,DarkGreen,1);												// Angle Box
  lcdDrawRect(20,270,115,310,DarkGreen,1);												// Demo Mode Button
  lcdDrawRect(125,270,220,310,DarkGreen,1);                       // Set Angle button
  lcdPrintString(70,75,"Planet",arial_14pt,White,1);              // Titles
  lcdPrintString(170,75,"Angle",arial_14pt,White,1);
  lcdPrintString(70,110,"Mercury",arial_10pt,White,1);
  lcdPrintString(70,130,"Venus",arial_10pt,White,1);
  lcdPrintString(70,150,"Earth",arial_10pt,White,1);
  lcdPrintString(70,170,"Mars",arial_10pt,White,1);
  lcdPrintString(70,190,"Jupiter",arial_10pt,White,1);
  lcdPrintString(70,210,"Saturn",arial_10pt,White,1);
  lcdPrintString(70,230,"Uranus",arial_10pt,White,1);
  lcdPrintString(70,250,"Neptune",arial_10pt,White,1);
  lcdPrintString(65,290,"Demo Mode",arial_14pt,White,1);
  lcdPrintString(175,290,"Set Angle",arial_14pt,White,1);
  char buffer[5];
  for (int i=0;i<8;i++){
    sprintf(buffer,"%.1f",setAngles[i]);
    lcdPrintString(170,110+i*20,buffer,arial_10pt,White,1);
  }
}

void Menu_DrawRemoteServerControl(void){
  lcdClear();
  Menu_Topbar();
  Menu_Back();
  lcdPrintString(130,15,"Remote Server Control",arial_10pt,White,1);		// Title
}

void Menu_DrawSettings(void){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(140,15,"Settings",arial_14pt,White,1);		// Title
 	lcdDrawRect(20,50,220,90,DarkGreen,1);								  // Calibration Button
 	lcdPrintString(120,70,"Calibrate",arial_14pt,White,1);   // Titles
  lcdDrawRect(20,100,220,140,DarkGreen,1);								  // Calibration Button
 	lcdPrintString(120,120,"Toggle Sun",arial_14pt,White,1);   // Titles
  lcdDrawRect(20,150,220,190,DarkGreen,1);								  // Calibration Button
 	lcdPrintString(120,170,"Reset Planets",arial_14pt,White,1);   // Titles
}

void Menu_DrawChangeDate(void){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(120,15,"Change Date",arial_14pt,White,1);		// Title
 	Menu_Numpad();
 }
void Menu_DrawChangeAngle(void){
 	lcdClear();
 	Menu_Topbar();
 	Menu_Back();
 	lcdPrintString(130,15,"Change Angle",arial_14pt,White,1);		// Title
 	lcdDrawRect(20,50,220,90,LightGrey,1);									// Date + Time
 	Menu_Numpad();
}
void Menu_DrawKeyboard(void){
  lcdClear();
  Menu_Topbar();
  Menu_Back();
  lcdPrintString(130,15,"Keyboard",arial_14pt,White,1);
  lcdDrawRect(20,50,220,140,LightGrey,1);
  DrawKeyboard(0);
  lcdDrawRect(20,260,220,300,DarkGreen,1);
  lcdPrintString(120,280,"Confirm",arial_14pt,White,1);
}

void DrawKeyboard(int type){ // 0-abc, 1-ABC, 2-Sym
  lcdDrawRect(20,150,220,250,Black,1);  // Clear Existing Keyboard
  lcdDrawRect(20,150,220,190,DarkGreen,1);  // Draw DarkGreen Backgrounds
  lcdDrawRect(30,190,210,210,DarkGreen,1);
  lcdDrawRect(20,210,220,230,DarkGreen,1);
  lcdDrawRect(20,230,50,250,DarkGreen,1);
  // Draw Key Outlines
  // Draw Horizontal Lines
  for (int i=0; i<5; i++){
    lcdDrawLine(20,150+i*20,220,150+20*i,LightGrey);
  }
  lcdDrawLine(20,250,50,250,LightGrey);
  // Draw Vertical Lines
  for (int i=0;i<11;i++){
    lcdDrawLine(20+i*20,150,20+i*20,190,LightGrey);
  }
  for (int i=0;i<8;i++){
    lcdDrawLine(50+i*20,190,50+i*20,230,LightGrey);
  }
  lcdDrawLine(30,190,30,210,LightGrey);
  lcdDrawLine(20,210,20,250,LightGrey);
  lcdDrawLine(50,230,50,250,LightGrey);
  lcdDrawLine(210,190,210,210,LightGrey);
  lcdDrawLine(220,210,220,230,LightGrey);
  // Print Characters
  const char KeyboardABC[37] = "1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
  const char Keyboardabc[37] = "1234567890qwertyuiopasdfghjklzxcvbnm";
  const char Keyboardsym[37] = "!\"#$%&\'()*+-,./\\:;<>=\?@[]^_`|{}~    ";
  const char* keyboard;
  switch(type){
    case 0:
      keyboard = Keyboardabc;
      break;
    case 1:
      keyboard = KeyboardABC;
      break;
    case 2:
      keyboard = Keyboardsym;
      break;
  }

  for (int i=0;i<10;i++){
    lcdPrintChar(30+i*20,160,keyboard[i],White,arial_10pt,1);
    lcdPrintChar(30+i*20,180,keyboard[i+10],White,arial_10pt,1);
  }
  for (int i=0;i<9;i++){
    lcdPrintChar(40+i*20,200,keyboard[i+20],White,arial_10pt,1);
  }
  for (int i=0;i<7;i++){
    lcdPrintChar(60+i*20,220,keyboard[i+29],White,arial_10pt,1);
  }
  Menu_Shift();
  lcdPrintString(205,220,"<-",arial_10pt,White,1);
  if (type==2){
    lcdPrintString(35,240,"Abc",arial_10pt,White,1);
  } else{
    lcdPrintString(35,240,"Sym",arial_10pt,White,1);
  }
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

void Menu_Shift(){
  const uint16_t ShiftBitmap[] =
  { 0,5 ,0,6,
    1,4 ,1,7,
    2,3 ,2,8,
    3,2 ,3,9,
    4,1 ,4,10,
    5,0 ,5,1 ,5,2 ,5,3 ,5,8 ,5,9 ,5,10 ,5,11,
    6,3 ,6,8,
    7,3 ,7,8,
    8,3 ,8,8,
    9,3 ,9,8,
    10,3 ,10,8,
    11,3 ,11,4 ,11,5 ,11,6 ,11,7 ,11,8};
  for (int i=0; i<68; i+=2){
    lcdSetPixel(ShiftBitmap[i+1]+30,ShiftBitmap[i]+215,White);
  }
}

void Menu_Back(){
	lcdDrawRect(0,0,50,30,DarkGreen,1);
	lcdPrintString(25,15,"Back",arial_10pt,White,1);
}

void Menu_Numpad(void){
 	lcdDrawRect(20,100,80,140,DarkGreen,1);
 	lcdDrawRect(90,100,150,140,DarkGreen,1);
 	lcdDrawRect(160,100,220,140,DarkGreen,1);
 	lcdDrawRect(20,150,80,190,DarkGreen,1);
 	lcdDrawRect(90,150,150,190,DarkGreen,1);
 	lcdDrawRect(160,150,220,190,DarkGreen,1);
 	lcdDrawRect(20,200,80,240,DarkGreen,1);
 	lcdDrawRect(90,200,150,240,DarkGreen,1);
 	lcdDrawRect(160,200,220,240,DarkGreen,1);
 	lcdDrawRect(20,250,80,300,DarkGreen,1);
 	lcdDrawRect(90,250,150,300,DarkGreen,1);
 	lcdDrawRect(160,250,220,300,DarkGreen,1);
 	lcdPrintString(50,120,"7",arial_14pt,White,1);
 	lcdPrintString(120,120,"8",arial_14pt,White,1);
 	lcdPrintString(190,120,"9",arial_14pt,White,1);
 	lcdPrintString(50,170,"4",arial_14pt,White,1);
 	lcdPrintString(120,170,"5",arial_14pt,White,1);
 	lcdPrintString(190,170,"6",arial_14pt,White,1);
 	lcdPrintString(50,220,"1",arial_14pt,White,1);
 	lcdPrintString(120,220,"2",arial_14pt,White,1);
 	lcdPrintString(190,220,"3",arial_14pt,White,1);
 	lcdPrintString(50,275,"<--",arial_14pt,White,1);
 	lcdPrintString(120,275,"0",arial_14pt,White,1);
 	lcdPrintString(190,275,"Confirm",arial_10pt,White,1);
 }
