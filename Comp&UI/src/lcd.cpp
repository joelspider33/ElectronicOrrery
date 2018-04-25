/*----------------------------------------------------------------------------
 * Name:    lcd.c
 * Purpose: Display Interface Driver
 *----------------------------------------------------------------------------
 * Author: 	Joel W Webb
 * Date: 		17/01/2017
 *----------------------------------------------------------------------------*/

#include "lcd.h"


// ################  Register Definitions  ################
// For configuring Interrupt On PTC10 (Xpos of resistive touchscreen)
#define PORTC_PCR10          (*((volatile unsigned long *) 0x4004B028))
#define PORTC_ISFR           (*((volatile unsigned long *) 0x4004B0A0))
#define GPIOC_PDDR           (*((volatile unsigned long *) 0x400FF094))

// GLOBAL VARIABLES
COORD pos; 				// Described with origin at top left of display
COORD calibReported[3];			// A set of 3 coords representing read value of 3 points on the touchscreen
COORD calibTrue[3];					// A set of 3 coords representing what calibReported should have been in perfect case
COORD calibBlank[3] = {{100,100,0},{900,500,0},{500,900,0}};				// A set of 3 coords that provides setCalibrationMatrix with a Unity matrix
Calibration_Matrix matrix;	// Calibration matrix for resistive touchscreen
int height = 320; // Globals representing size of display, changes with orientation
int width = 240;

// API Declaration
SPI spi(PTD2,PTD3,PTD1,PTD0);
DigitalOut reset(PTC4);
InterruptIn Xpos(PTC10);


// ################  RESISTIVE TOUCHSCREEN FUNCTIONS  ################

void enableTouchInterrupt(void){
  Xpos.fall(&PORTC_IRQHandler);
  DigitalIn Xneg(PTB11,PullNone);
  DigitalIn Ypos(PTC11,PullNone);
  DigitalOut Yneg(PTB10);
  GPIOC_PDDR &= ~0x400;	// PTC10 as Input
  PORTC_PCR10 = 0xA0103;// PTC10 interrupt Falling Edge
  Yneg = 0;
}

void disableTouchInterrupt(void){
  DigitalIn Xneg(PTB11,PullNone);
  DigitalIn Ypos(PTC11,PullNone);
  DigitalIn Yneg(PTB10,PullNone);
  PORTC_PCR10 = 0;
  GPIOC_PDDR &= ~0x400;	// PTC10 as Input
}

void readX(void){
  DigitalOut Xpos(PTC10,1);
  DigitalOut Xneg(PTB11,0);
  AnalogIn Ypos(PTC11);
  DigitalIn Yneg(PTB10,PullNone);
  int temp=0;
  for(char i=0; i<32; i++){
    temp += Ypos.read_u16();
  }
  pos.x = temp/32;
}

void readY(void){
  DigitalIn Xpos(PTC10,PullNone);
  AnalogIn Xneg(PTB11);
  DigitalOut Ypos(PTC11,1);
  DigitalOut Yneg(PTB10,0);
  int temp=0;
  for(char i=0; i<32; i++){
    temp += Xneg.read_u16();
  }
  pos.y = temp/32;
}


void PORTC_IRQHandler(void){
	if(pos.flag==0 ){	// Skips if unhandled Touch Event is detected or interrupt triggered wrongly
		wait_ms(10);								// Debounce touch
    disableTouchInterrupt();
		readX();										// Read X position of touch
		readY();										// Read Y position of touch
		enableTouchInterrupt();			// Re-enable the interrupt
		pos.flag = 1;								// Set touch flag
    pos.x = pos.x/64; // Scale down 16 bit ints to 10 bit ints for use in calibration
    pos.y = pos.y/64;
    pos = applyCalib();
	}
	PORTC_ISFR = 0x400;	// Clear the interrupt flag
}
// ########## Touchscreen Calibration ##########

