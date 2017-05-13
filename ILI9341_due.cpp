/*
ILI9341_due_.cpp - Arduino Due library for interfacing with ILI9341-based TFTs

Copyright (c) 2014  Marek Buriak

This library is based on ILI9341_t3 library from Paul Stoffregen
(https://github.com/PaulStoffregen/ILI9341_t3), Adafruit_ILI9341
and Adafruit_GFX libraries from Limor Fried/Ladyada
(https://github.com/adafruit/Adafruit_ILI9341).

This file is part of the Arduino ILI9341_due library.
Sources for this library can be found at https://github.com/marekburiak/ILI9341_Due.

ILI9341_due is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

ILI9341_due is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with ILI9341_due.  If not, see <http://www.gnu.org/licenses/>.
*/

/***************************************************
This is our library for the Adafruit ILI9341 Breakout and Shield
----> http://www.adafruit.com/products/1651

Check out the links above for our tutorials and wiring diagrams
These displays use SPI to communicate, 4 or 5 pins are required to
interface (RST is optional)
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution.
****************************************************/

#include "ILI9341_due.h"
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED | defined(ILI_USE_SPI_TRANSACTION)
#include <SPI.h>
#endif

//#include "..\Streaming\Streaming.h"

#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wswitch"


static const uint8_t init_commands[] PROGMEM = {
	4, 0xEF, 0x03, 0x80, 0x02,
	4, 0xCF, 0x00, 0XC1, 0X30,
	5, 0xED, 0x64, 0x03, 0X12, 0X81,
	4, 0xE8, 0x85, 0x00, 0x78,
	6, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
	2, 0xF7, 0x20,
	3, 0xEA, 0x00, 0x00,
	2, ILI9341_PWCTR1, 0x23, // Power control
	2, ILI9341_PWCTR2, 0x10, // Power control
	3, ILI9341_VMCTR1, 0x3e, 0x28, // VCM control
	2, ILI9341_VMCTR2, 0x86, // VCM control2
	2, ILI9341_MADCTL, 0x48, // Memory Access Control
	2, ILI9341_PIXFMT, 0x55,
	3, ILI9341_FRMCTR1, 0x00, 0x18,
	4, ILI9341_DFUNCTR, 0x08, 0x82, 0x27, // Display Function Control
	2, 0xF2, 0x00, // Gamma Function Disable
	2, ILI9341_GAMMASET, 0x01, // Gamma curve selected
	16, ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
	0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00, // Set Gamma
	16, ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
	0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F, // Set Gamma
	0
};


ILI9341_due::ILI9341_due(uint8_t cs, uint8_t dc, uint8_t rst)
{
	_cs = cs;
	_dc = dc;
	_rst = rst;
	_spiClkDivider = ILI9341_SPI_CLKDIVIDER;
	_width = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;
	_area.x = 0;
	_area.y = 0;
	_area.w = ILI9341_TFTWIDTH;
	_area.h = ILI9341_TFTHEIGHT;
	_rotation = iliRotation0;

	_arcAngleMax = DEFAULT_ARC_ANGLE_MAX;
	_angleOffset = DEFAULT_ANGLE_OFFSET;

#ifdef ILI_USE_SPI_TRANSACTION
	_isInTransaction = false;
#endif

	_fontMode = gTextFontModeSolid;
	_fontBgColor = ILI9341_BLACK;
	_fontColor = ILI9341_WHITE;
	_letterSpacing = DEFAULT_LETTER_SPACING;
	_lineSpacing = DEFAULT_LINE_SPACING;
#ifdef TEXT_SCALING_ENABLED
	_textScale = 1;
#endif
	_isFirstChar = true;
	setTextArea(0, 0, _width - 1, _height - 1);

}


bool ILI9341_due::begin(void)
{
	if (pinIsChipSelect(_cs)) {
		pinMode(_dc, OUTPUT);
		_dcport = portOutputRegister(digitalPinToPort(_dc));
		_dcpinmask = digitalPinToBitMask(_dc);

#if SPI_MODE_NORMAL | SPI_MODE_DMA
		pinMode(_cs, OUTPUT);
		_csport = portOutputRegister(digitalPinToPort(_cs));
		_cspinmask = digitalPinToBitMask(_cs);
#endif

#if SPI_MODE_NORMAL
		SPI.begin();
#elif SPI_MODE_EXTENDED
		SPI.begin(_cs);
#elif SPI_MODE_DMA
		dmaBegin();
#endif
		setSPIClockDivider(ILI9341_SPI_CLKDIVIDER);

		// toggle RST low to reset
		if (_rst < 255) {
			pinMode(_rst, OUTPUT);
			digitalWrite(_rst, HIGH);
			delay(5);
			digitalWrite(_rst, LOW);
			delay(20);
			digitalWrite(_rst, HIGH);
			delay(150);
		}

		const uint8_t *addr = init_commands;
		while (1) {
			uint8_t count = pgm_read_byte(addr++);
			if (count-- == 0) break;
			writecommand_cont(pgm_read_byte(addr++));
			while (count-- > 0) {
				writedata8_cont(pgm_read_byte(addr++));
			}
		}

		writecommand_last(ILI9341_SLPOUT);    // Exit Sleep
		delay(120);
		writecommand_last(ILI9341_DISPON);    // Display on
		delay(120);
		_isInSleep = _isIdle = false;



		//#ifdef ILI_USE_SPI_TRANSACTION
		//#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		endTransaction();
		//#endif
		//#endif
		return true;
	}
	else {
		return false;
	}
}

bool ILI9341_due::pinIsChipSelect(uint8_t cs)
{
#if SPI_MODE_EXTENDED
	if (cs == 4 || cs == 10 || cs == 52)	// in Extended SPI mode only these pins are valid
	{
		return true;
	}
	else
	{
		Serial.print("Pin ");
		Serial.print(_cs);
		Serial.println(" is not a valid Chip Select pin for SPI Extended Mode. Valid pins are 4, 10, 52");
		return false;
	}
#elif SPI_MODE_NORMAL | SPI_MODE_DMA
	return true;
#endif
}

void ILI9341_due::getDisplayStatus(void)
{
	beginTransaction();
	uint8_t x = readcommand8(ILI9341_RDMODE);
	Serial.print(F("\nDisplay Power Mode: 0x")); Serial.println(x, HEX);
	Serial.print(F("  Booster: ")); Serial.println(x & 0x80 ? F("On and working OK") : F("Off or has a fault"));
	Serial.print(F("  Idle Mode: ")); Serial.println(x & 0x40 ? F("On") : F("Off"));
	Serial.print(F("  Partial Mode: ")); Serial.println(x & 0x20 ? F("On") : F("Off"));
	Serial.print(F("  Sleep Mode: ")); Serial.println(x & 0x10 ? F("Off") : F("On"));
	Serial.print(F("  Display Normal Mode: ")); Serial.println(x & 0x08 ? F("On") : F("Off"));
	Serial.print(F("  Display: ")); Serial.println(x & 0x04 ? F("On") : F("Off"));

	x = readcommand8(ILI9341_RDMADCTL);
	Serial.print(F("MADCTL Mode: 0x")); Serial.println(x, HEX);
	Serial.println(x & 0x80 ? F("  Bottom to Top") : F("  Top to Bottom"));
	Serial.println(x & 0x40 ? F("  Right to Left") : F("  Left to Right"));
	Serial.println(x & 0x20 ? F("  Normal Mode") : F("  Reverse Mode"));
	Serial.println(x & 0x10 ? F("  LCD Refresh Bottom to Top") : F("  LCD Refresh Top to Bottom"));
	Serial.println(x & 0x08 ? F("  BGR") : F("RGB"));
	Serial.println(x & 0x04 ? F("  LCD Refresh Right to Left") : F("  LCD Refresh Left to Right"));

	x = readcommand8(ILI9341_RDPIXFMT);
	Serial.print(F("Pixel Format: 0x")); Serial.println(x, HEX);
	if ((x & 0x07) == 0x05)
		Serial.println(F("  16 bits/pixel"));
	if ((x & 0x07) == 0x06)
		Serial.println(F("  18 bits/pixel"));

	x = readcommand8(ILI9341_RDIMGFMT);
	Serial.print(F("Image Format: 0x")); Serial.println(x, HEX);
	if ((x & 0x07) == 0x00)
		Serial.println(F("  Gamma curve 1"));

	x = readcommand8(ILI9341_RDDSPSGNMODE);
	Serial.print(F("Display Signal Mode: 0x")); Serial.println(x, HEX);
	Serial.print(F("  Tearing effect line: ")); Serial.println(x & 0x80 ? F("On") : F("Off"));
	Serial.print(F("  Tearing effect line: mode ")); Serial.println(x & 0x40 ? F("2") : F("1"));
	Serial.print(F("  Horizontal sync: ")); Serial.println(x & 0x20 ? F("On") : F("Off"));
	Serial.print(F("  Vertical sync: ")); Serial.println(x & 0x10 ? F("On") : F("Off"));
	Serial.print(F("  Pixel clock: ")); Serial.println(x & 0x08 ? F("On") : F("Off"));
	Serial.print(F("  Data enable: ")); Serial.println(x & 0x04 ? F("On") : F("Off"));

	x = readcommand8(ILI9341_RDSELFDIAG);
	Serial.print(F("Self Diagnostic: 0x")); Serial.println(x, HEX);
	Serial.print(F("  Register Loading: ")); Serial.println(x & 0x80 ? F("working") : F("not working"));
	Serial.print(F("  Functionality: ")); Serial.println(x & 0x40 ? F("working") : F("not working"));

	endTransaction();
}

