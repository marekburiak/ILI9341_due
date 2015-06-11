/*
This sketch is demonstrating loading images from an array.
Use BMP24toILI565Array to generate .h files from your .bmp images.
Then include those .h files in your sketch and use the array name
(which is same as filename) in drawImage call.
*/

#include "SPI.h"
#include "ILI9341_due_config.h"
#include "ILI9341_due.h"
#include "alert.h"
#include "info.h"
#include "close.h"

#define LCD_CS 10	// Chip Select for LCD
#define LCD_DC 9	// Command/Data for LCD
#define LCD_RST 8	// Reset for LCD

ILI9341_due tft(LCD_CS, LCD_DC, LCD_RST);

void setup()
{
	Serial.begin(9600);
	tft.begin();
	tft.setRotation(iliRotation270);	// landscape
	
	tft.fillScreen(ILI9341_BLACK);
	//tft.setSPIClockDivider(2); // lower the frequency if the image is distorted
	tft.drawImage(info, 100, 100, infoWidth, infoHeight);
	tft.drawImage(alert, 140, 100, alertWidth, alertHeight);
	tft.drawImage(close, 180, 100, closeWidth, closeHeight);
}

void loop()
{

  /* add main program code here */

}
