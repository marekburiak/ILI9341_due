// UTouch_Calibration 
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// This program can be used to calibrate the touchscreen
// of the display modules.
// This program requires the UTFT library and a touch
// screen module that is compatible with UTFT.
//
// It is assumed that the display module is connected to an
// appropriate shield or that you know how to change the pin 
// numbers in the setup.
//
// Instructions will be given on the display.
//
#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "SystemFont5x7.h"

#include <URTouch.h>


// Define the orientation of the touch screen. Further 
// information can be found in the instructions.
#define TOUCH_ORIENTATION  PORTRAIT

// Initialize display
// ------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : <display model>,19,18,17,16
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

// Initialize touchscreen
// ----------------------
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino Uno/2009 Shield            : 15,10,14, 9, 8
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
// CTE TFT LCD/SD Shield for Arduino Due       :  6, 5, 4, 3, 2
// Teensy 3.x TFT Test Board                   : 26,31,27,28,29
// ElecHouse TFT LCD/SD Shield for Arduino Due : 25,26,27,29,30
//
URTouch  myTouch(30, 28, 26, 24, 22);

// ************************************
// DO NOT EDIT ANYTHING BELOW THIS LINE
// ************************************

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];

void setup()
{
	Serial.begin(9600);
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	tft.fillScreen(ILI9341_BLACK);
	tft.setFont(SystemFont5x7);

	myTouch.InitTouch(TOUCH_ORIENTATION);
	dispx = tft.width();
	dispy = tft.height();
	text_y_center = (dispy / 2) - 6;
}

void drawCrossHair(int x, int y, uint16_t color)
{
	tft.drawRect(x - 10, y - 10, 20, 20, color);
	tft.drawLine(x - 5, y, x + 5, y, color);
	tft.drawLine(x, y - 5, x, y + 5, color);
}

void readCoordinates()
{
	int iter = 5000;
	int failcount = 0;
	int cnt = 0;
	uint32_t tx = 0;
	uint32_t ty = 0;
	boolean OK = false;

	while (OK == false)
	{
		tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
		tft.printAligned("PRESS", gTextAlignMiddleCenter, 10, 10);
		while (myTouch.dataAvailable() == false) {}
		tft.printAligned("HOLD!", gTextAlignMiddleCenter);
		while ((myTouch.dataAvailable() == true) && (cnt < iter) && (failcount < 10000))
		{
			myTouch.calibrateRead();
			if (!((myTouch.TP_X == 65535) || (myTouch.TP_Y == 65535)))
			{
				tx += myTouch.TP_X;
				ty += myTouch.TP_Y;
				cnt++;
			}
			else
				failcount++;
		}
		if (cnt >= iter)
		{
			OK = true;
		}
		else
		{
			tx = 0;
			ty = 0;
			cnt = 0;
		}
		if (failcount >= 10000)
			fail();
	}

	cx = tx / iter;
	cy = ty / iter;
}

void calibrate(int x, int y, int i)
{
	drawCrossHair(x, y, ILI9341_WHITE);
	readCoordinates();
	tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
	tft.printAligned("RELEASE", gTextAlignMiddleCenter);
	drawCrossHair(x, y, ILI9341_DARKGRAY);
	rx[i] = cx;
	ry[i] = cy;
	while (myTouch.dataAvailable() == true) {}
}

void waitForTouch()
{
	while (myTouch.dataAvailable() == true) {}
	while (myTouch.dataAvailable() == false) {}
	while (myTouch.dataAvailable() == true) {}
}

void toHex(uint32_t num)
{
	buf[0] = '0';
	buf[1] = 'x';
	buf[10] = 'U';
	buf[11] = 'L';
	buf[12] = 0;
	for (int zz = 9; zz > 1; zz--)
	{
		if ((num & 0xF) > 9)
			buf[zz] = (num & 0xF) + 55;
		else
			buf[zz] = (num & 0xF) + 48;
		num = num >> 4;
	}
}