void ILI9341_due::setSPIClockDivider(uint8_t divider)
{
	_spiClkDivider = divider;
#ifdef ILI_USE_SPI_TRANSACTION
#if defined (ARDUINO_SAM_DUE)
	_spiSettings = SPISettings(F_CPU / divider, MSBFIRST, SPI_MODE0);
#elif defined (ARDUINO_ARCH_AVR)
#if divider == SPI_CLOCK_DIV2
	_spiSettings = SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV4
	_spiSettings = SPISettings(F_CPU / 4, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV8
	_spiSettings = SPISettings(F_CPU / 8, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV16
	_spiSettings = SPISettings(F_CPU / 16, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV32
	_spiSettings = SPISettings(F_CPU / 32, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV64
	_spiSettings = SPISettings(F_CPU / 64, MSBFIRST, SPI_MODE0);
#elif divider == SPI_CLOCK_DIV128
	_spiSettings = SPISettings(F_CPU / 128, MSBFIRST, SPI_MODE0);
#endif
#endif
#endif

#ifdef ILI_USE_SPI_TRANSACTION
	beginTransaction();
#else
#if SPI_MODE_NORMAL
	SPI.setClockDivider(divider);
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);
#elif SPI_MODE_EXTENDED
	SPI.setClockDivider(_cs, divider);
	SPI.setBitOrder(_cs, MSBFIRST);
	SPI.setDataMode(_cs, SPI_MODE0);
#endif
#endif

#if SPI_MODE_DMA
	dmaInit(divider);
#endif
}

void ILI9341_due::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	beginTransaction();
	enableCS();
	setAddrAndRW_cont(x0, y0, x1 - x0 + 1, y1 - y0 + 1);
	disableCS();
	endTransaction();
}

void ILI9341_due::setAddrWindowRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
	beginTransaction();
	enableCS();
	setAddrAndRW_cont(x, y, w, h);
	disableCS();
	endTransaction();
}

void ILI9341_due::pushColor(uint16_t color)
{
	beginTransaction();
	enableCS();
	setDCForData();
	write16_last(color);
	endTransaction();
}

//void ILI9341_due::pushColors(uint16_t *colors, uint16_t offset, uint16_t len) {
//	beginTransaction();
//	enableCS();
//	setDCForData();
//	colors = colors + offset * 2;
//#if SPI_MODE_EXTENDED
//	uint16_t i;
//	for (i = 0; i < len-1; i++) {
//		write16_cont(colors[i]);
//	}
//	write16_last(colors[i]);
//#else
//	for (uint16_t i = 0; i < (len << 1); i += 2) {
//		uint16_t color = *colors;
//		_scanline[i] = highByte(color);
//		_scanline[i + 1] = lowByte(color);
//		colors++;
//	}
//	writeScanline(len);
//	disableCS();
//#endif
//
//	endTransaction();
//}

// pushes pixels stored in the colors array (one color is 2 bytes) 
// in big endian (high byte first)
// len should be the length of the array (so to push 320 pixels,
// you have to have a 640-byte array and len should be 640)
//void ILI9341_due::pushColors565(uint8_t *colors, uint16_t offset, uint32_t len) {
//	beginTransaction();
//	enableCS();
//	setDCForData();
//	colors = colors + offset;
//
//	//#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
//	//	for (uint16_t i = 0; i < len; i++) {
//	//		write8_cont(colors[i]);
//	//	}
//	//#elif SPI_MODE_DMA
//	write_cont(colors, len);
//	//#endif
//	disableCS();
//	endTransaction();
//}

void ILI9341_due::pushColors(const uint16_t *colors, uint16_t offset, uint32_t len) {
	beginTransaction();
	enableCS();
	pushColors_noTrans_noCS(colors, offset, len);
	disableCS();
	endTransaction();
}

void ILI9341_due::pushColors(uint16_t *colors, uint16_t offset, uint32_t len) {
	beginTransaction();
	enableCS();
	setDCForData();
	colors = colors + offset;
	write_cont(colors, len);
	disableCS();
	endTransaction();
}

void ILI9341_due::pushColors_noTrans_noCS(const uint16_t *colors, uint16_t offset, uint32_t len) {
	setDCForData();
	colors = colors + offset;

#if SPI_MODE_DMA
	const uint32_t numLoops = len / (uint32_t)SCANLINE_PIXEL_COUNT;
	for (uint32_t l = 0; l < numLoops; l++)
	{
		for (uint32_t i = 0; i < SCANLINE_PIXEL_COUNT; i++)
		{
			_scanline16[i] = colors[l*SCANLINE_PIXEL_COUNT + i];
		}
		writeScanline16(SCANLINE_PIXEL_COUNT);
	}
	uint16_t remainingPixels = len % SCANLINE_PIXEL_COUNT;
	if (remainingPixels > 0) {
		for (uint32_t i = 0; i < remainingPixels; i++)
		{
			_scanline16[i] = colors[numLoops*SCANLINE_PIXEL_COUNT + i];
		}
		writeScanline16(remainingPixels);
	}
#else
	write_cont(colors, len);
#endif
}


void ILI9341_due::drawPixel(int16_t x, int16_t y, uint16_t color) {
	beginTransaction();
	enableCS();
	drawPixel_last(x, y, color);
	disableCS();
	endTransaction();
}

void ILI9341_due::drawImage(const uint16_t *colors, uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	const uint32_t totalPixels = (uint32_t)w*(uint32_t)h;
	beginTransaction();
	enableCS();
	setAddrAndRW_cont(x, y, w, h);
	pushColors_noTrans_noCS(colors, 0, totalPixels);
	disableCS();
	endTransaction();
}

void ILI9341_due::drawFastVLine(int16_t x, int16_t y, uint16_t h, uint16_t color)
{
	beginTransaction();
	drawFastVLine_noTrans(x, y, h, color);
	endTransaction();
}

void ILI9341_due::drawFastVLine_noTrans(int16_t x, int16_t y, uint16_t h, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height)) return;
	if ((y + (int16_t)h - 1) >= _height) h = _height - y;

	fillScanline16(color, min(h, SCANLINE_PIXEL_COUNT));

	enableCS();
	setAddrAndRW_cont(x, y, 1, h);
	setDCForData();
#ifdef ARDUINO_SAM_DUE
	writeScanline16(h);
#elif defined ARDUINO_ARCH_AVR
	writeScanlineLooped(h);
#endif
	disableCS();
}

void ILI9341_due::drawFastVLine_cont_noFill(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	//	if ((x >= _width) || (y >= _height)) return;
	//	if ((y + h - 1) >= _height) h = _height - y;
	//
	//	setAddrAndRW_cont(x, y, 1, h);
	//	setDCForData();
	//#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	//	while (h-- > 0) {
	//		write16_cont(color);
	//	}
	//#elif SPI_MODE_DMA
	//	writeScanline(h);
	//#endif

	if ((x >= _width) || (y >= _height)) return;
	if ((y + h - 1) >= _height) h = _height - y;

	setAddrAndRW_cont(x, y, 1, h);
	setDCForData();
#ifdef ARDUINO_SAM_DUE
	writeScanline16(h);
#elif defined ARDUINO_ARCH_AVR
	writeScanlineLooped(h);
#endif

}

void ILI9341_due::drawFastHLine(int16_t x, int16_t y, uint16_t w, uint16_t color)
{
	beginTransaction();
	drawFastHLine_noTrans(x, y, w, color);
	endTransaction();
}

void ILI9341_due::drawFastHLine_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t color)
{
	// Rudimentary clipping
	if ((x >= _width) || (y >= _height)) return;
	if ((x + (int16_t)w - 1) >= _width)  w = _width - x;


	fillScanline16(color, min(w, SCANLINE_PIXEL_COUNT));
	enableCS();
	setAddrAndRW_cont(x, y, w, 1);
	setDCForData();
#ifdef ARDUINO_SAM_DUE
	writeScanline16(w);
#elif defined ARDUINO_ARCH_AVR
	writeScanlineLooped(w);
#endif
	disableCS();
}

void ILI9341_due::fillScreen(uint16_t color)
{
	const uint32_t numLoops = (uint32_t)76800 / (uint32_t)SCANLINE_PIXEL_COUNT;
	fillScanline16(color);

	beginTransaction();
	enableCS();
	setAddrAndRW_cont(0, 0, _width, _height);
	setDCForData();
	for (uint32_t l = 0; l < numLoops; l++)
	{
		writeScanline16(SCANLINE_PIXEL_COUNT);
	}
	disableCS();
	endTransaction();
	//#endif
}

// fill a rectangle
void ILI9341_due::fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	beginTransaction();
	fillRect_noTrans(x, y, w, h, color);
	endTransaction();
}

void ILI9341_due::fillRectWithShader(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t(*fillShader)(uint16_t rx, uint16_t ry))
{
	beginTransaction();
	fillRectWithShader_noTrans(x, y, w, h, fillShader);
	endTransaction();
}


// fill a rectangle
void ILI9341_due::fillRect_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	//Serial << "x:" << x << " y:" << y << " w:" << x << " h:" << h << " width:" << _width << " height:" << _height <<endl;
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height) || (x + w - 1 < 0) || (y + h - 1 < 0)) return;
	if ((x + (int16_t)w - 1) >= _width)  w = _width - x;
	if ((y + (int16_t)h - 1) >= _height) h = _height - y;

	const uint32_t totalPixels = (uint32_t)w*(uint32_t)h;
	fillScanline16(color, min(totalPixels, SCANLINE_PIXEL_COUNT));
	enableCS();
	setAddrAndRW_cont(x, y, w, h);
	setDCForData();
	writeScanlineLooped(totalPixels);
	disableCS();
}

