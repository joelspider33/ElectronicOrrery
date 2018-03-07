/*----------------------------------------------------------------------------
 * Name:    lcd.h
 * Purpose: Display Interface Driver Header File
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		17/01/2017
 *----------------------------------------------------------------------------*/

 #ifndef LCD_HEADER_FILE
 #define LCD_HEADER_FILE

#include "mbed.h"
#include "Arial_10pt.h"
#include "Arial_14pt.h"
#include "stdint.h"

#define arial_10pt &arial_10ptFontInfo
#define arial_14pt &arial_14ptFontInfo
#define Left 0
#define Center 1
#define Right 2

// 16-bit per pixel RGB Hex codes
#define Black           0x0000      /*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255,   0 */
#define White           0xFFFF      /* 255, 255, 255 */
#define Orange          0xFD20      /* 255, 165,   0 */
#define GreenYellow     0xAFE5      /* 173, 255,  47 */

// Defining coordinate structure for touch detection
typedef struct coord {
	int x;		// 0 < x < 240 (In default orientation)
	int y;		// 0 < y < 320
	int flag;	// Touch Event Flag
} COORD;

extern COORD pos;

typedef struct calib {
	int An,			/* A = An/Divider */
			Bn,     /* B = Bn/Divider */
			Cn,     /* C = Cn/Divider */
			Dn,     /* D = Dn/Divider */
			En,     /* E = En/Divider */
			Fn,     /* F = Fn/Divider */
			Divider;
}	Calibration_Matrix;



void setCalibrationMatrix(void);
COORD applyCalib(void);
// Credit for accurate calibration functions goes to Carlos E. Vidales, more information can be found at the link below
// https://www.embedded.com/design/system-integration/4023968/How-To-Calibrate-Touch-Screens

void enableTouchInterrupt(void);
void disableTouchInterrupt(void);
void readX(void);
void readY(void);
void PORTC_IRQHandler(void);
void lcdReset(void);
void lcdcmd(int8_t cmd);
void lcddat(int8_t data);
uint8_t lcdRead8(int32_t cmd);
uint32_t lcdRead32(int32_t cmd);

void lcdSetOrientation(int a);
void lcdSetMem(int x, int y, int w, int h);
void lcdClear(void);
void lcdSetPixel(int x,int y,int colour);
void lcdDrawLine(int x0, int y0, int x1, int y1, int colour);
void plotLineLow(int x0, int y0, int x1, int y1, int colour);
void plotLineHigh(int x0, int y0, int x1, int y1, int colour);
void lcdDrawRect(int x1, int y1, int x2, int y2, int colour, int fill);
void lcdDrawCircle(int x0,int y0,int r,int colour,int fill);
void lcdPrintString(int x, int y,char *string,const FONT_INFO* font, int colour, int position);
void lcdPrintChar(int x, int y, char c, int colour, const FONT_INFO* font);
uint8_t stringPixelLength(char* string, const FONT_INFO* font);

 #endif
