#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "fonts/Arial_bold_14.h"

#include <URTouch.h>

// For the Adafruit shield, these are the default.
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

URTouch  myTouch(30, 28, 26, 24, 22);

int x, y;
char strBuff[10];
char stCurrent[20] = "";
int stCurrentLen = 0;
char stLast[20] = "";

/*************************
**   Custom functions   **
*************************/

void drawButtons()
{
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
	// Draw the upper row of buttons
	for (x = 0; x<5; x++)
	{
		tft.fillRoundRect(10 + (x * 60), 10, 50, 50, 5, ILI9341_BLUE);
		tft.drawRoundRect(10 + (x * 60), 10, 50, 50, 5, ILI9341_WHITE);
		itoa(x + 1, strBuff, 10);
		Serial.println(strBuff);
		tft.printAt(strBuff, 32 + (x * 60), 30);
	}
	// Draw the center row of buttons
	for (x = 0; x<5; x++)
	{
		tft.fillRoundRect(10 + (x * 60), 70, 50, 50, 5, ILI9341_BLUE);
		tft.drawRoundRect(10 + (x * 60), 70, 50, 50, 5, ILI9341_WHITE);
		if (x < 4){
			itoa(x + 6, strBuff, 10);
			tft.printAt(strBuff, 32 + (x * 60), 90);
		}
			
	}
	tft.printAt("0", 270, 90);
	// Draw the lower row of buttons
	tft.fillRoundRect(10, 130, 140, 50, 5, ILI9341_BLUE);
	tft.drawRoundRect(10, 130, 140, 50, 5, ILI9341_WHITE);
	tft.printAt("Clear", 63, 150);
	tft.fillRoundRect(160, 130, 140, 50, 5, ILI9341_BLUE);
	tft.drawRoundRect(160, 130, 140, 50, 5, ILI9341_WHITE);
	tft.printAt("Enter", 208, 150);
}

void updateStr(int val)
{
	if (stCurrentLen<20)
	{
		stCurrent[stCurrentLen] = val;
		stCurrent[stCurrentLen + 1] = '\0';
		stCurrentLen++;
		tft.setTextColor(ILI9341_LIME, ILI9341_BLACK);
		tft.printAlignedOffseted(stCurrent, gTextAlignTopCenter, 0, 224);
	}
	else
	{
		tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
		tft.printAlignedOffseted("BUFFER FULL!", gTextAlignTopCenter, 0, 190);
		delay(500);
		tft.printAlignedOffseted(" ", gTextAlignTopCenter, 0, 190, gTextEraseFullLine);
		delay(500);
		tft.printAlignedOffseted("BUFFER FULL!", gTextAlignTopCenter, 0, 190);
		delay(500);
		tft.printAlignedOffseted(" ", gTextAlignTopCenter, 0, 190, gTextEraseFullLine);
	}
}

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
	tft.drawRoundRect(x1, y1, x2-x1, y2-y1, 5, ILI9341_RED);
	while (myTouch.dataAvailable())
		myTouch.read();
	tft.drawRoundRect(x1, y1, x2-x1, y2-y1, 5, ILI9341_WHITE);
}

/*************************
**  Required functions  **
*************************/

void setup()
{
	Serial.begin(9600);
	// Initial setup
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	tft.fillScreen(ILI9341_BLACK);

	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);

	tft.setFont(Arial_bold_14);
	drawButtons();
}

void loop()
{
	while (true)
	{
		if (myTouch.dataAvailable())
		{
			myTouch.read();
			x = myTouch.getX();
			y = myTouch.getY();

			if ((y >= 10) && (y <= 60))  // Upper row
			{
				if ((x >= 10) && (x <= 60))  // Button: 1
				{
					waitForIt(10, 10, 60, 60);
					updateStr('1');
				}
				if ((x >= 70) && (x <= 120))  // Button: 2
				{
					waitForIt(70, 10, 120, 60);
					updateStr('2');
				}
				if ((x >= 130) && (x <= 180))  // Button: 3
				{
					waitForIt(130, 10, 180, 60);
					updateStr('3');
				}
				if ((x >= 190) && (x <= 240))  // Button: 4
				{
					waitForIt(190, 10, 240, 60);
					updateStr('4');
				}
				if ((x >= 250) && (x <= 300))  // Button: 5
				{
					waitForIt(250, 10, 300, 60);
					updateStr('5');
				}
			}

			if ((y >= 70) && (y <= 120))  // Center row
			{
				if ((x >= 10) && (x <= 60))  // Button: 6
				{
					waitForIt(10, 70, 60, 120);
					updateStr('6');
				}
				if ((x >= 70) && (x <= 120))  // Button: 7
				{
					waitForIt(70, 70, 120, 120);
					updateStr('7');
				}
				if ((x >= 130) && (x <= 180))  // Button: 8
				{
					waitForIt(130, 70, 180, 120);
					updateStr('8');
				}
				if ((x >= 190) && (x <= 240))  // Button: 9
				{
					waitForIt(190, 70, 240, 120);
					updateStr('9');
				}
				if ((x >= 250) && (x <= 300))  // Button: 0
				{
					waitForIt(250, 70, 300, 120);
					updateStr('0');
				}
			}

			if ((y >= 130) && (y <= 180))  // Upper row
			{
				if ((x >= 10) && (x <= 150))  // Button: Clear
				{
					waitForIt(10, 130, 150, 180);
					stCurrent[0] = '\0';
					stCurrentLen = 0;
					tft.fillRect(0, 224, 320, 15, ILI9341_BLACK);
				}
				if ((x >= 160) && (x <= 300))  // Button: Enter
				{
					waitForIt(160, 130, 300, 180);
					if (stCurrentLen>0)
					{
						for (x = 0; x<stCurrentLen + 1; x++)
						{
							stLast[x] = stCurrent[x];
						}
						stCurrent[0] = '\0';
						stCurrentLen = 0;
						tft.fillRect(0, 208, 320, 31, ILI9341_BLACK);
						tft.setTextColor(ILI9341_LIME, ILI9341_BLACK);
						tft.printAlignedOffseted(stLast, gTextAlignTopCenter, 0, 208);
					}
					else
					{
						tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
						tft.printAlignedOffseted("BUFFER EMPTY", gTextAlignTopCenter, 0, 190);
						delay(500);
						tft.printAlignedOffseted(" ", gTextAlignTopCenter, 0, 190, gTextEraseFullLine);
						delay(500);
						tft.printAlignedOffseted("BUFFER EMPTY", gTextAlignTopCenter, 0, 190);
						delay(500);
						tft.printAlignedOffseted(" ", gTextAlignTopCenter, 0, 190, gTextEraseFullLine);
					}
				}
			}
		}
	}
}