void ILI9341_due::fillRectWithShader_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t(*fillShader)(uint16_t rx, uint16_t ry))
{
	//Serial << "x:" << x << " y:" << y << " w:" << x << " h:" << h << " width:" << _width << " height:" << _height <<endl;
	// rudimentary clipping (drawChar w/big text requires this)
	if ((x >= _width) || (y >= _height) || (x + w - 1 < 0) || (y + h - 1 < 0)) return;
	if ((x + (int16_t)w - 1) >= _width)  w = _width - x;
	if ((y + (int16_t)h - 1) >= _height) h = _height - y;

	enableCS();
	setAddrAndRW_cont(x, y, w, h);
	setDCForData();
	for (uint16_t ry = 0; ry < h; ry++) {
		for (uint16_t rx = 0; rx < w; rx++)
		{
			_scanline16[rx] = fillShader(rx, ry);
		}
		writeScanline16(w);
	}
	disableCS();
}

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void ILI9341_due::setRotation(iliRotation r)
{
	beginTransaction();
	writecommand_cont(ILI9341_MADCTL);
	_rotation = r;
	switch (r) {
	case iliRotation0:
		writedata8_last(MADCTL_MX | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case iliRotation90:
		writedata8_last(MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	case iliRotation180:
		writedata8_last(MADCTL_MY | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case iliRotation270:
		writedata8_last(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	}
	//_area.x = 0;
	//_area.y = 0;
	_area.w = _width;
	_area.h = _height;
	endTransaction();
}


void ILI9341_due::invertDisplay(boolean i)
{
	beginTransaction();
	writecommand_last(i ? ILI9341_INVON : ILI9341_INVOFF);
	endTransaction();
}


// Reads one pixel/color from the TFT's GRAM
uint16_t ILI9341_due::readPixel(int16_t x, int16_t y)
{
	beginTransaction();
	//setAddr_cont(x, y, x + 1, y + 1); ? should it not be x,y,x,y?
	setAddr_cont(x, y, 1, 1);
	writecommand_cont(ILI9341_RAMRD); // read from RAM
	readdata8_cont(); // dummy read
	uint8_t red = read8_cont();
	uint8_t green = read8_cont();
	uint8_t blue = read8_last();
	uint16_t color = color565(red, green, blue);
	endTransaction();
	return color;

}

//void ILI9341_due::drawArc(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, uint16_t start, uint16_t end, uint16_t color) {
//	//void graphics_draw_arc(GContext *ctx, GPoint p, int radius, int thickness, int start, int end) {
//	start = start % 360;
//	end = end % 360;
//
//	while (start < 0) start += 360;
//	while (end < 0) end += 360;
//
//	if (end == 0) end = 360;
//
//	//Serial << "start: " << start << " end:" << end << endl;
//
//	// Serial <<  (float)cos_lookup(start * ARC_MAX_STEPS / 360) << " x " << (float)sin_lookup(start * ARC_MAX_STEPS / 360) << endl;
//
//	float sslope = (float)cos_lookup(start * ARC_MAX_STEPS / 360) / (float)sin_lookup(start * ARC_MAX_STEPS / 360);
//	float eslope = (float)cos_lookup(end * ARC_MAX_STEPS / 360) / (float)sin_lookup(end * ARC_MAX_STEPS / 360);
//
//	//Serial << "sslope: " << sslope << " eslope:" << eslope << endl;
//
//	if (end == 360) eslope = -1000000;
//
//	int ir2 = (radius - thickness) * (radius - thickness);
//	int or2 = radius * radius;
//
//	for (int x = -radius; x <= radius; x++)
//		for (int y = -radius; y <= radius; y++)
//		{
//			int x2 = x * x;
//			int y2 = y * y;
//
//			if (
//				(x2 + y2 < or2 && x2 + y2 >= ir2) &&
//				(
//				(y > 0 && start < 180 && x <= y * sslope) ||
//				(y < 0 && start > 180 && x >= y * sslope) ||
//				(y < 0 && start <= 180) ||
//				(y == 0 && start <= 180 && x < 0) ||
//				(y == 0 && start == 0 && x > 0)
//				) &&
//				(
//				(y > 0 && end < 180 && x >= y * eslope) ||
//				(y < 0 && end > 180 && x <= y * eslope) ||
//				(y > 0 && end >= 180) ||
//				(y == 0 && end >= 180 && x < 0) ||
//				(y == 0 && start == 0 && x > 0)
//				)
//				)
//				drawPixel_cont(cx+x, cy+y, color);
//		}
//}


// DrawArc function thanks to Jnmattern and his Arc_2.0 (https://github.com/Jnmattern)
void ILI9341_due::fillArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color) {
	int16_t xmin = 65535, xmax = -32767, ymin = 32767, ymax = -32767;
	float cosStart, sinStart, cosEnd, sinEnd;
	float r, t;
	float startAngle, endAngle;

	//Serial << "start: " << start << " end: " << end << endl;
	startAngle = (start / _arcAngleMax) * 360;	// 252
	endAngle = (end / _arcAngleMax) * 360;		// 807
	//Serial << "startAngle: " << startAngle << " endAngle: " << endAngle << endl;

	while (startAngle < 0) startAngle += 360;
	while (endAngle < 0) endAngle += 360;
	while (startAngle > 360) startAngle -= 360;
	while (endAngle > 360) endAngle -= 360;
	//Serial << "startAngleAdj: " << startAngle << " endAngleAdj: " << endAngle << endl;
	//if (endAngle == 0) endAngle = 360;

	if (startAngle > endAngle) {
		fillArcOffsetted(cx, cy, radius, thickness, ((startAngle) / (float)360) * _arcAngleMax, _arcAngleMax, color);
		fillArcOffsetted(cx, cy, radius, thickness, 0, ((endAngle) / (float)360) * _arcAngleMax, color);
	}
	else {
		// Calculate bounding box for the arc to be drawn
		cosStart = cosDegrees(startAngle);
		sinStart = sinDegrees(startAngle);
		cosEnd = cosDegrees(endAngle);
		sinEnd = sinDegrees(endAngle);

		//Serial << cosStart << " " << sinStart << " " << cosEnd << " " << sinEnd << endl;

		r = radius;
		// Point 1: radius & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 2: radius & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		r = radius - thickness;
		// Point 3: radius-thickness & startAngle
		t = r * cosStart;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinStart;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;

		// Point 4: radius-thickness & endAngle
		t = r * cosEnd;
		if (t < xmin) xmin = t;
		if (t > xmax) xmax = t;
		t = r * sinEnd;
		if (t < ymin) ymin = t;
		if (t > ymax) ymax = t;


		//Serial << xmin << " " << xmax << " " << ymin << " " << ymax << endl;
		// Corrections if arc crosses X or Y axis
		if ((startAngle < 90) && (endAngle > 90)) {
			ymax = radius;
		}

		if ((startAngle < 180) && (endAngle > 180)) {
			xmin = -radius;
		}

		if ((startAngle < 270) && (endAngle > 270)) {
			ymin = -radius;
		}

		// Slopes for the two sides of the arc
		float sslope = (float)cosStart / (float)sinStart;
		float eslope = (float)cosEnd / (float)sinEnd;

		//Serial << "sslope2: " << sslope << " eslope2:" << eslope << endl;

		if (endAngle == 360) eslope = -1000000;

		int ir2 = (radius - thickness) * (radius - thickness);
		int or2 = radius * radius;
		//Serial << "ymin: " << ymin << " ymax: " << ymax << endl;

		fillScanline16(color);

		enableCS();
		for (int x = xmin; x <= xmax; x++) {
			bool y1StartFound = false, y2StartFound = false;
			bool y1EndFound = false, y2EndSearching = false;
			int y1s = 0, y1e = 0, y2s = 0;
			for (int y = ymin; y <= ymax; y++)
			{
				int x2 = x * x;
				int y2 = y * y;

				if (
					(x2 + y2 < or2 && x2 + y2 >= ir2) && (
					(y > 0 && startAngle < 180 && x <= y * sslope) ||
						(y < 0 && startAngle > 180 && x >= y * sslope) ||
						(y < 0 && startAngle <= 180) ||
						(y == 0 && startAngle <= 180 && x < 0) ||
						(y == 0 && startAngle == 0 && x > 0)
						) && (
						(y > 0 && endAngle < 180 && x >= y * eslope) ||
							(y < 0 && endAngle > 180 && x <= y * eslope) ||
							(y > 0 && endAngle >= 180) ||
							(y == 0 && endAngle >= 180 && x < 0) ||
							(y == 0 && startAngle == 0 && x > 0)))
				{
					if (!y1StartFound)	//start of the higher line found
					{
						y1StartFound = true;
						y1s = y;
					}
					else if (y1EndFound && !y2StartFound) //start of the lower line found
					{
						//Serial << "Found y2 start x: " << x << " y:" << y << endl;
						y2StartFound = true;
						//drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
						y2s = y;
						y += y1e - y1s - 1;	// calculate the most probable end of the lower line (in most cases the length of lower line is equal to length of upper line), in the next loop we will validate if the end of line is really there
						if (y > ymax - 1) // the most probable end of line 2 is beyond ymax so line 2 must be shorter, thus continue with pixel by pixel search
						{
							y = y2s;	// reset y and continue with pixel by pixel search
							y2EndSearching = true;
						}

						//Serial << "Upper line length: " << (y1e - y1s) << " Setting y to " << y << endl;
					}
					else if (y2StartFound && !y2EndSearching)
					{
						// we validated that the probable end of the lower line has a pixel, continue with pixel by pixel search, in most cases next loop with confirm the end of lower line as it will not find a valid pixel
						y2EndSearching = true;
					}
					//Serial << "x:" << x << " y:" << y << endl;
					//drawPixel_cont(cx+x, cy+y, ILI9341_BLUE);
				}
				else
				{
					if (y1StartFound && !y1EndFound) //higher line end found
					{
						y1EndFound = true;
						y1e = y - 1;
						//Serial << "line: " << y1s << " - " << y1e << endl;
						drawFastVLine_cont_noFill(cx + x, cy + y1s, y - y1s, color);
						if (y < 0)
						{
							//Serial << x << " " << y << endl;
							y = abs(y); // skip the empty middle
						}
						else
							break;
					}
					else if (y2StartFound)
					{
						if (y2EndSearching)
						{
							//Serial << "Found final end at y: " << y << endl;
							// we found the end of the lower line after pixel by pixel search
							drawFastVLine_cont_noFill(cx + x, cy + y2s, y - y2s, color);
							y2EndSearching = false;
							break;
						}
						else
						{
							//Serial << "Expected end not found" << endl;
							// the expected end of the lower line is not there so the lower line must be shorter
							y = y2s;	// put the y back to the lower line start and go pixel by pixel to find the end
							y2EndSearching = true;
						}
					}
					//else
					//drawPixel_cont(cx+x, cy+y, ILI9341_RED);
				}
				//

				//delay(75);
			}
			if (y1StartFound && !y1EndFound)
			{
				y1e = ymax;
				//Serial << "line: " << y1s << " - " << y1e << endl;
				drawFastVLine_cont_noFill(cx + x, cy + y1s, y1e - y1s + 1, color);
			}
			else if (y2StartFound && y2EndSearching)	// we found start of lower line but we are still searching for the end
			{										// which we haven't found in the loop so the last pixel in a column must be the end
				drawFastVLine_cont_noFill(cx + x, cy + y2s, ymax - y2s + 1, color);
			}
		}
		disableCS();
	}
}

void ILI9341_due::screenshotToConsole()
{
	uint8_t lastColor[3];
	uint8_t color[3];
	uint32_t sameColorPixelCount = 0;
	uint16_t sameColorPixelCount16 = 0;
	uint32_t sameColorStartIndex = 0;
	uint32_t totalImageDataLength = 0;
	Serial.println();
	Serial.println(F("==== PIXEL DATA START ===="));
	//uint16_t x=0;
	//uint16_t y=0;
	beginTransaction();
	setAddr_cont(0, 0, _width, _height);
	writecommand_cont(ILI9341_RAMRD); // read from RAM
	readdata8_cont(); // dummy read, also sets DC high

#if SPI_MODE_DMA
	read_cont(color, 3);
	lastColor[0] = color[0];
	lastColor[1] = color[1];
	lastColor[2] = color[2];
#elif SPI_MODE_NORMAL | SPI_MODE_EXTENDED
	lastColor[0] = color[0] = read8_cont();
	lastColor[1] = color[1] = read8_cont();
	lastColor[2] = color[2] = read8_cont();
#endif
	printHex8(color, 3);	//write color of the first pixel
	totalImageDataLength += 6;
	sameColorStartIndex = 0;

	for (uint32_t i = 1; i < (uint32_t)_width*(uint32_t)_height; i++)
	{
#if SPI_MODE_DMA
		read_cont(color, 3);
#elif SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		color[0] = read8_cont();
		color[1] = read8_cont();
		color[2] = read8_cont();
#endif

		if (color[0] != lastColor[0] ||
			color[1] != lastColor[1] ||
			color[2] != lastColor[2])
		{
			sameColorPixelCount = i - sameColorStartIndex;
			if (sameColorPixelCount > 65535)
			{
				sameColorPixelCount16 = 65535;
				printHex16(&sameColorPixelCount16, 1);
				printHex8(lastColor, 3);
				totalImageDataLength += 10;
				sameColorPixelCount16 = sameColorPixelCount - 65535;
			}
			else
				sameColorPixelCount16 = sameColorPixelCount;
			printHex16(&sameColorPixelCount16, 1);
			printHex8(color, 3);
			totalImageDataLength += 10;

			sameColorStartIndex = i;
			lastColor[0] = color[0];
			lastColor[1] = color[1];
			lastColor[2] = color[2];
	}
}
	disableCS();
	endTransaction();
	sameColorPixelCount = (uint32_t)_width*(uint32_t)_height - sameColorStartIndex;
	if (sameColorPixelCount > 65535)
	{
		sameColorPixelCount16 = 65535;
		printHex16(&sameColorPixelCount16, 1);
		printHex8(lastColor, 3);
		totalImageDataLength += 10;
		sameColorPixelCount16 = sameColorPixelCount - 65535;
	}
	else
		sameColorPixelCount16 = sameColorPixelCount;
	printHex16(&sameColorPixelCount16, 1);
	totalImageDataLength += 4;
	printHex32(&totalImageDataLength, 1);

	Serial.println();
	Serial.println(F("==== PIXEL DATA END ===="));
	Serial.print(F("Total Image Data Length: "));
	Serial.println(totalImageDataLength);
}

/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to bex
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

// Draw a circle outline
void ILI9341_due::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	beginTransaction();
	enableCS();

	drawPixel_cont(x0, y0 + r, color);
	drawPixel_cont(x0, y0 - r, color);
	drawPixel_cont(x0 + r, y0, color);
	drawPixel_cont(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel_cont(x0 + x, y0 + y, color);
		drawPixel_cont(x0 - x, y0 + y, color);
		drawPixel_cont(x0 + x, y0 - y, color);
		drawPixel_cont(x0 - x, y0 - y, color);
		drawPixel_cont(x0 + y, y0 + x, color);
		drawPixel_cont(x0 - y, y0 + x, color);
		drawPixel_cont(x0 + y, y0 - x, color);
		drawPixel_cont(x0 - y, y0 - x, color);
	}
	disableCS();
	endTransaction();
}


void ILI9341_due::drawCircleHelper(int16_t x0, int16_t y0,
	int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	enableCS();
	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			drawPixel_cont(x0 + x, y0 + y, color);
			drawPixel_cont(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			drawPixel_cont(x0 + x, y0 - y, color);
			drawPixel_cont(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			drawPixel_cont(x0 - y, y0 + x, color);
			drawPixel_cont(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			drawPixel_cont(x0 - y, y0 - x, color);
			drawPixel_cont(x0 - x, y0 - y, color);
		}
	}
	disableCS();
}

void ILI9341_due::fillCircle(int16_t x0, int16_t y0, int16_t r,
	uint16_t color)
{
	beginTransaction();
	drawFastVLine_noTrans(x0, y0 - r, 2 * r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
	endTransaction();
}

// Used to do circles and roundrects
// Further optimizations by Chris_CT
void ILI9341_due::fillCircleHelper(int16_t x0, int16_t y0, int16_t r,
	uint8_t cornername, int16_t delta, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	int16_t ylm = x0 - r; // **added**

#ifdef ARDUINO_SAM_DUE
	fillScanline16(color, 2 * max(x, y) + 1 + delta);
#else
	fillScanline16(color);
#endif

	enableCS();
	while (x < y) {
		if (f >= 0) {
			if (cornername & 0x1) drawFastVLine_cont_noFill(x0 + y, y0 - x, 2 * x + 1 + delta, color); // **moved**
			if (cornername & 0x2) drawFastVLine_cont_noFill(x0 - y, y0 - x, 2 * x + 1 + delta, color); // **moved**
			ylm = x0 - y; // **added**
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if ((x0 - x) > ylm) { // **added**
			if (cornername & 0x1) drawFastVLine_cont_noFill(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			if (cornername & 0x2) drawFastVLine_cont_noFill(x0 - x, y0 - y, 2 * y + 1 + delta, color);
		} // **added**
}
	disableCS();
}

void ILI9341_due::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	beginTransaction();
	drawLine_noTrans(x0, y0, x1, y1, color);
	endTransaction();
}

void ILI9341_due::drawLineByAngle(int16_t x, int16_t y, int16_t angle, uint16_t length, uint16_t color)
{
	beginTransaction();
	drawLine_noTrans(
		x,
		y,
		x + length*cosDegrees(angle + _angleOffset),
		y + length*sinDegrees(angle + _angleOffset), color);
	endTransaction();
}


void ILI9341_due::drawLineByAngle(int16_t x, int16_t y, int16_t angle, uint16_t start, uint16_t length, uint16_t color)
{
	beginTransaction();
	drawLine_noTrans(
		x + start*cosDegrees(angle + _angleOffset),
		y + start*sinDegrees(angle + _angleOffset),
		x + (start + length)*cosDegrees(angle + _angleOffset),
		y + (start + length)*sinDegrees(angle + _angleOffset), color);
	endTransaction();
}

// Bresenham's algorithm - thx wikpedia
void ILI9341_due::drawLine_noTrans(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	beginTransaction();
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine_noTrans(x0, y0, x1 - x0 + 1, color);
		}
		else if (x1 < x0) {
			drawFastHLine_noTrans(x1, y0, x0 - x1 + 1, color);
		}
		else {
			drawPixel_last(x0, y0, color);
		}
		return;
	}
	else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine_noTrans(x0, y0, y1 - y0 + 1, color);
		}
		else {
			drawFastVLine_noTrans(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx >> 1;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}

	int16_t xbegin = x0;

	fillScanline16(color);

	enableCS();
	if (steep) {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
#ifdef ARDUINO_SAM_DUE
					writeVLine_cont_noCS_noFill(y0, xbegin, len + 1);
#elif defined ARDUINO_ARCH_AVR
					writeVLine_cont_noCS_noScanline(y0, xbegin, len + 1, color);
#endif
				}
				else {
					writePixel_cont(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) {
#ifdef ARDUINO_SAM_DUE
			writeVLine_cont_noCS_noFill(y0, xbegin, x0 - xbegin);
#elif defined ARDUINO_ARCH_AVR
			writeVLine_cont_noCS_noScanline(y0, xbegin, x0 - xbegin, color);
#endif
		}

	}
	else {
		for (; x0 <= x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
#ifdef ARDUINO_SAM_DUE
					writeHLine_cont_noCS_noFill(xbegin, y0, len + 1);
#elif defined ARDUINO_ARCH_AVR
					writeHLine_cont_noCS_noScanline(xbegin, y0, len + 1, color);
#endif
				}
				else {
					writePixel_cont(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
	}
		if (x0 > xbegin + 1) {
#ifdef ARDUINO_SAM_DUE
			writeHLine_cont_noCS_noFill(xbegin, y0, x0 - xbegin);
#elif defined ARDUINO_ARCH_AVR
			writeHLine_cont_noCS_noScanline(xbegin, y0, x0 - xbegin, color);
#endif
		}
	}
	disableCS();
	endTransaction();
}


// Draw a rectangle
//void ILI9341_due::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
//{
//	writeHLine_cont(x, y, w, color);
//	writeHLine_cont(x, y+h-1, w, color);
//	writeVLine_cont(x, y, h, color);
//	writeVLine_last(x+w-1, y, h, color);
//}

void ILI9341_due::drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	beginTransaction();

	fillScanline16(color, min(SCANLINE_PIXEL_COUNT, max(w, h)));

	enableCS();
	writeHLine_cont_noCS_noFill(x, y, w);
	writeHLine_cont_noCS_noFill(x, y + h - 1, w);
	writeVLine_cont_noCS_noFill(x, y, h);
	writeVLine_cont_noCS_noFill(x + w - 1, y, h);
	disableCS();
	endTransaction();
}

// Draw a rounded rectangle
void ILI9341_due::drawRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
{
	beginTransaction();

	fillScanline16(color, min(SCANLINE_PIXEL_COUNT, max(w, h)));

	enableCS();
	// smarter version
	writeHLine_cont_noCS_noFill(x + r, y, w - 2 * r); // Top
	writeHLine_cont_noCS_noFill(x + r, y + h - 1, w - 2 * r); // Bottom
	writeVLine_cont_noCS_noFill(x, y + r, h - 2 * r); // Left
	writeVLine_cont_noCS_noFill(x + w - 1, y + r, h - 2 * r); // Right
	disableCS();
	// draw four corners
	drawCircleHelper(x + r, y + r, r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
	endTransaction();
}

// Fill a rounded rectangle
void ILI9341_due::fillRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color)
{
	beginTransaction();
	// smarter version
	fillRect_noTrans(x + r, y, w - 2 * r, h, color);

	// draw four corners
	fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
	fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
	endTransaction();
}

// Draw a triangle
void ILI9341_due::drawTriangle(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1,
	int16_t x2, int16_t y2, uint16_t color)
{
	beginTransaction();
	drawLine_noTrans(x0, y0, x1, y1, color);
	drawLine_noTrans(x1, y1, x2, y2, color);
	drawLine_noTrans(x2, y2, x0, y0, color);
	endTransaction();
}

// Fill a triangle
void ILI9341_due::fillTriangle(int16_t x0, int16_t y0,
	int16_t x1, int16_t y1,
	int16_t x2, int16_t y2, uint16_t color)
{
	beginTransaction();
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}
	if (y1 > y2) {
		swap(y2, y1); swap(x2, x1);
	}
	if (y0 > y1) {
		swap(y0, y1); swap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawFastHLine_noTrans(a, y0, b - a + 1, color);
		endTransaction();
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1,
		sa = 0,
		sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2) last = y1;   // Include y1 scanline
	else         last = y1 - 1; // Skip it

	fillScanline16(color, min(SCANLINE_PIXEL_COUNT, max(x0, max(x1, x2)) - min(x0, min(x1, x2))));	// fill scanline with the widest scanline that'll be used

	enableCS();
	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) swap(a, b);
		writeHLine_cont_noCS_noFill(a, y, b - a + 1);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) swap(a, b);
		writeHLine_cont_noCS_noFill(a, y, b - a + 1);
	}
	disableCS();
	endTransaction();
}

