/*----------------------------------------------------------------------------
 * Name:    Main.h
 * Purpose: Computation and User Interface main h file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		18/01/2018
 *----------------------------------------------------------------------------*/

// Complementary Functions
void init(struct planet PlanetArray[8]);
void sleepUntilTouch(void);
bool isTouchInside(int x1,int x2,int y1,int y2);
bool checkDate(int date[5]);  // YY,MM,DD,hh,mm

// Structure Functions
void DateSelection(struct planet PlanetArray[8],int date[5], float setAngles[8]);
void EngineeringMode(float* setAngles);
void ChangeDate(int date[5]);
void ChangeAngle(int p, float setAngles[8]);
bool SetAngles(float *); // Returns 1 if succesful
bool SetDemoMode(void); // Returns 1 if successful
