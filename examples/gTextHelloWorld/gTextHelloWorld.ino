#include <SdSpi.h>
#include <SdFatConfig.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

#include "ILI9341_due\fonts\allFonts.h"

#define TFT_CS 4
#define TFT_DC 5

ILI9341_due myTFT(TFT_CS, TFT_DC);

void setup()
{
	Serial.begin(9600);
	
	bool result = myTFT.begin();
	
	Serial.print("TFT begin successful: ");
	Serial.println(result ? "YES" : "NO");

	myTFT.fillScreen(ILI9341_BLUE);

	ILI9341_due_gText t1(&myTFT);
	t1.DefineArea(100, 100, 200, 150);
	t1.SelectFont(Arial_bold_14, ILI9341_WHITE);
	t1.SetFontLetterSpacing(5);
	t1.DrawString("Hello World", 0, 0);	// these coordinates are relative to the defined area
}

void loop()
{

	/* add main program code here */

}