// draws monochrome (single color) bitmaps
void ILI9341_due::drawBitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	uint16_t i, j, byteWidth = (w + 7) / 8;

	beginTransaction();
	enableCS();
	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel_cont(x + i, y + j, color);
			}
		}
	}
	disableCS();
	endTransaction();
}

// draws monochrome (single color) bitmaps
void ILI9341_due::drawBitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t bgcolor)
{
	uint16_t i, j, byteWidth = (w + 7) / 8;

	beginTransaction();
	enableCS();
	for (j = 0; j < h; j++)
	{
		for (i = 0; i < w; i++)
		{
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
#if defined ARDUINO_ARCH_AVR
				drawPixel_cont(x + i, y + j, color);
#elif defined ARDUINO_SAM_DUE
				_scanline16[i] = color;
#endif
			}
			else
			{
#if defined ARDUINO_ARCH_AVR
				drawPixel_cont(x + i, y + j, bgcolor);
#elif defined ARDUINO_SAM_DUE
				_scanline16[i] = bgcolor;
#endif
		}
		}
#ifdef ARDUINO_SAM_DUE
		setAddrAndRW_cont(x, y + j, w, 1);
		setDCForData();
		writeScanline16(w);
#endif
	}
	disableCS();
	endTransaction();
}

uint8_t ILI9341_due::getRotation(void) {
	return _rotation;
}

// if true, tft will be blank (white), 
// display's frame buffer is unaffected
// (you can write to it without showing content on the screen)
void ILI9341_due::display(boolean d) {
	beginTransaction();
	writecommand_last(d ? ILI9341_DISPON : ILI9341_DISPOFF);
	endTransaction();
}

// puts display in/out of sleep mode
void ILI9341_due::sleep(boolean s)
{
	beginTransaction();
	writecommand_last(s ? ILI9341_SLPIN : ILI9341_SLPOUT);
	endTransaction();
	delay(120);
}

void ILI9341_due::idle(boolean i) {
	beginTransaction();
	writecommand_last(i ? ILI9341_IDMON : ILI9341_IDMOFF);
	endTransaction();
}


void ILI9341_due::setPowerLevel(pwrLevel p)
{
	switch (p)
	{
	case pwrLevelNormal:
		if (_isIdle) { idle(false); _isIdle = false; }
		if (_isInSleep) { sleep(false); _isInSleep = false; }
		break;
	case pwrLevelIdle:
		if (!_isIdle) { idle(true); _isIdle = true; }
		if (_isInSleep) { sleep(false); _isInSleep = false; }
		break;
	case pwrLevelSleep:
		if (!_isInSleep) { sleep(true); _isInSleep = true; }
		if (_isIdle) { idle(false); _isIdle = false; }
		break;
	}
}


void ILI9341_due::setArcParams(float arcAngleMax)
{
	_arcAngleMax = arcAngleMax;
}

void ILI9341_due::setAngleOffset(int16_t angleOffset)
{
	_angleOffset = DEFAULT_ANGLE_OFFSET + angleOffset;
}

//uint8_t ILI9341_due::spiread(void) {
//	uint8_t r = 0;
//
//	//SPI.setClockDivider(_cs, 12); // 8-ish MHz (full! speed!)
//	//SPI.setBitOrder(_cs, MSBFIRST);
//	//SPI.setDataMode(_cs, SPI_MODE0);
//	r = SPI.transfer(_cs, 0x00);
//	Serial.print("read: 0x"); Serial.print(r, HEX);
//
//	return r;
//}
//
//void ILI9341_due::spiwrite(uint8_t c) {
//
//	//Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");
//
//
//	//SPI.setClockDivider(_cs, 12); // 8-ish MHz (full! speed!)
//	//SPI.setBitOrder(_cs, MSBFIRST);
//	//SPI.setDataMode(_cs, SPI_MODE0);
//	SPI.transfer(_cs, c);
//
//}