void setCalibrationMatrix(void){
    matrix.Divider = ((calibReported[0].x - calibReported[2].x) * (calibReported[1].y - calibReported[2].y)) -
                     ((calibReported[1].x - calibReported[2].x) * (calibReported[0].y - calibReported[2].y)) ;

		matrix.An = ((calibTrue[0].x - calibTrue[2].x) * (calibReported[1].y - calibReported[2].y)) -
								((calibTrue[1].x - calibTrue[2].x) * (calibReported[0].y - calibReported[2].y)) ;

		matrix.Bn = ((calibReported[0].x - calibReported[2].x) * (calibTrue[1].x - calibTrue[2].x)) -
								((calibTrue[0].x - calibTrue[2].x) * (calibReported[1].x - calibReported[2].x)) ;

		matrix.Cn = (calibReported[2].x * calibTrue[1].x - calibReported[1].x * calibTrue[2].x) * calibReported[0].y +
								(calibReported[0].x * calibTrue[2].x - calibReported[2].x * calibTrue[0].x) * calibReported[1].y +
								(calibReported[1].x * calibTrue[0].x - calibReported[0].x * calibTrue[1].x) * calibReported[2].y ;

		matrix.Dn = ((calibTrue[0].y - calibTrue[2].y) * (calibReported[1].y - calibReported[2].y)) -
								((calibTrue[1].y - calibTrue[2].y) * (calibReported[0].y - calibReported[2].y)) ;

		matrix.En = ((calibReported[0].x - calibReported[2].x) * (calibTrue[1].y - calibTrue[2].y)) -
								((calibTrue[0].y - calibTrue[2].y) * (calibReported[1].x - calibReported[2].x)) ;

		matrix.Fn = (calibReported[2].x * calibTrue[1].y - calibReported[1].x * calibTrue[2].y) * calibReported[0].y +
								(calibReported[0].x * calibTrue[2].y - calibReported[2].x * calibTrue[0].y) * calibReported[1].y +
								(calibReported[1].x * calibTrue[0].y - calibReported[0].x * calibTrue[1].y) * calibReported[2].y ;
}

void resetCalibrationMatrix(void){
		matrix.Divider = ((calibBlank[0].x - calibBlank[2].x) * (calibBlank[1].y - calibBlank[2].y)) -
                     ((calibBlank[1].x - calibBlank[2].x) * (calibBlank[0].y - calibBlank[2].y)) ;

		matrix.An = ((calibBlank[0].x - calibBlank[2].x) * (calibBlank[1].y - calibBlank[2].y)) -
								((calibBlank[1].x - calibBlank[2].x) * (calibBlank[0].y - calibBlank[2].y)) ;

		matrix.Bn = ((calibBlank[0].x - calibBlank[2].x) * (calibBlank[1].x - calibBlank[2].x)) -
								((calibBlank[0].x - calibBlank[2].x) * (calibBlank[1].x - calibBlank[2].x)) ;

		matrix.Cn = (calibBlank[2].x * calibBlank[1].x - calibBlank[1].x * calibBlank[2].x) * calibBlank[0].y +
								(calibBlank[0].x * calibBlank[2].x - calibBlank[2].x * calibBlank[0].x) * calibBlank[1].y +
								(calibBlank[1].x * calibBlank[0].x - calibBlank[0].x * calibBlank[1].x) * calibBlank[2].y ;

		matrix.Dn = ((calibBlank[0].y - calibBlank[2].y) * (calibBlank[1].y - calibBlank[2].y)) -
								((calibBlank[1].y - calibBlank[2].y) * (calibBlank[0].y - calibBlank[2].y)) ;

		matrix.En = ((calibBlank[0].x - calibBlank[2].x) * (calibBlank[1].y - calibBlank[2].y)) -
								((calibBlank[0].y - calibBlank[2].y) * (calibBlank[1].x - calibBlank[2].x)) ;

		matrix.Fn = (calibBlank[2].x * calibBlank[1].y - calibBlank[1].x * calibBlank[2].y) * calibBlank[0].y +
								(calibBlank[0].x * calibBlank[2].y - calibBlank[2].x * calibBlank[0].y) * calibBlank[1].y +
								(calibBlank[1].x * calibBlank[0].y - calibBlank[0].x * calibBlank[1].y) * calibBlank[2].y ;
}

