#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>

#include "fonts\Arial_bold_14.h"

#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8

ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

void setup()
{
	Serial.begin(9600);
	
	bool result = tft.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");

	tft.setRotation(iliRotation270);
	tft.fillScreen(ILI9341_BLUE);

	tft.setFont(Arial_bold_14);
	tft.setTextLetterSpacing(5);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
	tft.printAligned(F("Hello World"), gTextAlignMiddleCenter);
}

void loop()
{

	/* add main program code here */

}
