#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>

#include "fonts\jokerman_255.h"

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
	tft.fillScreen(ILI9341_BLACK);

	tft.setFont(jokerman_255);
}

void loop()
{
	tft.setTextColor(255,230,0,0,0,0);
	tft.printAligned("0", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(232,157,12);
	tft.printAligned("1", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(255,88,0);
	tft.printAligned("2", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(232,12,15);
	tft.printAligned("3", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(227,13,255);
	tft.printAligned("4", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(57,0,255);
	tft.printAligned("5", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(12,103,232);
	tft.printAligned("6", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(0,255,243);
	tft.printAligned("7", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(12,232,73);
	tft.printAligned("8", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
	tft.setTextColor(132,255,13);
	tft.printAligned("9", gTextAlignMiddleCenter, gTextEraseFullLine);
	delay(750);
}