//void ILI9341_due::writecommand(uint8_t c) {
//	//*dcport &=  ~dcpinmask;
//	digitalWrite(_dc, LOW);
//	//*clkport &= ~clkpinmask; // clkport is a NULL pointer when hwSPI==true
//	//digitalWrite(_sclk, LOW);
//	//*csport &= ~cspinmask;
//	//digitalWrite(_cs, LOW);
//
//	spiwrite(c);
//
//	//*csport |= cspinmask;
//	//digitalWrite(_cs, HIGH);
//}
//
//
//void ILI9341_due::writedata(uint8_t c) {
//	//*dcport |=  dcpinmask;
//	digitalWrite(_dc, HIGH);
//	//*clkport &= ~clkpinmask; // clkport is a NULL pointer when hwSPI==true
//	//digitalWrite(_sclk, LOW);
//	//*csport &= ~cspinmask;
//	//digitalWrite(_cs, LOW);
//
//	spiwrite(c);
//
//	//digitalWrite(_cs, HIGH);
//	//*csport |= cspinmask;
//} 

void ILI9341_due::printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 2 + 1];
	byte first;
	byte second;
	for (int i = 0; i < length; i++) {
		first = (data[i] >> 4) & 0x0f;
		second = data[i] & 0x0f;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 2] = first + 48;
		tmp[i * 2 + 1] = second + 48;
		if (first > 9) tmp[i * 2] += 7;
		if (second > 9) tmp[i * 2 + 1] += 7;
	}
	tmp[length * 2] = 0;
	Serial.print(tmp);
}


void ILI9341_due::printHex16(uint16_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 4 + 1];
	byte first;
	byte second;
	byte third;
	byte fourth;
	for (int i = 0; i < length; i++) {
		first = (data[i] >> 12) & 0x0f;
		second = (data[i] >> 8) & 0x0f;
		third = (data[i] >> 4) & 0x0f;
		fourth = data[i] & 0x0f;
		//Serial << first << " " << second << " " << third << " " << fourth << endl;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 4] = first + 48;
		tmp[i * 4 + 1] = second + 48;
		tmp[i * 4 + 2] = third + 48;
		tmp[i * 4 + 3] = fourth + 48;
		//tmp[i*5+4] = 32; // add trailing space
		if (first > 9) tmp[i * 4] += 7;
		if (second > 9) tmp[i * 4 + 1] += 7;
		if (third > 9) tmp[i * 4 + 2] += 7;
		if (fourth > 9) tmp[i * 4 + 3] += 7;
	}
	tmp[length * 4] = 0;
	Serial.print(tmp);
}

void ILI9341_due::printHex32(uint32_t *data, uint8_t length) // prints 8-bit data in hex
{
	char tmp[length * 8 + 1];
	byte dataByte[8];
	for (int i = 0; i < length; i++) {
		dataByte[0] = (data[i] >> 28) & 0x0f;
		dataByte[1] = (data[i] >> 24) & 0x0f;
		dataByte[2] = (data[i] >> 20) & 0x0f;
		dataByte[3] = (data[i] >> 16) & 0x0f;
		dataByte[4] = (data[i] >> 12) & 0x0f;
		dataByte[5] = (data[i] >> 8) & 0x0f;
		dataByte[6] = (data[i] >> 4) & 0x0f;
		dataByte[7] = data[i] & 0x0f;
		//Serial << first << " " << second << " " << third << " " << fourth << endl;
		// base for converting single digit numbers to ASCII is 48
		// base for 10-16 to become upper-case characters A-F is 55
		// note: difference is 7
		tmp[i * 4] = dataByte[0] + 48;
		tmp[i * 4 + 1] = dataByte[1] + 48;
		tmp[i * 4 + 2] = dataByte[2] + 48;
		tmp[i * 4 + 3] = dataByte[3] + 48;
		tmp[i * 4 + 4] = dataByte[4] + 48;
		tmp[i * 4 + 5] = dataByte[5] + 48;
		tmp[i * 4 + 6] = dataByte[6] + 48;
		tmp[i * 4 + 7] = dataByte[7] + 48;
		//tmp[i*5+4] = 32; // add trailing space
		if (dataByte[0] > 9) tmp[i * 4] += 7;
		if (dataByte[1] > 9) tmp[i * 4 + 1] += 7;
		if (dataByte[2] > 9) tmp[i * 4 + 2] += 7;
		if (dataByte[3] > 9) tmp[i * 4 + 3] += 7;
		if (dataByte[4] > 9) tmp[i * 4 + 4] += 7;
		if (dataByte[5] > 9) tmp[i * 4 + 5] += 7;
		if (dataByte[6] > 9) tmp[i * 4 + 6] += 7;
		if (dataByte[7] > 9) tmp[i * 4 + 7] += 7;
	}
	tmp[length * 8] = 0;
	Serial.print(tmp);
}



void ILI9341_due::clearTextArea()
{
	fillRect(_area.x, _area.y, _area.w, _area.h, _fontBgColor);
}

void ILI9341_due::clearTextArea(uint16_t color)
{
	fillRect(_area.x, _area.y, _area.w, _area.h, color);
}

void ILI9341_due::clearTextArea(gTextArea area)
{
	fillRect(area.x, area.y, area.w, area.h, _fontBgColor);
}

void ILI9341_due::clearTextArea(gTextArea area, uint16_t color)
{
	fillRect(area.x, area.y, area.w, area.h, color);
}

void ILI9341_due::setTextArea(gTextArea area) //, textMode mode)
{
	_area.x = area.x;
	_area.y = area.y;
	_area.w = area.w;
	_area.h = area.h;
	_x = _xStart = area.x;
	_y = _yStart = area.y;
}

//void ILI9341_due::setTextArea(int16_t x0, int16_t y0, int16_t x1, int16_t y1) //, textMode mode)
//{
//	_area.x = x0;
//	_area.y = y0;
//	_area.x1 = x1;
//	_area.y1 = y1;
//	_x = x0;
//	_y = y0;
//}

void ILI9341_due::setTextArea(int16_t x, int16_t y, int16_t columns, int16_t rows, gTextFont font) //, textMode mode)
{
	//textMode mode = DEFAULT_SCROLLDIR;
	uint16_t x1, y1;

	setFont(font);

	x1 = x + columns * (pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH) + 1) - 1;
	y1 = y + rows * (getFontHeight() + 1) - 1;

	setTextArea(x, y, x1, y1); //, mode);
}

void ILI9341_due::setTextArea(int16_t x, int16_t y, int16_t w, int16_t h) //, textMode mode)
{
	_area.x = x;
	_area.y = y;
	_area.w = w;
	_area.h = h;
	_x = _xStart = x;
	_y = _xStart = y;
}

__attribute__((always_inline))
void ILI9341_due::specialChar(uint8_t c)
{


	if (c == '\n')
	{
		uint8_t height = getFontHeight();

		/*
		* Erase all pixels remaining to edge of text area.on all wraps
		* It looks better when using inverted (WHITE) text, on proportional fonts, and
		* doing WHITE scroll fills.
		*
		*/


		/*if (_fontMode == gTextFontModeSolid && _x < _area.x1)
			fillRect(_x, _y, _area.x1 - _x, height, _fontBgColor);*/
			//glcd_Device::SetPixels(_x, _y, _area.x1, _y+height, _fontColor == BLACK ? WHITE : BLACK);

			//		/*
			//		* Check for scroll up vs scroll down (scrollup is normal)
			//		*/
			//#ifndef GLCD_NO_SCROLLDOWN
			//		if (_area.mode == SCROLL_UP)
			//#endif
			//		{
			//
			//			/*
			//			* Normal/up scroll
			//			*/
			//
			//			/*
			//			* Note this comparison and the pixel calcuation below takes into
			//			* consideration that fonts
			//			* are atually 1 pixel taller when rendered.
			//			* This extra pixel is along the bottom for a "gap" between the character below.
			//			*/
			//			if (_y + 2 * height >= _area.y1)
			//			{
			//#ifndef GLCD_NODEFER_SCROLL
			//				if (!_needScroll)
			//				{
			//					_needScroll = 1;
			//					return;
			//				}
			//#endif
			//
			//				/*
			//				* forumula for pixels to scroll is:
			//				*	(assumes "height" is one less than rendered height)
			//				*
			//				*		pixels = height - ((_area.y1 - _y)  - height) +1;
			//				*
			//				*		The forumala below is unchanged
			//				*		But has been re-written/simplified in hopes of better code
			//				*
			//				*/
			//
			//				uint8_t pixels = 2 * height + _y - _area.y1 + 1;
			//
			//				/*
			//				* Scroll everything to make room
			//				* * NOTE: (FIXME, slight "bug")
			//				* When less than the full character height of pixels is scrolled,
			//				* There can be an issue with the newly created empty line.
			//				* This is because only the # of pixels scrolled will be colored.
			//				* What it means is that if the area starts off as white and the text
			//				* color is also white, the newly created empty text line after a scroll
			//				* operation will not be colored BLACK for the full height of the character.
			//				* The only way to fix this would be alter the code use a "move pixels"
			//				* rather than a scroll pixels, and then do a clear to end line immediately
			//				* after the move and wrap.
			//				*
			//				* Currently this only shows up when
			//				* there are are less than 2xheight pixels below the current Y coordinate to
			//				* the bottom of the text area
			//				* and the current background of the pixels below the current text line
			//				* matches the text color
			//				* and  a wrap was just completed.
			//				*
			//				* After a full row of text is printed, the issue will resolve itself.
			//				*
			//				*
			//				*/
			//				//ScrollUp(_area.x, _area.y, _area.x1, _area.y1, pixels, _fontBgColor);
			//
			//				_x = _area.x;
			//				_y = _area.y1 - height;
			//			}
			//			else
			//			{
			/*
			* Room for simple wrap
			*/

		_x = _xStart; // _area.x;
		_y = _y + (height + _lineSpacing)*_textScale;
		_isFirstChar = true;

		//			}
		//		}
		//#ifndef GLCD_NO_SCROLLDOWN
		//		else
		//		{
		//			/*
		//			* Reverse/Down scroll
		//			*/
		//
		//			/*
		//			* Check for Wrap vs scroll.
		//			*
		//			* Note this comparison and the pixel calcuation below takes into
		//			* consideration that fonts
		//			* are atually 1 pixel taller when rendered.
		//			*
		//			*/
		//			if (_y > _area.y + height)
		//			{
		//				/*
		//				* There is room so just do a simple wrap
		//				*/
		//				_x = _area.x;
		//				_y = _y - (height + 1);
		//			}
		//			else
		//			{
		//#ifndef GLCD_NODEFER_SCROLL
		//				if (!_needScroll)
		//				{
		//					_needScroll = 1;
		//					return;
		//				}
		//#endif
		//
		//				/*
		//				* Scroll down everything to make room for new line
		//				*	(assumes "height" is one less than rendered height)
		//				*/
		//
		//				uint8_t pixels = height + 1 - (_area.y - _y);
		//
		//				//ScrollDown(_area.x, _area.y, _area.x1, _area.y1, pixels, _fontBgColor);
		//
		//				_x = _area.x;
		//				_y = _area.y;
		//			}
		//		}
		//#endif
	}
	else if (c == '\r') {
		_isFirstChar = true;
	}

}

