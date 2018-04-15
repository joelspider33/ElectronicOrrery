/*----------------------------------------------------------------------------
 * Name:    Menu.h
 * Purpose: Menu header file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		07/03/2018
 *----------------------------------------------------------------------------*/

// Menu Function States
void Menu_DrawMainMenu(void);

void Menu_DrawDateSelection(struct planet PlanetArray[8],int date[5]);
void Menu_DrawChangeDate(void);

void Menu_DrawEngineeringMode(float setAngles[8]);
void Menu_DrawChangeAngle(void);

void Menu_DrawRemoteServerControl(void);
void Menu_Ethernet(void);
void Menu_Wifi(void);
void Menu_SSIDSelection(void);
void Menu_PasswordEntry(void);
void Menu_DrawKeyboard(void);
void DrawKeyboard(int type);  // 0-abc, 1-ABC, 2-Symbols

void Menu_DrawSettings(void);


// Menu Drawing Functions and bitmaps
void Menu_Topbar(void);
void Menu_Wifi(int);
void Menu_Data(int);
void Menu_Shift(void);
void Menu_UpArrow(int x, int y);
void Menu_DownArrow(int x, int y);
void Menu_Back(void);
void Menu_Numpad(void);