void startup()
{
	tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
	tft.fillRect(0, 0, dispx, 13, ILI9341_RED);
	tft.drawLine(0, 13, dispx - 1, 13, ILI9341_WHITE);
	tft.printAlignedOffseted("UTouch Calibration", gTextAlignTopCenter, 0, 2);

	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.printAlignedOffseted("INSTRUCTIONS", gTextAlignTopCenter, 0, 30);
	tft.printAlignedOffseted("Use a stylus or something similar to", gTextAlignTopCenter, 0, 50);
	tft.printAlignedOffseted("touch as close to the center of the", gTextAlignTopCenter, 0, 62);
	tft.printAlignedOffseted("highlighted crosshair as possible. Keep", gTextAlignTopCenter, 0, 74);
	tft.printAlignedOffseted("as still as possible and keep holding", gTextAlignTopCenter, 0, 86);
	tft.printAlignedOffseted("until the highlight is removed. Repeat", gTextAlignTopCenter, 0, 98);
	tft.printAlignedOffseted("for all crosshairs in sequence.", gTextAlignTopCenter, 0, 110);

	tft.printAlignedOffseted("Further instructions will be displayed", gTextAlignTopCenter, 0, 134);
	tft.printAlignedOffseted("when the calibration is complete.", gTextAlignTopCenter, 0, 146);

	tft.printAlignedOffseted("Do NOT use your finger as a calibration", gTextAlignTopCenter, 0, 170);
	tft.printAlignedOffseted("stylus or the result WILL BE imprecise.", gTextAlignTopCenter, 0, 182);

	tft.printAlignedOffseted("Touch screen to continue", gTextAlignTopCenter, 0, 226);

	waitForTouch();
	tft.fillScreen(ILI9341_BLACK);
}

void done()
{
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
	tft.fillRect(0, 0, dispx, 13, ILI9341_RED);
	tft.drawLine(0, 13, dispx - 1, 13, ILI9341_WHITE);
	tft.printAlignedOffseted("UTouch Calibration", gTextAlignTopCenter, 0, 2);

	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.printAlignedOffseted("CALIBRATION COMPLETE", gTextAlignTopCenter, 0, 30);
	tft.printAlignedOffseted("To use the new calibration", gTextAlignTopCenter, 0, 50);
	tft.printAlignedOffseted("settings you must edit the", gTextAlignTopCenter, 0, 62);
	tft.setTextColor(160, 160, 255);
	tft.printAlignedOffseted("UTouchCD.h", gTextAlignTopCenter, -47, 74);
	tft.setTextColor(ILI9341_WHITE);
	tft.printAlignedOffseted("file and change", gTextAlignTopCenter, 47, 74);
	tft.printAlignedOffseted("the following values.", gTextAlignTopCenter, 0, 86);
	tft.printAlignedOffseted("The values are located right", gTextAlignTopCenter, 0, 98);
	tft.printAlignedOffseted("below the opening comment in", gTextAlignTopCenter, 0, 110);
	tft.printAlignedOffseted("the file.", gTextAlignTopCenter, 0, 122);
	tft.printAlignedOffseted("CAL_X", gTextAlignTopCenter, -50, 150);
	tft.printAlignedOffseted("CAL_Y", gTextAlignTopCenter, -50, 162);
	tft.printAlignedOffseted("CAL_S", gTextAlignTopCenter, -50, 174);

	toHex(calx);
	tft.printAt(buf, 135, 150);
	Serial.print("#define CAL_X ");
	Serial.println(buf);
	toHex(caly);
	Serial.print("#define CAL_Y ");
	tft.printAt(buf, 135, 162);
	Serial.println(buf);
	toHex(cals);
	Serial.print("#define CAL_S ");
	tft.printAt(buf, 135, 174);
	Serial.println(buf);
}

void fail()
{
	tft.fillScreen(ILI9341_BLACK);
	tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
	tft.fillRect(0, 0, dispx, 13, ILI9341_RED);
	tft.drawLine(0, 13, dispx - 1, 13, ILI9341_WHITE);
	tft.printAlignedOffseted("UTouch Calibration FAILED", gTextAlignTopCenter, 0, 2);

	tft.printAlignedOffseted("Unable to read the position", gTextAlignTopCenter, 0, 30);
	tft.printAlignedOffseted("of the press. This is a", gTextAlignTopCenter, 0, 42);
	tft.printAlignedOffseted("hardware issue and can", gTextAlignTopCenter, 0, 54);
	tft.printAlignedOffseted("not be corrected in", gTextAlignTopCenter, 0, 66);
	tft.printAlignedOffseted("software.", gTextAlignTopCenter, 0, 78);

	while (true) {};
}

