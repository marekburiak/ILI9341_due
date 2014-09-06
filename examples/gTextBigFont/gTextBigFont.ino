#include <SPI.h> //uncomment when using SPI_MODE_NORMAL or SPI_MODE_EXTENDED
#include <SdSpi.h>
#include <SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

#include "ILI9341_due\fonts\jokerman_255.h"

#define TFT_CS 10
#define TFT_DC 9

ILI9341_due myTFT(TFT_CS, TFT_DC);
ILI9341_due_gText t1(&myTFT);

void setup()
{
	Serial.begin(9600);
	
	bool result = myTFT.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");

	myTFT.fillScreen(ILI9341_BLACK);

	t1.defineArea(18, 32, 222, 287);
	t1.selectFont(jokerman_255);
	//t1.setFontMode(FONT_MODE_TRANSPARENT);
}

void loop()
{
	
	t1.setFontLetterSpacing(2);
	t1.setFontColor(255,230,0,0,0,0);
	unsigned long start = micros();
	t1.drawString("0", 0, 0);
	delay(750);
	t1.setFontLetterSpacing(91); //a lazy way of erasing the remains of previous number
								 //(if the width of the current one is smaller) than of the previous one,
								 //like when rendering number 1 after number 0)
	t1.setFontColor(232,157,12);
	t1.drawString("1", 0, 0);
	delay(750);
	t1.setFontLetterSpacing(0);
	t1.setFontColor(255,88,0);
	t1.drawString("2", 0, 0);
	delay(750);
	t1.setFontColor(232,12,15);
	t1.drawString("3", 0, 0);
	delay(750);
	t1.setFontColor(227,13,255);
	t1.drawString("4", 0, 0);
	delay(750);
	t1.setFontLetterSpacing(34);
	t1.setFontColor(57,0,255);
	t1.drawString("5", 0, 0);
	delay(750);
	t1.setFontLetterSpacing(3);
	t1.setFontColor(12,103,232);
	t1.drawString("6", 0, 0);
	delay(750);
	t1.setFontColor(0,255,243);
	t1.drawString("7", 0, 0);
	delay(750);
	t1.setFontColor(12,232,73);
	t1.drawString("8", 0, 0);
	delay(750);
	t1.setFontColor(132,255,13);
	t1.drawString("9", 0, 0);
	delay(750);
	Serial.println( (micros() - start)/10000);

}