size_t ILI9341_due::write(uint8_t c)
{
	//Serial << c << endl2;
	if (_font == 0)
	{
		Serial.println(F("No font selected"));
		return 0; // no font selected
	}

	/*
	* check for special character processing
	*/

	if (c < 0x20)
	{
		specialChar(c);
		return 1;
	}
	uint16_t charWidth = 0;
	uint16_t charHeight = getFontHeight();
	uint8_t charHeightInBytes = (charHeight + 7) / 8; /* calculates height in rounded up bytes */

	uint8_t firstChar = pgm_read_byte(_font + GTEXT_FONT_FIRST_CHAR);
	uint8_t charCount = pgm_read_byte(_font + GTEXT_FONT_CHAR_COUNT);

	uint16_t index = 0;

	if (c < firstChar || c >= (firstChar + charCount)) {
		return 0; // invalid char
	}
	c -= firstChar;

	if (isFixedWidthFont(_font) {
		//thielefont = 0;
		charWidth = pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH);
		index = c*charHeightInBytes*charWidth + GTEXT_FONT_WIDTH_TABLE;
	}
	else {
		// variable width font, read width data, to get the index
		//thielefont = 1;
		/*
		* Because there is no table for the offset of where the data
		* for each character glyph starts, run the table and add up all the
		* widths of all the characters prior to the character we
		* need to locate.
		*/
		for (uint8_t i = 0; i < c; i++) {
			index += pgm_read_byte(_font + GTEXT_FONT_WIDTH_TABLE + i);
		}
		/*
		* Calculate the offset of where the font data
		* for our character starts.
		* The index value from above has to be adjusted because
		* there is potentialy more than 1 byte per column in the glyph,
		* when the characgter is taller than 8 bits.
		* To account for this, index has to be multiplied
		* by the height in bytes because there is one byte of font
		* data for each vertical 8 pixels.
		* The index is then adjusted to skip over the font width data
		* and the font header information.
		*/

		index = index*charHeightInBytes + charCount + GTEXT_FONT_WIDTH_TABLE;

		/*
		* Finally, fetch the width of our character
		*/
		charWidth = pgm_read_byte(_font + GTEXT_FONT_WIDTH_TABLE + c);
	}

	//#ifndef GLCD_NODEFER_SCROLL
	//	/*
	//	* check for a defered scroll
	//	* If there is a deferred scroll,
	//	* Fake a newline to complete it.
	//	*/
	//
	//	if (_needScroll)
	//	{
	//		write('\n'); // fake a newline to cause wrap/scroll
	//		_needScroll = 0;
	//	}
	//#endif

	/*
	* If the character won't fit in the text area,
	* fake a newline to get the text area to wrap and
	* scroll if necessary.
	* NOTE/WARNING: the below calculation assumes a 1 pixel pad.
	* This will need to be changed if/when configurable pixel padding is supported.
	*/
	//	if (_wrap && _x + charWidth > _area.x1)
	//	{
	//		write('\n'); // fake a newline to cause wrap/scroll
	//#ifndef GLCD_NODEFER_SCROLL
	//		/*
	//		* We can't defer a scroll at this point since we need to ouput
	//		* a character right now.
	//		*/
	//		if (_needScroll)
	//		{
	//			write('\n'); // fake a newline to cause wrap/scroll
	//			_needScroll = 0;
	//		}
	//#endif
	//	}
	beginTransaction();
	if (_fontMode == gTextFontModeSolid)
		drawSolidChar(c, index, charWidth, charHeight);
	else if (_fontMode == gTextFontModeTransparent)
		drawTransparentChar(c, index, charWidth, charHeight);
	endTransaction();

	return 1; // valid char
}

void ILI9341_due::drawSolidChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight)
{
	uint8_t bitId = 0;
	uint16_t py;
#ifdef ARDUINO_SAM_DUE
	uint16_t lineId = 0;
#endif
	uint16_t charHeightInBytes = (charHeight + 7) / 8; /* calculates height in rounded up bytes */

	uint8_t numRenderBits = 8;
	const uint8_t numRemainingBits = charHeight % 8 == 0 ? 8 : charHeight % 8;
	uint16_t numPixelsInOnePoint = 1;
	if (_textScale > 1)
		numPixelsInOnePoint = (uint16_t)_textScale *(uint16_t)_textScale;
#ifdef ARDUINO_ARCH_AVR
	uint16_t pixelsInOnePointToDraw;
#endif

	if (_letterSpacing > 0 && !_isFirstChar)
	{
#ifdef LINE_SPACING_AS_PART_OF_LETTERS
		fillRect(_x, _y, _letterSpacing * _textScale, (charHeight + _lineSpacing)*_textScale, _fontBgColor);
#else
		fillRect(_x, _y, _letterSpacing * _textScale, charHeight *_textScale, _fontBgColor);
#endif
		_x += _letterSpacing * _textScale;
	}
	_isFirstChar = false;

#ifdef LINE_SPACING_AS_PART_OF_LETTERS
	if (_lineSpacing > 0) {
		fillRect(_x, _y + charHeight*_textScale, charWidth * _textScale, _lineSpacing *_textScale, _fontBgColor);
	}
#endif

	//#if SPI_MODE_DMA
	//	if (_textScale > 1)
	//		fillScanline16(_fontColor, numPixelsInOnePoint);	//pre-fill the scanline, we will be drawing different lenghts of it
	//#endif

	enableCS();
	if (_textScale == 1)
		setRowAddr(_y, charHeight);

	for (uint16_t j = 0; j < charWidth; j++) /* each column */
	{
		//Serial << "Printing row" << endl;
#ifdef ARDUINO_SAM_DUE
		lineId = 0;
#endif
		numRenderBits = 8;
		if (_x >= 0 && _x < _width)
		{
			setColumnAddr(_x, _textScale);

			if (_textScale == 1)
			{
				//setAddrAndRW_cont(_x, _y, 1, charHeight);
				setRW();
				setDCForData();
			}

			for (uint16_t i = 0; i < charHeightInBytes; i++)	/* each vertical byte */
			{
				uint16_t page = i*charWidth; // page must be 16 bit to prevent overflow
				uint8_t data = pgm_read_byte(_font + index + page + j);

				/*
				* This funkyness is because when the character glyph is not a
				* multiple of 8 in height, the residual bits in the font data
				* were aligned to the incorrect end of the byte with respect
				* to the GLCD. I believe that this was an initial oversight (bug)
				* in Thieles font creator program. It is easily fixed
				* in the font program but then creates a potential backward
				* compatiblity problem.
				*	--- bperrybap
				*/
				if (charHeight > 8 && charHeight < (i + 1) * 8)	/* is it last byte of multibyte tall font? */
				{
					data >>= ((i + 1) << 3) - charHeight;	// (i+1)*8
				}
				//Serial << "data:" <<data << " x:" << cx << " y:" << cy << endl;


				if (i == charHeightInBytes - 1)	// last byte in column
					numRenderBits = numRemainingBits;

				for (bitId = 0; bitId < numRenderBits; bitId++)
				{
					py = _y + (i * 8 + bitId)*_textScale;
					if ((data & 0x01) == 0)
					{
						if (_textScale == 1)
						{
#ifdef ARDUINO_ARCH_AVR
							write16_cont(_fontBgColor);
#elif defined ARDUINO_SAM_DUE
							_scanline16[lineId++] = _fontBgColor;
#endif
						}
						else
						{
							// set a rectangle area
							//setAddrAndRW_cont(_x, py, _textScale, _textScale);
							setRowAddr(py, _textScale);
							setRW();
							//Serial << cx << " " << cy + (i * 8 + bitId)*_textScale << " " << _textScale <<endl2;
							setDCForData();
#ifdef ARDUINO_ARCH_AVR
							pixelsInOnePointToDraw = numPixelsInOnePoint;
							while (pixelsInOnePointToDraw--) {
								write16_cont(_fontBgColor);
							}
#elif defined ARDUINO_SAM_DUE
							fillScanline16(_fontBgColor, numPixelsInOnePoint);
							writeScanlineLooped(numPixelsInOnePoint);
#endif
						}
					}
					else
					{
						if (_textScale == 1)
						{
#ifdef ARDUINO_ARCH_AVR
							write16_cont(_fontColor);
#elif defined ARDUINO_SAM_DUE
							_scanline16[lineId++] = _fontColor;
#endif
						}
						else
						{
							// set a rectangle area
							//setAddrAndRW_cont(_x, py, _textScale, _textScale);
							setRowAddr(py, _textScale);
							setRW();
							setDCForData();
#ifdef ARDUINO_ARCH_AVR
							pixelsInOnePointToDraw = numPixelsInOnePoint;
							while (pixelsInOnePointToDraw--) {
								write16_cont(_fontColor);
							}
#elif defined ARDUINO_SAM_DUE
							fillScanline16(_fontColor, numPixelsInOnePoint);
							writeScanlineLooped(numPixelsInOnePoint);
#endif
						}
					}
					data >>= 1;
				}

				//#ifdef ARDUINO_ARCH_AVR
				//if (_textScale == 1 && (lineId == SCANLINE_PIXEL_COUNT - 1 || i == charHeightInBytes - 1))	// we have either filled the buffer or are rendering the bottom portion of the char
				//{
				//	writeScanline16(numRenderBits);	// max 8
				//	lineId = 0;
				//}
				//#endif

				//delay(50);
					}
				}
		//Serial << endl;
#ifdef ARDUINO_SAM_DUE
		if (_textScale == 1)
		{
			writeScanline16(charHeight);
		}
#endif
		_x += _textScale;

	}
	disableCS();	// to put CS line back up

	//_x = cx;

	//Serial << " ending at " << _x  << " lastChar " << _lastChar <<endl;


	//Serial << "letterSpacing " << _letterSpacing <<" x: " << _x <<endl;
			}

void ILI9341_due::drawTransparentChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight)
{
	uint8_t bitId = 0;
	uint8_t bit = 0, lastBit = 0;
	uint16_t lineStart = 0;
	uint16_t lineEnd = 0;
	if (_letterSpacing > 0 && !_isFirstChar)
	{
		_x += _letterSpacing * _textScale;
	}
	_isFirstChar = false;


	fillScanline16(_fontColor);	//pre-fill the scanline, we will be drawing different lenghts of it


	uint16_t charHeightInBytes = (charHeight + 7) / 8; /* calculates height in rounded up bytes */

	uint8_t numRenderBits = 8;
	const uint8_t numRemainingBits = charHeight % 8 == 0 ? 8 : charHeight % 8;

	enableCS();

	for (uint8_t j = 0; j < charWidth; j++) /* each column */
	{
		//Serial << "Printing row" << endl;
		numRenderBits = 8;

		if (_x >= 0 && _x < _width)
		{
			setColumnAddr(_x, _textScale);

			for (uint16_t i = 0; i < charHeightInBytes; i++)	/* each vertical byte */
			{
				uint16_t page = i*charWidth; // page must be 16 bit to prevent overflow
				uint8_t data = pgm_read_byte(_font + index + page + j);

				/*
				* This funkyness is because when the character glyph is not a
				* multiple of 8 in height, the residual bits in the font data
				* were aligned to the incorrect end of the byte with respect
				* to the GLCD. I believe that this was an initial oversight (bug)
				* in Thieles font creator program. It is easily fixed
				* in the font program but then creates a potential backward
				* compatiblity problem.
				*	--- bperrybap
				*/
				if (charHeight > 8 && charHeight < (i + 1) * 8)	/* is it last byte of multibyte tall font? */
				{
					data >>= ((i + 1) << 3) - charHeight;	// (i+1)*8
				}
				//Serial << "data:" <<data << " x:" << cx << " y:" << cy << endl;

				if (i == 0)
					bit = lastBit = lineStart = 0;
				else if (i == charHeightInBytes - 1)	// last byte in column
					numRenderBits = numRemainingBits;

				for (bitId = 0; bitId < numRenderBits; bitId++)
				{
					bit = data & 0x01;
					if (bit ^ lastBit)	// same as bit != lastBit
					{
						if (bit ^ 0x00) // if bit != 0 (so it's 1)
						{
							lineStart = lineEnd = (i * 8 + bitId) * _textScale;
						}
						else
						{
							const uint32_t totalPixels = (lineEnd - lineStart + _textScale) * _textScale;
							//setRowAddr(_y + lineStart, _y + lineEnd + _textScale - 1);
							setRowAddr(_y + lineStart, lineEnd - lineStart + _textScale);
							setRW();
							setDCForData();
							writeScanlineLooped(totalPixels);

							//setAddrAndRW_cont(_x, _y + lineStart, _textScale, lineEnd - lineStart + _textScale);
							////fillRect(cx, cy + lineStart, _textScale, lineEnd - lineStart + _textScale, ILI9341_BLUEVIOLET);

							//setDCForData();

							//for (uint8_t s = 0; s < _textScale; s++)
							//{
							//	writeScanline16(lineEnd - lineStart + _textScale);
							//}
						}
						lastBit = bit;
					}
					else if (bit ^ 0x00)	// increment only if bit is 1
					{
						lineEnd += _textScale;
					}

					data >>= 1;
				}

				if (lineEnd == (charHeight - 1) * _textScale)	// we have a line that goes all the way to the bottom
				{
					const uint32_t totalPixels = uint32_t(lineEnd - lineStart + _textScale)*(uint32_t)_textScale;
					//setRowAddr(_y + lineStart, _y + lineEnd + _textScale - 1);
					setRowAddr(_y + lineStart, lineEnd - lineStart + _textScale);
					setRW();
					setDCForData();
					writeScanlineLooped(totalPixels);

					////fillRect(cx, cy + lineStart, _textScale, lineEnd - lineStart + _textScale, ILI9341_BLUEVIOLET);
					//setAddrAndRW_cont(_x, _y + lineStart, _textScale, lineEnd - lineStart + _textScale);
					//setDCForData();

					//for (uint8_t s = 0; s < _textScale; s++)
					//{
					//	writeScanline16(lineEnd - lineStart + _textScale);
					//	//delay(25);
					//}
				}
			}
		}
		//Serial << endl;
		_x += _textScale;
	}
	disableCS();	// to put CS line back up

	//_x = cx;
}