void loop()
{
	startup();

	drawCrossHair(dispx - 11, 10, ILI9341_DARKGRAY);
	drawCrossHair(dispx / 2, 10, ILI9341_DARKGRAY);
	drawCrossHair(10, 10, ILI9341_DARKGRAY);
	drawCrossHair(dispx - 11, dispy / 2, ILI9341_DARKGRAY);
	drawCrossHair(10, dispy / 2, ILI9341_DARKGRAY);
	drawCrossHair(dispx - 11, dispy - 11, ILI9341_DARKGRAY);
	drawCrossHair(dispx / 2, dispy - 11, ILI9341_DARKGRAY);
	drawCrossHair(10, dispy - 11, ILI9341_DARKGRAY);
	tft.setTextColor(ILI9341_WHITE, ILI9341_RED);
	tft.fillRect(120, 100, 80, 40, ILI9341_RED);
	tft.drawRect(120, 100, 80, 40, ILI9341_WHITE);

	calibrate(10, 10, 0);
	calibrate(10, dispy / 2, 1);
	calibrate(10, dispy - 11, 2);
	calibrate(dispx / 2, 10, 3);
	calibrate(dispx / 2, dispy - 11, 4);
	calibrate(dispx - 11, 10, 5);
	calibrate(dispx - 11, dispy / 2, 6);
	calibrate(dispx - 11, dispy - 11, 7);

	if (TOUCH_ORIENTATION == LANDSCAPE)
		cals = (long(dispx - 1) << 12) + (dispy - 1);
	else
		cals = (long(dispy - 1) << 12) + (dispx - 1);

	if (TOUCH_ORIENTATION == PORTRAIT)
		px = abs(((float(rx[2] + rx[4] + rx[7]) / 3) - (float(rx[0] + rx[3] + rx[5]) / 3)) / (dispy - 20));  // PORTRAIT
	else
		px = abs(((float(rx[5] + rx[6] + rx[7]) / 3) - (float(rx[0] + rx[1] + rx[2]) / 3)) / (dispy - 20));  // LANDSCAPE

	if (TOUCH_ORIENTATION == PORTRAIT)
	{
		clx = (((rx[0] + rx[3] + rx[5]) / 3));  // PORTRAIT
		crx = (((rx[2] + rx[4] + rx[7]) / 3));  // PORTRAIT
	}
	else
	{
		clx = (((rx[0] + rx[1] + rx[2]) / 3));  // LANDSCAPE
		crx = (((rx[5] + rx[6] + rx[7]) / 3));  // LANDSCAPE
	}
	if (clx < crx)
	{
		clx = clx - (px * 10);
		crx = crx + (px * 10);
	}
	else
	{
		clx = clx + (px * 10);
		crx = crx - (px * 10);
	}

	if (TOUCH_ORIENTATION == PORTRAIT)
		py = abs(((float(ry[5] + ry[6] + ry[7]) / 3) - (float(ry[0] + ry[1] + ry[2]) / 3)) / (dispx - 20));  // PORTRAIT
	else
		py = abs(((float(ry[0] + ry[3] + ry[5]) / 3) - (float(ry[2] + ry[4] + ry[7]) / 3)) / (dispx - 20));  // LANDSCAPE

	if (TOUCH_ORIENTATION == PORTRAIT)
	{
		cty = (((ry[5] + ry[6] + ry[7]) / 3));  // PORTRAIT
		cby = (((ry[0] + ry[1] + ry[2]) / 3));  // PORTRAIT
	}
	else
	{
		cty = (((ry[0] + ry[3] + ry[5]) / 3));  // LANDSCAPE
		cby = (((ry[2] + ry[4] + ry[7]) / 3));  // LANDSCAPE
	}
	if (cty < cby)
	{
		cty = cty - (py * 10);
		cby = cby + (py * 10);
	}
	else
	{
		cty = cty + (py * 10);
		cby = cby - (py * 10);
	}

	calx = (long(clx) << 14) + long(crx);
	caly = (long(cty) << 14) + long(cby);
	if (TOUCH_ORIENTATION == LANDSCAPE)
		cals = cals + (1L << 31);

	done();
	while (true) {}
}
