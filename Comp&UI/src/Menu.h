/*----------------------------------------------------------------------------
 * Name:    Menu.h
 * Purpose: Menu header file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		07/03/2018
 *----------------------------------------------------------------------------*/

#ifndef MENU_HEADER_FILE
#define MENU_HEADER_FILE

#include "stdint.h"
#include "lcd.h"

uint8_t stateChange(uint8_t);
uint8_t checkDate(int year, int month, int day, int hour, int minute);

// Menu Function States
void Menu_StartScreen(void);
void Menu_SelectDate(void);
void Menu_EngineeringMode(void);
void Menu_WifiSettings(void);
void Menu_Settings(void);
void Menu_ChangeDate(void);
void Menu_ChangeAngle(void);

// Menu Drawing Functions and bitmaps
void Menu_Topbar(void);
void Menu_Wifi(int);
void Menu_Data(int);
void Menu_Back(void);
void Menu_Numpad(void);

#endif
