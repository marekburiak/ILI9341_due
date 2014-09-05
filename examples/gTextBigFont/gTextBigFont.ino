#include <SPI.h>
#include <SdSpi.h>
#include <SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

#include "ILI9341_due\fonts\jokerman_255.h"

#define TFT_CS 4
#define TFT_DC 5

ILI9341_due myTFT(TFT_CS, TFT_DC);
ILI9341_due_gText t1(&myTFT);

void setup()
{
	Serial.begin(9600);
	
	bool result = myTFT.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");

	myTFT.fillScreen(ILI9341_WHITE);

	t1.DefineArea(18, 32, 222, 287);
	t1.SelectFont(jokerman_255);
	//t1.SetFontMode(FONT_MODE_TRANSPARENT);
}

void loop()
{
	t1.SetFontLetterSpacing(2);
	t1.SetFontColor(255,230,0,255,255,255);
	t1.DrawString("0", 0, 0);
	delay(750);
	t1.SetFontLetterSpacing(91); //a lazy way of erasing the remains of previous number
								 //(if the width of the current one is smaller) than of the previous one,
								 //like when rendering number 1 after number 0)
	t1.SetFontColor(232,157,12);
	t1.DrawString("1", 0, 0);
	delay(750);
	t1.SetFontLetterSpacing(0);
	t1.SetFontColor(255,88,0);
	t1.DrawString("2", 0, 0);
	delay(750);
	t1.SetFontColor(232,12,15);
	t1.DrawString("3", 0, 0);
	delay(750);
	t1.SetFontColor(227,13,255);
	t1.DrawString("4", 0, 0);
	delay(750);
	t1.SetFontLetterSpacing(34);
	t1.SetFontColor(57,0,255);
	t1.DrawString("5", 0, 0);
	delay(750);
	t1.SetFontLetterSpacing(3);
	t1.SetFontColor(12,103,232);
	t1.DrawString("6", 0, 0);
	delay(750);
	t1.SetFontColor(0,255,243);
	t1.DrawString("7", 0, 0);
	delay(750);
	t1.SetFontColor(12,232,73);
	t1.DrawString("8", 0, 0);
	delay(750);
	t1.SetFontColor(132,255,13);
	t1.DrawString("9", 0, 0);
	delay(750);
}