COORD applyCalib(void){
		COORD temp;
		temp.x = ( (matrix.An * pos.x) + (matrix.Bn * pos.y) + matrix.Cn) / matrix.Divider ;
		temp.y = ( (matrix.Dn * pos.x) + (matrix.En * pos.y) + matrix.Fn) / matrix.Divider ;
		temp.flag = pos.flag;
		return temp;
}

void calibration(void){
	resetCalibrationMatrix();
  lcdClear();
  lcdPrintString(120,140,"Calibration",arial_14pt,White,1);
  lcdPrintString(120,160,"Please Press Dots",arial_10pt,White,1);
	lcdDrawCircle(36,48,5,Green,1);
  wait_ms(500);
  pos.flag = 0;
	while(pos.flag==0){
    sleep();
  }
	calibTrue[0].x = 36;
	calibTrue[0].y = 48;
	calibReported[0].x = pos.x;
	calibReported[0].y = pos.y;
	lcdDrawCircle(36,48,5,Black,1);
	lcdDrawCircle(204,160,5,Green,1);
  wait_ms(500);
  pos.flag = 0;
	while(pos.flag==0){
    sleep();
  }
	calibTrue[1].x = 204;
	calibTrue[1].y = 160;
	calibReported[1].x = pos.x;
	calibReported[1].y = pos.y;
	lcdDrawCircle(204,160,5,Black,1);
	lcdDrawCircle(120,272,5,Green,1);
  wait_ms(500);
  pos.flag = 0;
	while(pos.flag==0){
    sleep();
  }
	calibTrue[2].x = 120;
	calibTrue[2].y = 272;
	calibReported[2].x = pos.x;
	calibReported[2].y = pos.y;
	lcdDrawCircle(120,272,5,Black,1);
	setCalibrationMatrix();
  // Save calibration information to SD card
  pos.flag = 0;
}


// Reset and Initialisation code taken from MI0283QT datasheet
void lcdReset(void){
  // SPI Setup

  spi.format(9,0);					// 9 bit, Mode 0 (polarity 0, phase 0)
  spi.frequency(15000000);	// 10MHz Clock

  // Hardware Reset
  reset = 1;
  wait_us(15);
  reset = 0;
  wait_us(50);
  reset = 1;
  wait_ms(125);

	// Software Reset
	lcdcmd(0x01);		// Software reset command
	wait_ms(5);
	lcdcmd(0x28);		// Turn Display off for configuration

	// Configuring
	lcdcmd(0xCF);		// Power Control B
  lcddat(0x00);
  lcddat(0x83);
  lcddat(0x30);

  lcdcmd(0xED);		// Power on sequence control
  lcddat(0x64);
  lcddat(0x03);
  lcddat(0x12);
  lcddat(0x81);

  lcdcmd(0xE8);		// Driver Timing control A
  lcddat(0x85);
  lcddat(0x01);
  lcddat(0x79);

  lcdcmd(0xCB);		// Power Control A
  lcddat(0x39);
  lcddat(0x2C);
  lcddat(0x00);
  lcddat(0x34);
  lcddat(0x02);

  lcdcmd(0xF7);		// Pump ratio control
  lcddat(0x20);

  lcdcmd(0xEA);		// Driver timing control B
  lcddat(0x00);
  lcddat(0x00);

	lcdcmd(0xC0);		// Power Control 1
	lcddat(0x26);

	lcdcmd(0xC1);		// Power Control 2
  lcddat(0x11);

	lcdcmd(0xC5);		// VCOM Control 1
	lcddat(0x35);
	lcddat(0x3E);

	lcdcmd(0xC7);		// VCOM Control 2
  lcddat(0xBE);

	lcdcmd(0x36);		// Memory Access Control
	lcddat(0x48);

	lcdcmd(0x3A);		// Pixel format set
  lcddat(0x55);		// 16 bit/pixel

	lcdcmd(0xB1);		// Frame Control
	lcddat(0x00);
	lcddat(0x1B);		// Default 70Hz

	lcdcmd(0xF2);		// 3 Gamma Function Disable
	lcddat(0x08);

	lcdcmd(0x26);		// Gamma set
	lcddat(0x01);		// Gamma set 4 gamma curve 01/02/04/08

	lcdcmd(0xE0);		// Positive gamma correction
	lcddat(0x1F);
	lcddat(0x1A);
	lcddat(0x18);
	lcddat(0x0A);
	lcddat(0x0F);
	lcddat(0x06);
	lcddat(0x45);
	lcddat(0x87);
	lcddat(0x32);
	lcddat(0x0A);
	lcddat(0x07);
	lcddat(0x02);
	lcddat(0x07);
	lcddat(0x05);
	lcddat(0x00);

	lcdcmd(0xE1);		// Negamma gamma correction
	lcddat(0x00);
	lcddat(0x25);
	lcddat(0x27);
	lcddat(0x05);
	lcddat(0x10);
	lcddat(0x09);
	lcddat(0x3A);
	lcddat(0x78);
	lcddat(0x4D);
	lcddat(0x05);
	lcddat(0x18);
	lcddat(0x0D);
	lcddat(0x38);
	lcddat(0x3A);
	lcddat(0x1F);

	lcdcmd(0x2A);		// Column set
	lcddat(0x00);
	lcddat(0x00);
	lcddat(0x00);
	lcddat(0xEF);

	lcdcmd(0x2B);		// Page address set
	lcddat(0x00);
	lcddat(0x00);
	lcddat(0x01);
	lcddat(0x3F);

	// lcdcmd(0x34);		//  Tearing effect off
	// lcdcmd(0x35);		//  Tearing effect on
	// lcdcmd(0xB4);		//  Display inversion
	// lcddat(0x00);

	lcdcmd(0xB7);		// Entry mode set
	lcddat(0x07);

	lcdcmd(0xB6);		// Display function control
	lcddat(0x0A);
	lcddat(0x82);
	lcddat(0x27);
	lcddat(0x00);

	lcdcmd(0x11);		// Sleep out
	wait_ms(100);
	lcdcmd(0x29);		// Display on
	wait_ms(100);

	lcdSetOrientation(0);
  enableTouchInterrupt();
}


