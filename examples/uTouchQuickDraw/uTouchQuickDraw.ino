#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>

#include <URTouch.h>

// For the Adafruit shield, these are the default.
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

URTouch  myTouch(30, 28, 26, 24, 22);
int x, y, px, py;


void setup()
{
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	tft.fillScreen(ILI9341_BLUE);
	
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);
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
			tft.drawPixel(x, y, ILI9341_WHITE);
		}
	}
}