size_t ILI9341_due::print(char c) {
	_isFirstChar = true;
	beginTransaction();
	write(c);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(unsigned char c, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(c, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(int d, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(d, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(unsigned int u, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(u, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(long l, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(l, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(unsigned long ul, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(ul, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(double d, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(d, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::print(const Printable& str) {
	_isFirstChar = true;
	beginTransaction();
	Print::print(str);
	endTransaction();
	return 0;
}

size_t ILI9341_due::println(const __FlashStringHelper *str) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(str);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(const String &str) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(str);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(const char* str) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(str);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(char c) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(c);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(unsigned char c, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(c, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(int d, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(d, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(unsigned int u, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(u, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(long l, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(l, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(unsigned long ul, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(ul, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(double d, int b) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(d, b);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(const Printable& str) {
	_isFirstChar = true;
	beginTransaction();
	Print::println(str);
	endTransaction();
	return 0;
}
size_t ILI9341_due::println(void) {
	_isFirstChar = true;
	beginTransaction();
	Print::println();
	endTransaction();
	return 0;
}

size_t ILI9341_due::print(const char *str)
{
	beginTransaction();
	_isFirstChar = true;
	while (*str)
	{
		write((uint8_t)*str);
		//_isFirstChar = false;
		str++;
	}
	endTransaction();
	return 0;
}

size_t ILI9341_due::print(const String &str)
{
	beginTransaction();
	_isFirstChar = true;
	for (uint16_t i = 0; i < str.length(); i++)
	{
		write(str[i]);
		//_isFirstChar = false;
	}
	endTransaction();
	return 0;
}

size_t ILI9341_due::print(const __FlashStringHelper *str)
{
	beginTransaction();
	_isFirstChar = true;
	PGM_P p = reinterpret_cast<PGM_P>(str);
	uint8_t c;
	while ((c = pgm_read_byte(p)) != 0) {
		write(c);
		//_isFirstChar = false;
		p++;
	}
	endTransaction();
	return 0;
}

void ILI9341_due::printAt(const char *str, int16_t x, int16_t y)
{
	cursorToXY(x, y);
	print(str);
}

void ILI9341_due::printAt(const String &str, int16_t x, int16_t y)
{
	cursorToXY(x, y);
	print(str);
}

void ILI9341_due::printAt(const __FlashStringHelper *str, int16_t x, int16_t y)
{
	cursorToXY(x, y);
	print(str);
}

void ILI9341_due::printAt(const char *str, int16_t x, int16_t y, gTextEraseLine eraseLine)
{
	cursorToXY(x, y);
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnLeft(1024);
	print(str);
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnRight(1024);
}

void ILI9341_due::printAt(const String &str, int16_t x, int16_t y, gTextEraseLine eraseLine)
{
	cursorToXY(x, y);
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnLeft(1024);
	print(str);
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnRight(1024);
}

void ILI9341_due::printAt(const __FlashStringHelper *str, int16_t x, int16_t y, gTextEraseLine eraseLine)
{
	cursorToXY(x, y);
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnLeft(1024);
	print(str);
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		clearPixelsOnRight(1024);
}

void ILI9341_due::printAt(const char *str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	cursorToXY(x, y);

	// CLEAR PIXELS ON THE LEFT
	if (pixelsClearedOnLeft > 0)
		clearPixelsOnLeft(pixelsClearedOnLeft);

	print(str);

	// CLEAR PIXELS ON THE RIGHT
	if (pixelsClearedOnRight > 0)
		clearPixelsOnRight(pixelsClearedOnRight);
}

void ILI9341_due::printAt(const String &str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	cursorToXY(x, y);

	// CLEAR PIXELS ON THE LEFT
	if (pixelsClearedOnLeft > 0)
		clearPixelsOnLeft(pixelsClearedOnLeft);

	print(str);

	// CLEAR PIXELS ON THE RIGHT
	if (pixelsClearedOnRight > 0)
		clearPixelsOnRight(pixelsClearedOnRight);
}

void ILI9341_due::printAt(const __FlashStringHelper *str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	cursorToXY(x, y);

	// CLEAR PIXELS ON THE LEFT
	if (pixelsClearedOnLeft > 0)
		clearPixelsOnLeft(pixelsClearedOnLeft);

	print(str);

	// CLEAR PIXELS ON THE RIGHT
	if (pixelsClearedOnRight > 0)
		clearPixelsOnRight(pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const char *str, gTextAlign align)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, 0, 0);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const String &str, gTextAlign align)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, 0, 0);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const __FlashStringHelper *str, gTextAlign align)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, 0, 0);
}

void ILI9341_due::printAligned(const char *str, gTextAlign align, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAligned(const String &str, gTextAlign align, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAligned(const __FlashStringHelper *str, gTextAlign align, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const char *str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const String &str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAligned(const __FlashStringHelper *str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, 0, 0);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, 0, 0);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, 0, 0);
}

void ILI9341_due::printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, gTextPivotDefault, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAtPivoted(const char *str, int16_t x, int16_t y, gTextPivot pivot)
{
	cursorToXY(x, y);

	if (pivot != gTextPivotTopLeft && pivot != gTextPivotDefault)
		applyPivot(str, pivot, gTextAlignTopLeft);

	print(str);
}

void ILI9341_due::printAtPivoted(const String &str, int16_t x, int16_t y, gTextPivot pivot)
{
	cursorToXY(x, y);

	if (pivot != gTextPivotTopLeft && pivot != gTextPivotDefault)
		applyPivot(str, pivot, gTextAlignTopLeft);

	print(str);
}

void ILI9341_due::printAtPivoted(const __FlashStringHelper *str, int16_t x, int16_t y, gTextPivot pivot)
{
	cursorToXY(x, y);

	if (pivot != gTextPivotTopLeft && pivot != gTextPivotDefault)
		applyPivot(str, pivot, gTextAlignTopLeft);

	print(str);
}

void ILI9341_due::printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, 0, 0);
}

void ILI9341_due::printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, 0, 0);
}

void ILI9341_due::printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, 0, 0);
}

void ILI9341_due::printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	printAlignedPivotedOffseted(str, align, pivot, 0, 0, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, 0, 0);
}

void ILI9341_due::printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, 0, 0);
}

void ILI9341_due::printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY)
{
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, 0, 0);
}

void ILI9341_due::printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine)
{
	uint16_t pixelsClearedOnLeft = 0;
	uint16_t pixelsClearedOnRight = 0;
	if (eraseLine == gTextEraseFromBOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnLeft = 1024;
	if (eraseLine == gTextEraseToEOL || eraseLine == gTextEraseFullLine)
		pixelsClearedOnRight = 1024;
	printAlignedPivotedOffseted(str, align, pivot, offsetX, offsetY, pixelsClearedOnLeft, pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	//Serial << pixelsClearedOnLeft << " " << pixelsClearedOnRight << endl;
	_x = _xStart = _area.x;
	_y = _yStart = _area.y;

	applyAlignPivotOffset(str, align, pivot, offsetX, offsetY);

	clearPixelsOnLeft(pixelsClearedOnLeft);
	print(str);
	clearPixelsOnRight(pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	//Serial << pixelsClearedOnLeft << " " << pixelsClearedOnRight << endl;
	_x = _xStart = _area.x;
	_y = _yStart = _area.y;

	applyAlignPivotOffset(str, align, pivot, offsetX, offsetY);

	clearPixelsOnLeft(pixelsClearedOnLeft);
	print(str);
	clearPixelsOnRight(pixelsClearedOnRight);
}

void ILI9341_due::printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight)
{
	//Serial << pixelsClearedOnLeft << " " << pixelsClearedOnRight << endl;
	_x = _xStart = _area.x;
	_y = _yStart = _area.y;

	applyAlignPivotOffset(str, align, pivot, offsetX, offsetY);

	clearPixelsOnLeft(pixelsClearedOnLeft);
	print(str);
	clearPixelsOnRight(pixelsClearedOnRight);
}

__attribute__((always_inline))
void ILI9341_due::clearPixelsOnLeft(uint16_t pixelsToClearOnLeft) {
	// CLEAR PIXELS ON THE LEFT
	if (pixelsToClearOnLeft > 0)
	{
		int16_t clearX1 = max(min((int16_t)_x, (int16_t)_area.x), (int16_t)_x - (int16_t)pixelsToClearOnLeft);
		//Serial.println(clearX1);
		//Serial << "clearPixelsOnLeft " << _x << " " << _area.x << " " << clearX1 << endl2;
		fillRect(clearX1, _y, _x - clearX1, scaledFontHeight(), _fontBgColor);
	}
}

__attribute__((always_inline))
void ILI9341_due::clearPixelsOnRight(uint16_t pixelsToClearOnRight) {
	// CLEAR PIXELS ON THE RIGHT
	if (pixelsToClearOnRight > 0)
	{
		int16_t clearX2 = min(max((int16_t)_x, (int16_t)_area.x + (int16_t)_area.w - 1), (int16_t)_x + (int16_t)pixelsToClearOnRight);
		//Serial << "area from " << _area.x << " to " << _area.x1 << endl;
		//Serial << "clearing on right from " << _x << " to " << clearX2 << endl;
		fillRect(_x, _y, clearX2 - _x + 1, scaledFontHeight(), _fontBgColor);
		//TOTRY
		//fillRect(_x, _y, clearX2 - _x + 1, getFontHeight(), _fontBgColor);
	}
}

void ILI9341_due::applyAlignPivotOffset(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY) {
	//PIVOT
	if (pivot != gTextPivotTopLeft)
		applyPivot(str, pivot, align);

	// ALIGN & OFFSET
	applyAlignOffset(align, offsetX, offsetY);
}

void ILI9341_due::applyAlignPivotOffset(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY) {
	//PIVOT
	if (pivot != gTextPivotTopLeft)
		applyPivot(str, pivot, align);

	// ALIGN & OFFSET
	applyAlignOffset(align, offsetX, offsetY);
}

void ILI9341_due::applyAlignPivotOffset(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY) {
	//PIVOT
	if (pivot != gTextPivotTopLeft)
		applyPivot(str, pivot, align);

	// ALIGN & OFFSET
	applyAlignOffset(align, offsetX, offsetY);
}

void ILI9341_due::applyAlignOffset(gTextAlign align, int16_t offsetX, int16_t offsetY)
{
	if (align != gTextAlignTopLeft)
	{
		switch (align)
		{
		case gTextAlignTopCenter:
		{
			_x += _area.w / 2;
			break;
		}
		case gTextAlignTopRight:
		{
			_x += _area.w;
			break;
		}
		case gTextAlignMiddleLeft:
		{
			_y += _area.h / 2;
			break;
		}
		case gTextAlignMiddleCenter:
		{
			_x += _area.w / 2;
			_y += _area.h / 2;
			break;
		}
		case gTextAlignMiddleRight:
		{
			_x += _area.w;
			_y += _area.h / 2;
			break;
		}
		case gTextAlignBottomLeft:
		{
			_y += _area.h;
			break;
		}
		case gTextAlignBottomCenter:
		{
			_x += _area.w / 2;
			_y += _area.h;
			break;
		}
		case gTextAlignBottomRight:
		{
			_x += _area.w;
			_y += _area.h;
			break;
		}
		}
	}
	// OFFSET
	_x += offsetX;
	_y += offsetY;
}

void ILI9341_due::applyPivot(const String &str, gTextPivot pivot, gTextAlign align)
{
	applyPivot(str.c_str(), pivot, align);
}

void ILI9341_due::applyPivot(const __FlashStringHelper *str, gTextPivot pivot, gTextAlign align)
{
	//PIVOT
	if (pivot == gTextPivotDefault)
	{
		switch (align)
		{
		case gTextAlignTopLeft: { pivot = gTextPivotTopLeft; break;	}
		case gTextAlignTopCenter: { pivot = gTextPivotTopCenter; break;	}
		case gTextAlignTopRight: { pivot = gTextPivotTopRight; break; }
		case gTextAlignMiddleLeft: { pivot = gTextPivotMiddleLeft; break; }
		case gTextAlignMiddleCenter: { pivot = gTextPivotMiddleCenter; break; }
		case gTextAlignMiddleRight: { pivot = gTextPivotMiddleRight; break; }
		case gTextAlignBottomLeft: { pivot = gTextPivotBottomLeft; break; }
		case gTextAlignBottomCenter: { pivot = gTextPivotBottomCenter; break; }
		case gTextAlignBottomRight: { pivot = gTextPivotBottomRight; break; }
		}
	}

	if (pivot != gTextPivotTopLeft)
	{
		switch (pivot)
		{
		case gTextPivotTopCenter:
		{
			_x -= getStringWidth(str) / 2;
			break;
		}
		case gTextPivotTopRight:
		{
			_x -= getStringWidth(str);
			break;
		}
		case gTextPivotMiddleLeft:
		{
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotMiddleCenter:
		{
			_x -= getStringWidth(str) / 2;
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotMiddleRight:
		{
			_x -= getStringWidth(str);
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotBottomLeft:
		{
			_y -= scaledFontHeight();
			break;
		}
		case gTextPivotBottomCenter:
		{
			_x -= getStringWidth(str) / 2;
			_y -= scaledFontHeight();
			break;
		}
		case gTextPivotBottomRight:
		{
			_x -= getStringWidth(str);
			_y -= scaledFontHeight();
			break;
		}
		}
	}
}

void ILI9341_due::applyPivot(const char *str, gTextPivot pivot, gTextAlign align)
{
	//PIVOT
	if (pivot == gTextPivotDefault)
	{
		switch (align)
		{
		case gTextAlignTopLeft: { pivot = gTextPivotTopLeft; break;	}
		case gTextAlignTopCenter: { pivot = gTextPivotTopCenter; break;	}
		case gTextAlignTopRight: { pivot = gTextPivotTopRight; break; }
		case gTextAlignMiddleLeft: { pivot = gTextPivotMiddleLeft; break; }
		case gTextAlignMiddleCenter: { pivot = gTextPivotMiddleCenter; break; }
		case gTextAlignMiddleRight: { pivot = gTextPivotMiddleRight; break; }
		case gTextAlignBottomLeft: { pivot = gTextPivotBottomLeft; break; }
		case gTextAlignBottomCenter: { pivot = gTextPivotBottomCenter; break; }
		case gTextAlignBottomRight: { pivot = gTextPivotBottomRight; break; }
		}
	}

	if (pivot != gTextPivotTopLeft)
	{
		switch (pivot)
		{
		case gTextPivotTopCenter:
		{
			_x -= getStringWidth(str) / 2;
			break;
		}
		case gTextPivotTopRight:
		{
			_x -= getStringWidth(str);
			break;
		}
		case gTextPivotMiddleLeft:
		{
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotMiddleCenter:
		{
			_x -= getStringWidth(str) / 2;
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotMiddleRight:
		{
			_x -= getStringWidth(str);
			_y -= scaledFontHeight() / 2;
			break;
		}
		case gTextPivotBottomLeft:
		{
			_y -= scaledFontHeight();
			break;
		}
		case gTextPivotBottomCenter:
		{
			_x -= getStringWidth(str) / 2;
			_y -= scaledFontHeight();
			break;
		}
		case gTextPivotBottomRight:
		{
			_x -= getStringWidth(str);
			_y -= scaledFontHeight();
			break;
		}
		}
	}
}

void ILI9341_due::cursorTo(uint8_t column, uint8_t row)
{
	if (_font == 0)
		return; // no font selected

	/*
	* Text position is relative to current text area
	*/

	_x = _xStart = _area.x + column * (pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH) + 1);
	_y = _yStart = _area.y + row * (getFontHeight() + _lineSpacing) * _textScale;
	_isFirstChar = true;
	//#ifndef GLCD_NODEFER_SCROLL
	//	/*
	//	* Make sure to clear a deferred scroll operation when repositioning
	//	*/
	//	_needScroll = 0;
	//#endif
}

void ILI9341_due::cursorTo(int8_t column)
{
	if (_font == 0)
		return; // no font selected
	/*
	* Text position is relative to current text area
	* negative value moves the cursor backwards
	*/
	if (column >= 0)
		_x = _xStart = column * (pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH) + 1) + _area.x;
	else
		_x -= column * (pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH) + 1);

	_isFirstChar = true;

	//#ifndef GLCD_NODEFER_SCROLL
	//	/*
	//	* Make sure to clear a deferred scroll operation when repositioning
	//	*/
	//	_needScroll = 0;
	//#endif
}

__attribute__((always_inline))
void ILI9341_due::cursorToXY(int16_t x, int16_t y)
{
	/*
	* Text position is relative to current text area
	*/

	_x = _xStart = _area.x + x;
	_y = _yStart = _area.y + y;
	_isFirstChar = true;
	//Serial << F("cursorToXY x:") << x << F(" y:") << y << endl;

	//#ifndef GLCD_NODEFER_SCROLL
	//	/*
	//	* Make sure to clear a deferred scroll operation when repositioning
	//	*/
	//	_needScroll = 0;
	//#endif
}

void ILI9341_due::setTextScale(uint8_t s) {
#ifdef TEXT_SCALING_ENABLED
	_textScale = (s > 0) ? s : 1;
#endif
}


void ILI9341_due::eraseTextLine(uint16_t color, gTextEraseLine type)
{
	/*int16_t x = _x;
	int16_t y = _y;
	int16_t height = getFontHeight();*/
	//uint8_t color = (_fontColor == BLACK) ? WHITE : BLACK;

	switch (type)
	{
	case gTextEraseToEOL:
		fillRect(_x, _y, _area.x + _area.w - _x, scaledFontHeight(), color);
		break;
	case gTextEraseFromBOL:
		fillRect(_area.x, _y, _x - _area.x, scaledFontHeight(), color);
		break;
	case gTextEraseFullLine:
		fillRect(_area.x, _y, _area.w, scaledFontHeight(), color);
		break;
	}

	//cursorToXY(x, y);
		}

void ILI9341_due::eraseTextLine(uint16_t color, uint8_t row)
{
	cursorTo(0, row);
	eraseTextLine(color, gTextEraseToEOL);
}


void ILI9341_due::setFont(gTextFont font)
{
	_font = font;
}

void ILI9341_due::setTextColor(uint16_t color)
{
	_fontColor = color;
}

void ILI9341_due::setTextColor(uint8_t R, uint8_t G, uint8_t B)
{
	_fontColor = color565(R, G, B);
}

void ILI9341_due::setTextColor(uint16_t color, uint16_t backgroundColor)
{
	_fontColor = color;
	_fontBgColor = backgroundColor;
}

void ILI9341_due::setTextColor(uint8_t R, uint8_t G, uint8_t B, uint8_t bgR, uint8_t bgG, uint8_t bgB)
{
	_fontColor = color565(R, G, B);
	_fontBgColor = color565(bgR, bgG, bgB);
}

void ILI9341_due::setTextLineSpacing(uint8_t lineSpacing)
{
	_lineSpacing = lineSpacing;
}

void ILI9341_due::setTextLetterSpacing(uint8_t letterSpacing)
{
	_letterSpacing = letterSpacing;
}

//void ILI9341_due::setTextWrap(bool wrap)
//{
//	_wrap = wrap;
//}

void ILI9341_due::setFontMode(gTextFontMode fontMode)
{
	if (fontMode == gTextFontModeSolid || fontMode == gTextFontModeTransparent)
		_fontMode = fontMode;
}

uint16_t ILI9341_due::getCharWidth(uint8_t c)
{
	int16_t width = 0;

	if (isFixedWidthFont(_font) {
		width = (pgm_read_byte(_font + GTEXT_FONT_FIXED_WIDTH)) * _textScale;
	}
	else {
		// variable width font 
		uint8_t firstChar = pgm_read_byte(_font + GTEXT_FONT_FIRST_CHAR);
		uint8_t charCount = pgm_read_byte(_font + GTEXT_FONT_CHAR_COUNT);

		// read width data
		if (c >= firstChar && c < (firstChar + charCount)) {
			c -= firstChar;
			width = (pgm_read_byte(_font + GTEXT_FONT_WIDTH_TABLE + c)) * _textScale;
			//Serial << "strWidth of " << c << " : " << width << endl;
		}
	}
	return width;
}

uint16_t ILI9341_due::getStringWidth(const char* str)
{
	uint16_t width = 0;

	while (*str != 0) {
		width += getCharWidth(*str++) + _letterSpacing * _textScale;
	}
	if (width > 0)
		width -= _letterSpacing * _textScale;
	return width;
}

uint16_t ILI9341_due::getStringWidth(const __FlashStringHelper *str)
{
	PGM_P p = reinterpret_cast<PGM_P>(str);
	uint16_t width = 0;
	uint8_t c;
	while ((c = pgm_read_byte(p)) != 0) {
		width += getCharWidth(c) + _letterSpacing * _textScale;
		p++;
	}
	width -= _letterSpacing * _textScale;
	return width;
}

uint16_t ILI9341_due::getStringWidth(const String &str)
{
	uint16_t width = 0;

	for (uint16_t i = 0; i < str.length(); i++)
	{
		width += getCharWidth(str[i]) + _letterSpacing * _textScale;
	}
	width -= _letterSpacing * _textScale;
	return width;
}



#pragma GCC diagnostic pop 