void lcdcmd(int8_t cmd){
  spi.write(cmd);
}


void lcddat(int8_t data){
  spi.write(data|0x100);
}

uint8_t lcdRead8(int32_t cmd){
  spi.write(cmd&0xFF);  // Send command
  return spi.write(0x00)>>1;  // Dummy cycle to read 8 bits
}

uint32_t lcdRead32(int32_t cmd){
  int r=0;
  spi.format(10,0);
  spi.write(cmd<<1);
  spi.format(16,0);
  r = spi.write(0x0000) << 16;
  r |= spi.write(0x0000);
  spi.format(9,0);
  return r;
}

void lcdSetOrientation(int a){
    lcdcmd(0x36);						// Memory Access Control Register
		switch (a) {
        case 0:
            lcddat(0x48);		// Default
						width = 240;
						height = 320;
            break;
        case 1:
            lcddat(0x28);		// Default rotated anti-clockwise 90°
						width = 320;
						height = 240;
            break;
        case 2:
            lcddat(0x88);		// Default rotated 180°
						width = 240;
						height = 320;
            break;
        case 3:
            lcddat(0xE8);		// Default rotated anti-clockwise 270°
						width = 320;
						height = 240;
            break;
    }
}

// Function used to set writable area of LCD memory
void lcdSetMem(int x, int y, int w, int h){
	// 0 < x < 239
	// 0 < y < 319
	// 1 < w < 240
	// 1 < h < 320
	/*
		   x x+w-1
			 |  |
		   V  V
		----------
		|	 |--|	 |<--y
		|	 |**|	 |
		|	 |--|	 |<--y+h-1
		|				 |
	  ----------

	** = writable memory in sequence
	*/
	lcdcmd(0x2A);		// Set column addresses
	lcddat(x>>8);
	lcddat(x&0xFF);
	lcddat((x+w-1)>>8);
	lcddat((x+w-1)&0xFF);

	lcdcmd(0x2B);		// Set page/row addresses
	lcddat(y>>8);
	lcddat(y&0xFF);
	lcddat((y+h-1)>>8);
	lcddat((y+h-1)&0xFF);
}

