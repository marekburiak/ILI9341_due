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

int color = 0;
word colorlist[] = { ILI9341_WHITE, ILI9341_BLACK, ILI9341_RED, ILI9341_BLUE, ILI9341_LIME, ILI9341_FUCHSIA, ILI9341_YELLOW, ILI9341_AQUA };
int  bsize = 4;

void drawColorMarkerAndBrushSize(int col)
{
	tft.fillRect(25, 0, 7, 239,ILI9341_BLACK);
	
	tft.drawPixel(25, (col * 30) + 15, ILI9341_WHITE);
	for (int i = 1; i<7; i++)
		tft.drawLine(25 + i, ((col * 30) + 15) - i, 25 + i, ((col * 30) + 15) + i, ILI9341_WHITE);

	tft.fillRect(tft.width() - 31, 161, 31, 31, colorlist[col] == ILI9341_BLACK ? ILI9341_WHITE : ILI9341_BLACK);

	if (bsize == 1)
		tft.drawPixel(tft.width() - 15, 177, colorlist[col]);
	else
		tft.fillCircle(tft.width() - 15, 177, bsize, colorlist[col]);
}

void setup()
{
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	tft.fillScreen(ILI9341_BLACK);
	
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_HI);

	tft.setFont(Arial_bold_14);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.drawLine(32, 0, 32, tft.height() - 1, ILI9341_WHITE);
	tft.drawLine(tft.width() - 32, 0, tft.width() - 32, tft.height() - 1, ILI9341_WHITE);
	tft.printAt("C", tft.width() - 20, 8);
	tft.printAt("L", tft.width() - 20, 24);
	tft.printAt("E", tft.width() - 20, 40);
	tft.printAt("A", tft.width() - 20, 56);
	tft.printAt("R", tft.width() - 20, 72);
	tft.printAt("+", tft.width() - 18, 138);
	tft.printAt("-", tft.width() - 18, 202);
	tft.fillRect(tft.width() - 32, 96, 32, 32, ILI9341_WHITE);
	tft.drawLine(tft.width() - 32, 160, tft.width() - 1, 160, ILI9341_WHITE);
	tft.drawLine(tft.width() - 32, 192, tft.width() - 1, 192, ILI9341_WHITE);
	tft.drawLine(tft.width() - 32, 224, tft.width() - 1, 224, ILI9341_WHITE);
	for (int i = 0; i<8; i++)
	{
		tft.fillRect(0, (i * 30), 24, 30, colorlist[i]);
	}
	drawColorMarkerAndBrushSize(color);
}

void loop()
{
	long x, y;

	while (myTouch.dataAvailable() == true)
	{
		myTouch.read();
		x = myTouch.getX();
		y = myTouch.getY();
		if ((x != -1) && (y != -1))
		{
			if (x>(32 + bsize) && (x<tft.width() - (32 + bsize)))
			{
				if (bsize == 1)
					tft.drawPixel(x, y, colorlist[color]);
				else
					tft.fillCircle(x, y, bsize, colorlist[color]);
			}
			else
			{
				if (x<(30 + bsize))
				{
					if (y<240)
					{
						color = y / 30;
						drawColorMarkerAndBrushSize(color);
						while (myTouch.dataAvailable()) {};
						delay(50);
					}
				}
				else
				{
					if (y<96)
					{
						tft.fillRect(33, 0, tft.width() - 65, tft.height(), ILI9341_BLACK);
					}
					if ((y>128) && (y<160))
					{
						if (bsize<10)
						{
							bsize++;
							drawColorMarkerAndBrushSize(color);
							while (myTouch.dataAvailable()) {};
							delay(50);
						}
					}
					if ((y>160) && (y<192))
					{
						bsize = 4;
						drawColorMarkerAndBrushSize(color);
						while (myTouch.dataAvailable()) {};
						delay(50);
					}
					if ((y>192) && (y<224))
					{
						if (bsize>1)
						{
							bsize--;
							drawColorMarkerAndBrushSize(color);
							while (myTouch.dataAvailable()) {};
							delay(50);
						}
					}
				}
			}
		}
	}
}

