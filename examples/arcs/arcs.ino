
#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "fonts\Arial_bold_14.h"
#include "roboto16.h"
#include "roboto32.h"
#include "roboto70.h"

#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

char textBuff[20];

uint16_t colorLightGray = tft.color565(192,192,192);
uint16_t colorGray = tft.color565(127,127,127);
uint16_t colorDarkGray = tft.color565(64,64,64);

void setup()
{
	Serial.begin(9600);
	while (!Serial) ; // wait for Arduino Serial Monitor

	tft.begin();
	tft.setRotation(iliRotation270);

	screenIntro();
	delay(2000);
	screenLoading();
	screenClock();
	screenPie();
	delay(2000);
	screenSensors();
}

void screenIntro()
{
	tft.fillScreen(ILI9341_BLUE);
	tft.setFont(Arial_bold_14);
	tft.setTextLetterSpacing(5);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLUE);
	tft.printAligned(F("Arcs demo"), gTextAlignMiddleCenter);
}

void screenLoading()
{
	const uint16_t x = 159;
	const uint16_t y = 149;
	tft.fillScreen(ILI9341_BLACK);
	tft.setFont(roboto32);
	tft.setTextLetterSpacing(5);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.printAlignedOffseted(F("Loading..."), gTextAlignMiddleCenter, 0, -20);
	tft.fillArc(x, y, 10, 3, 0, 360, colorLightGray);

	for(int i=0; i<2880; i+=4)
	{
		tft.fillArc(x, y, 10, 3, (i >> 1)-45, (i >> 1)+45, colorDarkGray);
		tft.fillArc(x, y, 10, 3, (i >> 1)-45-4, (i >> 1)-45, colorLightGray);

		tft.fillArc(x, y, 20, 3, 1440-i-45, 1440-i+45, colorDarkGray);
		tft.fillArc(x, y, 20, 3, 1440-i+45, 1440-i+45+4, colorLightGray);
	}
}

void screenClock()
{
	const uint16_t x = 159;
	const uint16_t y = 119;
	tft.fillScreen(ILI9341_BLACK);
	tft.setFont(roboto70);
	tft.setTextLetterSpacing(5);
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.printAligned("15:06", gTextAlignMiddleCenter);
	tft.fillArc(x,y,102,11,0,225, colorLightGray);	// 15 hours
	tft.fillArc(x,y,113,8,0,36, colorGray);	// 6 minutes
	tft.fillArc(x,y,120,5,0,360, colorDarkGray);	// seconds

	for(uint16_t d=324; d<372; d++)
	{
		tft.fillArc(x,y,120,5,d-1,d+1, ILI9341_RED);
		tft.fillArc(x,y,120,5,d-2,d-1, colorDarkGray);	// to erase the red
		if(d == 360)
		{
			tft.printAligned("15:07", gTextAlignMiddleCenter);
			tft.fillArc(x,y,113,8,0,42, colorGray);	// 7 minutes
		}
		delay(166);
	}
}

void screenPie()
{
	const uint16_t x = 159;
	const uint16_t y = 119;
	const uint16_t radius = 80;

	tft.fillScreen(ILI9341_BLACK);
	tft.setFont(roboto16);
	tft.setTextLetterSpacing(2);
	tft.fillArc(x+3,y-2,radius,radius,0,60, tft.color565(198,255,13));
	tft.fillArc(x-3,y+7,radius+10,radius+10,60,340, tft.color565(255,0,54));
	tft.fillArc(x-2,y-3,radius,radius,340,360, tft.color565(0,255,241));

	tft.setTextArea(0,0,220,180);
	tft.setFont(roboto16);
	tft.setFontMode(gTextFontModeTransparent);
	tft.setTextColor(ILI9341_BLACK);
	tft.printAt("16%",175,70);
	tft.printAt("78%",140,150);
	tft.setTextColor(colorLightGray);
	tft.printAt("6%",132,18);
	delay(2000);
}