void lcdClear(void){
	lcdDrawRect(0,0,width-1,height-1,Black,1);
}

void lcdSetPixel(int x,int y,int colour){
	lcdSetMem(x,y,1,1);
	lcdcmd(0x2C);
	lcddat(colour>>8);
	lcddat(colour&0xFF);
}

void lcdDrawLine(int x0, int y0, int x1, int y1, int colour){

	// Vertical Line
	if(x0==x1){
		int h;
		if(y0>y1){
			h=y0;
			y0=y1;
			y1=h;
		}
		h = y1-y0+1;
		lcdSetMem(x0,y0,1,h);
		for(int y=y0; y<(h+y0); y++){
			lcdSetPixel(x0,y,colour);
		}
	}

	// Horizontal Line
	else if(y0==y1){
		int w;
		if(x0>x1){
			w=x0;
			x0=x1;
			x1=w;
		}
		w = x1-x0+1;
		lcdSetMem(x0,y0,w,1);
		for(int x=x0; x<(w+x0); x++){
			lcdSetPixel(x,y0,colour);
		}
	}

	// Diagonal Line
	// Using Bresenham's line algorithm for simplicity
	else{
		if (abs(y1 - y0) < abs(x1 - x0)){
			if (x0 > x1){
				plotLineLow(x1, y1, x0, y0, colour);
			}else{
				plotLineLow(x0, y0, x1, y1, colour);
			}
		}else{
			if (y0 > y1){
				plotLineHigh(x1, y1, x0, y0, colour);
			}else{
				plotLineHigh(x0, y0, x1, y1, colour);
			}
		}

	}
}

void plotLineLow(int x0, int y0, int x1, int y1, int colour){
	int dx,dy,x,y,yi,D;
	dx = x1 - x0;
	dy = y1 - y0;
	yi = 1;
	if (dy < 0){
		yi = -1;
		dy = -dy;
	}
	D = 2*dy - dx;
	y = y0;
	for (x=x0; x<(x1+1); x++){
		lcdSetPixel(x,y,colour);
		if (D > 0){
			y = y + yi;
			D = D - 2*dx;
		}
		D = D + 2*dy;
	}
}

void plotLineHigh(int x0, int y0, int x1, int y1, int colour){
	int dx,dy,x,y,xi,D;
	dx = x1 - x0;
	dy = y1 - y0;
	xi = 1;
	if (dx < 0){
		xi = -1;
		dx = -dx;
	}
	D = 2*dx - dy;
	x = x0;
	for (y=y0; y<(y1+1); y++){
		lcdSetPixel(x,y,colour);
		if (D > 0){
			x = x + xi;
			D = D - 2*dy;
		}
		D = D + 2*dy;
	}
}


void lcdDrawRect(int x0, int y0, int x1, int y1, int colour, int fill){

	int h,w;
	if(x0>x1){	// Setup so x1>x0 and y1>y0
		h=x0;
		x0=x1;
		x1=h;
	}
	if(y0>y1){
		h=y0;
		y0=y1;
		y1=h;
	}

	if(fill){		// If fill is selected then all pixels within box are drawn
		h = y1 - y0 + 1;
    w = x1 - x0 + 1;

    char buffer[16];
    for(unsigned char i=0; i<16; i+=4){
      buffer[i] = colour>>8;
      buffer[i+1] = 0x01;
      buffer[i+2] = colour&0xFF;
      buffer[i+3] = 0x01;
    }

    lcdSetMem(x0,y0,w,h);
    lcdcmd(0x2C);
    for (int a=0; a<=(w*h-4); a+=4){
      spi.write(buffer,16,0,0);
    }
    for (int a=0; a<w*h%4; a++){
      lcddat(colour>>8);
      lcddat(colour&0xFF);
    }
  } else{			// If fill is 0 the just outside box is drawn
		lcdDrawLine(x0,y0,x1,y0,colour);
		lcdDrawLine(x0,y0,x0,y1,colour);
		lcdDrawLine(x1,y0,x1,y1,colour);
		lcdDrawLine(x0,y1,x1,y1,colour);
	}
}

