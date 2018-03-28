/*----------------------------------------------------------------------------
 * Name:    Main.h
 * Purpose: Computation and User Interface main h file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		18/01/2018
 *----------------------------------------------------------------------------*/


void init(struct planet PlanetArray[8]);
void sleepUntilTouch(void);
bool isTouchInside(int x1,int x2,int y1,int y2);

void DateSelection(struct planet PlanetArray[8],int date[5]);
void ChangeDate(int date[5]);
void SetAngles(float *);
void SetDemoMode(void);
bool checkDate(int date[5]);  // YY,MM,DD,hh,mm
