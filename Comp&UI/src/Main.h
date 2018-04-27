/*----------------------------------------------------------------------------
 * Name:    Main.h
 * Purpose: Computation and User Interface main h file
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		18/01/2018
 *----------------------------------------------------------------------------*/

// Complementary Functions
void sleepUntilTouch(void);
bool isTouchInside(int x1,int x2,int y1,int y2);
bool checkDate(int date[5]);  // YY,MM,DD,hh,mm
void bluetooth_ISR(void);
void init();
int min(int,int);

// Structure Functions
void DateSelection();
void EngineeringMode();
void RemoteServerControl();
void Settings();
void ChangeDate();
void ChangeAngle(int p);
void Keyboard(char* str);
bool SetAngles(float *,char identifier); // Returns 1 if succesful
bool SetDemoMode(void); // Returns 1 if successful
void I2CFailedInternet();
void I2CFailedMechatronics();
bool readCurrentDate();
void pollRemoteServer();
void nullISR();
void splashScreen();