void lcdDrawCircle(int x0,int y0,int r,int colour,int fill){
		int x = r-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (r << 1); // r<<1 bascially multiplies r by 2 to get the diameter

    while (x >= y)
    {
			if(fill){
				lcdDrawLine(x0+x,y0+y,x0-x,y0+y, colour);
				lcdDrawLine(x0+x,y0-y,x0-x,y0-y, colour);
				lcdDrawLine(x0+y,y0+x,x0+y,y0-x, colour);
				lcdDrawLine(x0-y,y0+x,x0-y,y0-x, colour);
			} else{
        lcdSetPixel(x0 + x, y0 + y, colour);	// First Octant
        lcdSetPixel(x0 + y, y0 + x, colour);	// Mirrored Octants
        lcdSetPixel(x0 - y, y0 + x, colour);
        lcdSetPixel(x0 - x, y0 + y, colour);
        lcdSetPixel(x0 - x, y0 - y, colour);
        lcdSetPixel(x0 - y, y0 - x, colour);
        lcdSetPixel(x0 + y, y0 - x, colour);
        lcdSetPixel(x0 + x, y0 - y, colour);
			}
        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        else
        {
            x--;
            dx += 2;
            err += dx - (r << 1);
        }
    }
}

/* lcdPrintString function: Prints the Input String
int x                   - X-coordinate
int y                   - Y-coordinate
char* string            - Input String
const FONT_INFO* font   - Font type (Choose from defined font types)
int colour              - 16-bit Colour
int position            - 0-Left, 1-Center, 2-Right
*/
void lcdPrintString(int x, int y,const char *string, const FONT_INFO* font, int colour, int position){
	uint8_t i = 0;												// String index
  if(position == Center){ // If Left then no need to alter x, if Centre then x=x-l/2, if Right then x=x-l
    uint8_t l = stringPixelLength(string,font);
    x = x - l/2;
    y = y-(font->heightBits/2);
  } else if (position == Right){         // If Positition is to Center then centre on y
    uint8_t l = stringPixelLength(string,font);
    x = x - l;
  }
	while(string[i]!='\0'){								// Loops until null character (end of string) is detected
		if(string[i] == 0x20){							// If space character then proceed number of spaces
			x+=font->spacePixels;							// Proceeds number of space pixels
			i++;
		}else{
			lcdPrintChar(x, y, string[i] ,colour, font,0);			// Print character function
			x+=(font->charInfo[(string[i]-'!')].widthBits)+1;	// Proceed x by width+1 pixels
			i++;																							// Next character
		}
	}
}

void lcdPrintChar(int x, int y, char c, int colour, const FONT_INFO* font, int position){
  int Length = font->charInfo[c-'!'].widthBits;
  if (position==1){
    x-= (Length/2);
    y = y-(font->heightBits/2);
  } else if (position==2){
    x-= Length;
  }
	uint16_t offset = font->charInfo[c-'!'].offset;									// Get offset from font chracter info array
	uint8_t widthBytes = (int)((font->charInfo[c-'!'].widthBits-1)/8)+1;	// Find # of bytes in character
	for(uint8_t i=0; i<font->heightBits; i++){											// Loop from top to bottom of bitmap
		for(uint8_t j=0; j<font->charInfo[c-'!'].widthBits; j++){			// Loop from left to right of bitmap
			if( ( font->data[offset+(int)(j/8)+(i*widthBytes)]<<(j%8) ) & 0x80 ){		// If bitmap bit set then set pixel
				lcdSetPixel(x+j,y+i,colour);
			}
		}
	}
}

uint8_t stringPixelLength(const char* string, const FONT_INFO* font){
  uint8_t pixelLength = 0;
  for(uint8_t i=0; i<strlen(string); i++){
    if (string[i] == 0x20){ // If the character is a space then find width in font info
      pixelLength += font->spacePixels;
    }else{
      pixelLength += font->charInfo[string[i]-'!'].widthBits+1;
    }
  }
  return pixelLength;
}