void screenSensors()
{
	const uint16_t radius = 55;
	float temp=22.4;
	uint16_t hum=73, lux=1154;

	const uint16_t s1x = 0;
	const uint16_t s1y = 10;

	const uint16_t s2x = 110;
	const uint16_t s2y = 70;

	const uint16_t s3x = 210;
	const uint16_t s3y = 130;

	tft.fillScreen(ILI9341_BLACK);

	tft.setFontMode(gTextFontModeSolid);
	tft.setTextLetterSpacing(3);

	gTextArea t1 = { s1x, s1y, 2 * radius, 2 * radius };
	gTextArea t2 = { s2x, s2y, 2 * radius, 2 * radius };
	gTextArea t3 = { s3x, s3y, 2 * radius, 2 * radius };

	tft.setTextColor(ILI9341_WHITE);
	tft.fillArc(s1x+radius,s1y+radius,radius,10,-3,3, tft.color565(127,0,27));
	tft.setFont(roboto16);
	tft.setTextArea(t1);
	tft.printAlignedOffseted("C", gTextAlignMiddleCenter, 0, 25);
	tft.setFont(roboto32);
	sprintf(textBuff, "%4.1f", temp);
	tft.printAligned(textBuff, gTextAlignMiddleCenter);
	tft.fillRect(s1x+radius-7,s1y+radius+17,2,2,ILI9341_WHITE);	// degrees symbol

	for(uint16_t d=1; d<temp*10; d++)
	{
		tft.fillArc(s1x+radius,s1y+radius,radius,10,d,d+3, tft.color565(127,0,27));
		tft.fillArc(s1x+radius,s1y+radius,radius-3,4,d-1,d, tft.color565(255,0,54));
	}

	tft.setTextColor(ILI9341_WHITE);
	tft.fillArc(s2x+radius,s2y+radius,radius,10,-3,3, tft.color565(0,43,127));
	tft.setFont(roboto16);
	tft.setTextArea(t2);
	tft.printAlignedOffseted("%", gTextAlignMiddleCenter, 0, 25);
	tft.setFont(roboto32);
	sprintf(textBuff, "%d", hum);
	tft.printAligned(textBuff, gTextAlignMiddleCenter);

	for(uint16_t d=1; d<263; d++)
	{
		tft.fillArc(s2x+radius,s2y+radius,radius,10,d,d+3, tft.color565(0,43,127));
		tft.fillArc(s2x+radius,s2y+radius,radius-3,4,d-1,d, tft.color565(0,86,255));
	}

	tft.setTextColor(ILI9341_WHITE);
	tft.fillArc(s3x+radius,s3y+radius,radius,10,-3,3, tft.color565(127,103,6));
	tft.setFont(roboto16);
	tft.setTextArea(t3);
	tft.printAlignedOffseted("lux", gTextAlignMiddleCenter, 0, 25);
	tft.setFont(roboto32);
	sprintf(textBuff, "%d", lux);
	tft.printAligned(textBuff, gTextAlignMiddleCenter);

	for(uint16_t d=1; d<42; d++)
	{
		tft.fillArc(s3x+radius,s3y+radius,radius,10,d,d+3, tft.color565(127,103,6));
		tft.fillArc(s3x+radius,s3y+radius,radius-3,4,d-1,d,  tft.color565(255,206,13));
	}

	delay(1000);
	for(uint16_t d=1; d<660; d++)
	{
		if(d<220 )
		{
			lux+=21;

			sprintf(textBuff, "%d", lux);
			tft.printAligned(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.fillArc(s3x+radius,s3y+radius,radius,10, ((float)lux/(float)10000)*360,((float)lux/(float)10000)*360+3, tft.color565(127,103,6));
			tft.fillArc(s3x+radius,s3y+radius,radius-3,4,((float)lux/(float)10000)*360-4,((float)lux/(float)10000)*360, tft.color565(255,206,13));
		}

		if(d == 220)
			delay(1000);

		if(d>220 && d % 15 == 0)
		{
			temp+=0.1;
			sprintf(textBuff, "%4.1f", temp);
			tft.setTextArea(t1);
			tft.printAligned(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.fillArc(s1x+radius,s1y+radius,radius,10,temp*10,temp*10+3, tft.color565(127,0,27));
			tft.fillArc(s1x+radius,s1y+radius,radius-3,4,temp*10-2,temp*10, tft.color565(255,0,54));
			delay(random(350, 700));
		}
		if(d>300 && d<600 && d % 38 == 0)
		{
			hum+=1;
			sprintf(textBuff, "%d", hum);
			tft.setTextArea(t2);
			tft.printAligned(textBuff, gTextAlignMiddleCenter, 3, 3);
			tft.fillArc(s2x+radius,s2y+radius,radius,10,(float)hum*3.6-4,(float)hum*3.6+3, tft.color565(0,43,127));
			tft.fillArc(s2x+radius,s2y+radius,radius-3,4,(float)hum*3.6-5,(float)hum*3.6, tft.color565(0,86,255));
		}
	}
}


void loop()
{

	/* add main program code here */

}
