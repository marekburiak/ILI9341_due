/*
This sketch is demonstrating taking a screenshot with ILIScreenshotViewer
from the Tools folder.
Instructions:
- Compile and upload the sketch.
- Run ILIScreenshotViewer, set the COM port to whatever your Arduino 
  is connected to and the Baud Rate of 115200 (has to be the same as
  in Serial.begin(115200); and click Connect
- restart Arduino (if it has not automatically)
- the image should show up in ILIScreenshotViewer
*/

#include "SPI.h"
#include "ILI9341_due_config.h"
#include "ILI9341_due.h"
#include "alert.h"

// CS and DC for the LCD
#define LCD_CS 10	// Chip Select for LCD
#define LCD_DC 9	// Command/Data for LCD
#define LCD_RST 8	// Command/Data for LCD

ILI9341_due tft(LCD_CS, LCD_DC, LCD_RST);

void setup()
{
	Serial.begin(115200);
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	
	tft.fillScreen(ILI9341_BLACK);
	tft.drawImage(alert, 140, 100, alertWidth, alertHeight);

	// reduce the SPI clock speed if you get errors or image artifacts
//#ifdef __ARM__
//	tft.setSPIClockDivider(10);
//#elif defined __AVR__
//	tft.setSPIClockDivider(SPI_CLOCK_DIV8);
//#endif
	tft.screenshotToConsole();
}

void loop()
{

  /* add main program code here */

}
