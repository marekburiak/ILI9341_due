/*
v1.01.010

ILI9341_due.h - Arduino Due library for interfacing with ILI9341-based TFTs

Code: https://github.com/marekburiak/ILI9341_due
Documentation: http://marekburiak.github.io/ILI9341_due/

Copyright (c) 2015  Marek Buriak

2.4 TFT Pin-out
T_IRQ T_DO T_DIN T_CS T_CLK MISO LED CLK MOSI DC RESET CS GND VCC

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
MIT license, all text above must be included in any redistribution
****************************************************/

#ifndef _ILI9341_dueH_
#define _ILI9341_dueH_

#include <ILI9341_due_config.h>
//#include "../Streaming/Streaming.h"

#include "Arduino.h"

#ifdef ARDUINO_ARCH_AVR
#include <avr/pgmspace.h>
#endif

#ifdef ARDUINO_ARCH_AVR
#define SPI_MODE_NORMAL 1
#define SPI_MODE_EXTENDED 0
#define SPI_MODE_DMA 0
#else
#ifndef ILI9341_SPI_MODE_NORMAL
#define SPI_MODE_NORMAL 0
#else 
#define SPI_MODE_NORMAL 1
#endif

#ifndef ILI9341_SPI_MODE_EXTENDED
#define SPI_MODE_EXTENDED 0
#else 
#define SPI_MODE_EXTENDED 1
#endif

#ifndef ILI9341_SPI_MODE_DMA
#define SPI_MODE_DMA 0
#else
#define SPI_MODE_DMA 1
#endif
#endif

#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED | defined(ILI_USE_SPI_TRANSACTION)
#include <SPI.h>
#endif
#if SPI_MODE_DMA
#include <stdint.h>
#endif

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0D
#define ILI9341_RDDSPSGNMODE  0x0E
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_IDMOFF  0x38
#define ILI9341_IDMON   0x39
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC

*/

// Color definitions
#define ILI9341_ALICEBLUE 0xF7DF 
#define ILI9341_ANTIQUEWHITE 0xFF5A 
#define ILI9341_AQUA 0x07FF 
#define ILI9341_AQUAMARINE 0x7FFA 
#define ILI9341_AZURE 0xF7FF 
#define ILI9341_BEIGE 0xF7BB 
#define ILI9341_BISQUE 0xFF38 
#define ILI9341_BLACK 0x0000 
#define ILI9341_BLANCHEDALMOND 0xFF59 
#define ILI9341_BLUE 0x001F 
#define ILI9341_BLUEVIOLET 0x895C 
#define ILI9341_BROWN 0xA145 
#define ILI9341_BURLYWOOD 0xDDD0 
#define ILI9341_CADETBLUE 0x5CF4 
#define ILI9341_CHARTREUSE 0x7FE0 
#define ILI9341_CHOCOLATE 0xD343 
#define ILI9341_CORAL 0xFBEA 
#define ILI9341_CORNFLOWERBLUE 0x64BD 
#define ILI9341_CORNSILK 0xFFDB 
#define ILI9341_CRIMSON 0xD8A7 
#define ILI9341_CYAN 0x07FF 
#define ILI9341_DARKBLUE 0x0011 
#define ILI9341_DARKCYAN 0x0451 
#define ILI9341_DARKGOLDENROD 0xBC21 
#define ILI9341_DARKGRAY 0xAD55 
#define ILI9341_DARKGREEN 0x0320 
#define ILI9341_DARKKHAKI 0xBDAD 
#define ILI9341_DARKMAGENTA 0x8811 
#define ILI9341_DARKOLIVEGREEN 0x5345 
#define ILI9341_DARKORANGE 0xFC60 
#define ILI9341_DARKORCHID 0x9999 
#define ILI9341_DARKRED 0x8800 
#define ILI9341_DARKSALMON 0xECAF 
#define ILI9341_DARKSEAGREEN 0x8DF1 
#define ILI9341_DARKSLATEBLUE 0x49F1 
#define ILI9341_DARKSLATEGRAY 0x2A69 
#define ILI9341_DARKTURQUOISE 0x067A 
#define ILI9341_DARKVIOLET 0x901A 
#define ILI9341_DEEPPINK 0xF8B2 
#define ILI9341_DEEPSKYBLUE 0x05FF 
#define ILI9341_DIMGRAY 0x6B4D 
#define ILI9341_DODGERBLUE 0x1C9F 
#define ILI9341_FIREBRICK 0xB104 
#define ILI9341_FLORALWHITE 0xFFDE 
#define ILI9341_FORESTGREEN 0x2444 
#define ILI9341_FUCHSIA 0xF81F 
#define ILI9341_GAINSBORO 0xDEFB 
#define ILI9341_GHOSTWHITE 0xFFDF 
#define ILI9341_GOLD 0xFEA0 
#define ILI9341_GOLDENROD 0xDD24 
#define ILI9341_GRAY 0x8410 
#define ILI9341_GREEN 0x0400 
#define ILI9341_GREENYELLOW 0xAFE5 
#define ILI9341_HONEYDEW 0xF7FE 
#define ILI9341_HOTPINK 0xFB56 
#define ILI9341_INDIANRED 0xCAEB 
#define ILI9341_INDIGO 0x4810 
#define ILI9341_IVORY 0xFFFE 
#define ILI9341_KHAKI 0xF731 
#define ILI9341_LAVENDER 0xE73F 
#define ILI9341_LAVENDERBLUSH 0xFF9E 
#define ILI9341_LAWNGREEN 0x7FE0 
#define ILI9341_LEMONCHIFFON 0xFFD9 
#define ILI9341_LIGHTBLUE 0xAEDC 
#define ILI9341_LIGHTCORAL 0xF410 
#define ILI9341_LIGHTCYAN 0xE7FF 
#define ILI9341_LIGHTGOLDENRODYELLOW 0xFFDA 
#define ILI9341_LIGHTGREEN 0x9772 
#define ILI9341_LIGHTGREY 0xD69A 
#define ILI9341_LIGHTPINK 0xFDB8 
#define ILI9341_LIGHTSALMON 0xFD0F 
#define ILI9341_LIGHTSEAGREEN 0x2595 
#define ILI9341_LIGHTSKYBLUE 0x867F 
#define ILI9341_LIGHTSLATEGRAY 0x7453 
#define ILI9341_LIGHTSTEELBLUE 0xB63B 
#define ILI9341_LIGHTYELLOW 0xFFFC 
#define ILI9341_LIME 0x07E0 
#define ILI9341_LIMEGREEN 0x3666 
#define ILI9341_LINEN 0xFF9C 
#define ILI9341_MAGENTA 0xF81F 
#define ILI9341_MAROON 0x8000 
#define ILI9341_MEDIUMAQUAMARINE 0x6675 
#define ILI9341_MEDIUMBLUE 0x0019 
#define ILI9341_MEDIUMORCHID 0xBABA 
#define ILI9341_MEDIUMPURPLE 0x939B 
#define ILI9341_MEDIUMSEAGREEN 0x3D8E 
#define ILI9341_MEDIUMSLATEBLUE 0x7B5D 
#define ILI9341_MEDIUMSPRINGGREEN 0x07D3 
#define ILI9341_MEDIUMTURQUOISE 0x4E99 
#define ILI9341_MEDIUMVIOLETRED 0xC0B0 
#define ILI9341_MIDNIGHTBLUE 0x18CE 
#define ILI9341_MINTCREAM 0xF7FF 
#define ILI9341_MISTYROSE 0xFF3C 
#define ILI9341_MOCCASIN 0xFF36 
#define ILI9341_NAVAJOWHITE 0xFEF5 
#define ILI9341_NAVY 0x0010 
#define ILI9341_OLDLACE 0xFFBC 
#define ILI9341_OLIVE 0x8400 
#define ILI9341_OLIVEDRAB 0x6C64 
#define ILI9341_ORANGE 0xFD20 
#define ILI9341_ORANGERED 0xFA20 
#define ILI9341_ORCHID 0xDB9A 
#define ILI9341_PALEGOLDENROD 0xEF55 
#define ILI9341_PALEGREEN 0x9FD3 
#define ILI9341_PALETURQUOISE 0xAF7D 
#define ILI9341_PALEVIOLETRED 0xDB92 
#define ILI9341_PAPAYAWHIP 0xFF7A 
#define ILI9341_PEACHPUFF 0xFED7 
#define ILI9341_PERU 0xCC27 
#define ILI9341_PINK 0xFE19 
#define ILI9341_PLUM 0xDD1B 
#define ILI9341_POWDERBLUE 0xB71C 
#define ILI9341_PURPLE 0x8010 
#define ILI9341_RED 0xF800 
#define ILI9341_ROSYBROWN 0xBC71 
#define ILI9341_ROYALBLUE 0x435C 
#define ILI9341_SADDLEBROWN 0x8A22 
#define ILI9341_SALMON 0xFC0E 
#define ILI9341_SANDYBROWN 0xF52C 
#define ILI9341_SEAGREEN 0x2C4A 
#define ILI9341_SEASHELL 0xFFBD 
#define ILI9341_SIENNA 0xA285 
#define ILI9341_SILVER 0xC618 
#define ILI9341_SKYBLUE 0x867D 
#define ILI9341_SLATEBLUE 0x6AD9 
#define ILI9341_SLATEGRAY 0x7412 
#define ILI9341_SNOW 0xFFDF 
#define ILI9341_SPRINGGREEN 0x07EF 
#define ILI9341_STEELBLUE 0x4416 
#define ILI9341_TAN 0xD5B1 
#define ILI9341_TEAL 0x0410 
#define ILI9341_THISTLE 0xDDFB 
#define ILI9341_TOMATO 0xFB08 
#define ILI9341_TURQUOISE 0x471A 
#define ILI9341_VIOLET 0xEC1D 
#define ILI9341_WHEAT 0xF6F6 
#define ILI9341_WHITE 0xFFFF 
#define ILI9341_WHITESMOKE 0xF7BE 
#define ILI9341_YELLOW 0xFFE0 
#define ILI9341_YELLOWGREEN 0x9E66

// Font Indices
#define GTEXT_FONT_LENGTH			0
#define GTEXT_FONT_FIXED_WIDTH	2
#define GTEXT_FONT_HEIGHT			3
#define GTEXT_FONT_FIRST_CHAR		4
#define GTEXT_FONT_CHAR_COUNT		5
#define GTEXT_FONT_WIDTH_TABLE	6

typedef enum  {
	gTextFontModeSolid = 0,
	gTextFontModeTransparent = 1
} gTextFontMode;

// the following returns true if the given font is fixed width
// zero length is flag indicating fixed width font (array does not contain width data entries)
#define isFixedWidthFont(font)  (pgm_read_byte(font+GTEXT_FONT_LENGTH) == 0 && pgm_read_byte(font+GTEXT_FONT_LENGTH+1) == 0))

typedef enum  {
	gTextAlignTopLeft,
	gTextAlignTopCenter,
	gTextAlignTopRight,
	gTextAlignMiddleLeft,
	gTextAlignMiddleCenter,
	gTextAlignMiddleRight,
	gTextAlignBottomLeft,
	gTextAlignBottomCenter,
	gTextAlignBottomRight
} gTextAlign;

typedef enum {
	gTextPivotDefault,
	gTextPivotTopLeft,
	gTextPivotTopCenter,
	gTextPivotTopRight,
	gTextPivotMiddleLeft,
	gTextPivotMiddleCenter,
	gTextPivotMiddleRight,
	gTextPivotBottomLeft,
	gTextPivotBottomCenter,
	gTextPivotBottomRight
} gTextPivot;

typedef enum {
	gTextEraseToEOL = 0x01, 	/**< Erase From cursor to end of Line */
	gTextEraseFromBOL = 0x02,	/**< Erase From Begining of Line to Cursor*/
	gTextEraseFullLine = 0x03	/**< Erase Entire line */
} gTextEraseLine;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
} gTextArea;

typedef const uint8_t* gTextFont;

typedef enum {
	iliRotation0 = 0,
	iliRotation90 = 1,
	iliRotation180 = 2,
	iliRotation270 = 3
} iliRotation;

typedef enum {
	// Normal Mode On (full display)
	// In this mode, the display is able to show maximum 262,144 colors. 
	pwrLevelNormal = 1,
	// Idle Mode On
	// In this mode, the full display area is used but with 8 colors. 
	pwrLevelIdle = 2,
	// In this mode, the DC : DC converter, Internal oscillator and panel driver circuit are stopped. Only the MCU 
	// interface and memory works with VDDI power supply. Contents of the memory are safe. 
	pwrLevelSleep = 3
} pwrLevel;

#ifndef swap
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

#ifdef ARDUINO_SAM_DUE
#define SCANLINE_PIXEL_COUNT 320
#elif defined ARDUINO_ARCH_AVR
#define SCANLINE_PIXEL_COUNT 16
#endif

#if SPI_MODE_DMA | SPI_MODE_EXTENDED
#define SCANLINE_BUFFER_SIZE SCANLINE_PIXEL_COUNT << 1 
#elif SPI_MODE_NORMAL
#define SCANLINE_BUFFER_SIZE SCANLINE_PIXEL_COUNT
#endif

class ILI9341_due
	: public Print
{
protected:
	int16_t _width, _height; // Display w/h as modified by current rotation
	iliRotation	_rotation;

	float _arcAngleMax;
	int16_t _angleOffset;

	void fillArcOffsetted(uint16_t cx, uint16_t cy, uint16_t radius, uint16_t thickness, float startAngle, float endAngle, uint16_t color);

	void drawFastVLine_cont_noFill(int16_t x, int16_t y, int16_t h, uint16_t color);
	void drawFastVLine_noTrans(int16_t x, int16_t y, uint16_t h, uint16_t color);
	void drawFastHLine_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t color);
	void drawLine_noTrans(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void printHex8(uint8_t *data, uint8_t length);
	void printHex16(uint16_t *data, uint8_t length);
	void printHex32(uint32_t *data, uint8_t length);

	uint8_t  _rst;
	uint8_t _hiByte, _loByte;
	bool _isIdle, _isInSleep;
	uint16_t _color;

	uint16_t _scanline16[SCANLINE_PIXEL_COUNT];
//#if SPI_MODE_DMA | SPI_MODE_EXTENDED
//	uint8_t _scanline[SCANLINE_BUFFER_SIZE];
//
//#elif SPI_MODE_NORMAL
//	uint16_t _scanline[SCANLINE_BUFFER_SIZE];
//#endif

	//uint16_t _scanlineCounter = 0;
#if SPI_MODE_DMA | SPI_MODE_EXTENDED

	//uint16_t _scanline[SCANLINE_BUFFER_SIZE];	 // DMA16
#endif

	uint8_t _spiClkDivider;
#ifdef ILI_USE_SPI_TRANSACTION
	SPISettings _spiSettings;
	uint8_t _transactionId;
	bool _isInTransaction;
#endif
	//Pio *_dcport;
#ifdef ARDUINO_SAM_DUE
	volatile RwReg *_dcport;
	uint32_t  _cs, _dc, _dcpinmask;
#else
	volatile uint8_t *_dcport, *_csport;
	volatile uint8_t _dcport2;
	uint8_t  _cs, _dc, _cspinmask, _dcpinmask, _backupSPCR;
#endif


#if SPI_MODE_NORMAL | SPI_MODE_DMA
#ifdef ARDUINO_SAM_DUE
	//Pio *_csport;
	volatile RwReg *_csport;
	uint32_t _cspinmask;
#endif
#endif

	gTextFontMode _fontMode;
	uint16_t _fontColor;
	uint16_t _fontBgColor;
	gTextFont _font;
	gTextArea _area;
	int16_t	_x;
	int16_t	_xStart;
	int16_t	_y;
	int16_t	_yStart;
	uint8_t _letterSpacing;
	uint8_t _lineSpacing;
	bool _isFirstChar;
	//bool _needScroll;
	//bool _wrap;

#ifdef TEXT_SCALING_ENABLED
	uint8_t _textScale;
#else
#define _textScale 1
#endif
	void fillRect_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
	void fillRectWithShader_noTrans(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t(*fillShader)(uint16_t rx, uint16_t ry));
	void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	void pushColors_noTrans_noCS(const uint16_t *colors, uint16_t offset, uint32_t len);

	void specialChar(uint8_t c);
	void drawSolidChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight);
	void drawTransparentChar(char c, uint16_t index, uint16_t charWidth, uint16_t charHeight);
	void applyPivot(const char *str, gTextPivot pivot, gTextAlign align);
	void applyPivot(const String &str, gTextPivot pivot, gTextAlign align);
	void applyPivot(const __FlashStringHelper *str, gTextPivot pivot, gTextAlign align);
	void applyAlignOffset(gTextAlign align, int16_t offsetX, int16_t offsetY);
	void applyAlignPivotOffset(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);
	void applyAlignPivotOffset(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);
	void applyAlignPivotOffset(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);
	void clearPixelsOnLeft(uint16_t pixelsToClearOnLeft);
	void clearPixelsOnRight(uint16_t pixelsToClearOnRight);

	bool pinIsChipSelect(uint8_t cs);

public:
	ILI9341_due(uint8_t cs, uint8_t dc, uint8_t rst = 255);


	bool begin(void);
	void getDisplayStatus();
	void fillScreen(uint16_t color);
	void fillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
	void fillRectWithShader(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t(*fillShader)(uint16_t rx, uint16_t ry));

	void pushColor(uint16_t color);
	void pushColors(const uint16_t *colors, uint16_t offset, uint32_t len);
	void pushColors(uint16_t *colors, uint16_t offset, uint32_t len);
	/*void pushColors565(uint8_t *colors, uint16_t offset, uint32_t len);
	void pushColors565(const uint16_t *colors, uint16_t offset, uint32_t len);*/

	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void drawFastVLine(int16_t x, int16_t y, uint16_t h, uint16_t color);
	void drawFastHLine(int16_t x, int16_t y, uint16_t w, uint16_t color);
	void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);

	void setRotation(iliRotation r);
	void invertDisplay(boolean i);
	void display(boolean d);
	void sleep(boolean s);
	void idle(boolean i);
	void setPowerLevel(pwrLevel p);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void setAddrWindowRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	void setSPIClockDivider(uint8_t divider);
	void setAngleOffset(int16_t angleOffset);
	void setArcParams(float arcAngleMax);

	uint16_t readPixel(int16_t x, int16_t y);
	void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
	void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void drawRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
	void fillRoundRect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t radius, uint16_t color);
	void drawBitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color);
	void drawBitmap(const uint8_t *bitmap, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t color, uint16_t bgcolor);
	void drawImage(const uint16_t *colors, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
	uint8_t getRotation(void);
	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void drawLineByAngle(int16_t x, int16_t y, int16_t angle, uint16_t length, uint16_t color);
	void drawLineByAngle(int16_t x, int16_t y, int16_t angle, uint16_t start, uint16_t length, uint16_t color);

	void screenshotToConsole();
	
	void setTextArea(gTextArea area);
	void setTextArea(int16_t x, int16_t y, int16_t w, int16_t h); //, textMode mode=DEFAULT_SCROLLDIR);
	void setTextArea(int16_t x, int16_t y, int16_t columns, int16_t rows, gTextFont font); //, textMode mode=DEFAULT_SCROLLDIR);
	gTextArea getTextArea()
	{
		return _area;
	}

	void clearTextArea();
	void clearTextArea(gTextArea area);
	void clearTextArea(uint16_t color);
	void clearTextArea(gTextArea area, uint16_t color);

	// Font Functions
	void setFont(gTextFont font);
	gTextFont getFont() {
		return _font;
	}
	void setTextColor(uint16_t color);
	void setTextColor(uint8_t R, uint8_t G, uint8_t B);
	void setTextColor(uint16_t color, uint16_t backgroundColor);
	void setTextColor(uint8_t R, uint8_t G, uint8_t B, uint8_t bgR, uint8_t bgG, uint8_t bgB);
	uint16_t getTextColor()
	{
		return _fontColor;
	}

	uint16_t getTextBackgroundColor()
	{
		return _fontBgColor;
	}

	void setTextLetterSpacing(uint8_t letterSpacing);
	uint8_t getTextLetterSpacing()	{
		return _letterSpacing;
	};

	void setTextLineSpacing(uint8_t lineSpacing);
	uint8_t getTextLineSpacing()	{
		return _lineSpacing;
	};

	//void setTextWrap(bool wrap);
	void setFontMode(gTextFontMode fontMode);
	gTextFontMode getFontMode()
	{
		return _fontMode;
	}

	void setTextScale(uint8_t textScale);
	uint8_t getTextScale() {
		return _textScale;
	}

	//void puts(const char *str);
	//void puts(const String &str);
	//void puts(const __FlashStringHelper* str);

	void printAt(const char *str, int16_t x, int16_t y);
	void printAt(const String &str, int16_t x, int16_t y);
	void printAt(const __FlashStringHelper* str, int16_t x, int16_t y);

	void printAt(const char *str, int16_t x, int16_t y, gTextEraseLine eraseLine);
	void printAt(const String &str, int16_t x, int16_t y, gTextEraseLine eraseLine);
	void printAt(const __FlashStringHelper *str, int16_t x, int16_t y, gTextEraseLine eraseLine);

	void printAt(const char *str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAt(const String &str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAt(const __FlashStringHelper *str, int16_t x, int16_t y, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);

	void printAtPivoted(const char *str, int16_t x, int16_t y, gTextPivot pivot);
	void printAtPivoted(const String &str, int16_t x, int16_t y, gTextPivot pivot);
	void printAtPivoted(const __FlashStringHelper *str, int16_t x, int16_t y, gTextPivot pivot);

	void printAligned(const char *str, gTextAlign align);
	void printAligned(const String &str, gTextAlign align);
	void printAligned(const __FlashStringHelper *str, gTextAlign align);

	void printAligned(const char *str, gTextAlign align, gTextEraseLine eraseLine);
	void printAligned(const String &str, gTextAlign align, gTextEraseLine eraseLine);
	void printAligned(const __FlashStringHelper *str, gTextAlign align, gTextEraseLine eraseLine);

	void printAligned(const char *str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAligned(const String &str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAligned(const __FlashStringHelper *str, gTextAlign align, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);

	void printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY);
	void printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY);
	void printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY);

	void printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);
	void printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);
	void printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);

	void printAlignedOffseted(const char *str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedOffseted(const String &str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedOffseted(const __FlashStringHelper *str, gTextAlign align, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);

	void printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot);
	void printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot);
	void printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot);

	void printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine);
	void printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine);
	void printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, gTextEraseLine eraseLine);

	void printAlignedPivoted(const char *str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedPivoted(const String &str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedPivoted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);

	void printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);
	void printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);
	void printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY);

	void printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);
	void printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);
	void printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, gTextEraseLine eraseLine);

	void printAlignedPivotedOffseted(const char *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedPivotedOffseted(const String &str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);
	void printAlignedPivotedOffseted(const __FlashStringHelper *str, gTextAlign align, gTextPivot pivot, int16_t offsetX, int16_t offsetY, uint16_t pixelsClearedOnLeft, uint16_t pixelsClearedOnRight);


	void cursorTo(uint8_t column, uint8_t row); // 0 based coordinates for character columns and rows
	void cursorTo(int8_t column); // move cursor on the current row
	void cursorToXY(int16_t x, int16_t y); // coordinates relative to active text area


	__attribute__((always_inline))
		uint8_t getFontHeight()	{
		return pgm_read_byte(_font + GTEXT_FONT_HEIGHT);
	};

	__attribute__((always_inline))
		static uint8_t getFontHeight(gTextFont font) {
		return pgm_read_byte(font + GTEXT_FONT_HEIGHT);
	};

	uint8_t readcommand8(uint8_t c) {
		writecommand_cont(0xD9);  // woo sekret command?
		writedata8_last(0x10);
		writecommand_cont(c);
		return readdata8_last();
	}

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

	int16_t width() {
		return _width;
	}

	int16_t height() {
		return _height;
	}


	virtual size_t write(uint8_t);
	virtual size_t print(const __FlashStringHelper *);
	virtual size_t print(const String &);
	virtual size_t print(const char[]);
	virtual size_t print(char);
	virtual size_t print(unsigned char, int = DEC);
	virtual size_t print(int, int = DEC);
	virtual size_t print(unsigned int, int = DEC);
	virtual size_t print(long, int = DEC);
	virtual size_t print(unsigned long, int = DEC);
	virtual size_t print(double, int = 2);
	virtual size_t print(const Printable&);

	virtual size_t println(const __FlashStringHelper *);
	virtual size_t println(const String &s);
	virtual size_t println(const char[]);
	virtual size_t println(char);
	virtual size_t println(unsigned char, int = DEC);
	virtual size_t println(int, int = DEC);
	virtual size_t println(unsigned int, int = DEC);
	virtual size_t println(long, int = DEC);
	virtual size_t println(unsigned long, int = DEC);
	virtual size_t println(double, int = 2);
	virtual size_t println(const Printable&);
	virtual size_t println(void);

	uint16_t getCharWidth(uint8_t c);
	uint16_t getStringWidth(const char* str);
	uint16_t getStringWidth(const __FlashStringHelper *str);
	uint16_t getStringWidth(const String &str);

	void eraseTextLine(uint16_t color, gTextEraseLine type = gTextEraseToEOL); //ansi like line erase function 
	void eraseTextLine(uint16_t color, uint8_t row); // erase the entire text line in the given row and move cursor to left position

	int16_t getCursorX() {
		return _x;
	}

	int16_t getCursorY() {
		return _y;
	}

	static uint16_t getCharWidth(uint8_t c, gTextFont font, uint8_t textScale)
	{
		int16_t width = 0;

		if (isFixedWidthFont(font){
			width = (pgm_read_byte(font + GTEXT_FONT_FIXED_WIDTH)) * textScale;
		}
		else{
			// variable width font 
			uint8_t firstChar = pgm_read_byte(font + GTEXT_FONT_FIRST_CHAR);
			uint8_t charCount = pgm_read_byte(font + GTEXT_FONT_CHAR_COUNT);

			// read width data
			if (c >= firstChar && c < (firstChar + charCount)) {
				c -= firstChar;
				width = (pgm_read_byte(font + GTEXT_FONT_WIDTH_TABLE + c)) * textScale;
			}
		}
		return width;
	};

	static uint16_t getStringWidth(const char* str, gTextFont font, uint8_t letterSpacing, uint8_t textScale)
	{
		uint16_t width = 0;

		while (*str != 0) {
			width += getCharWidth(*str++, font, textScale) + letterSpacing  * textScale;
		}
		if (width > 0)
			width -= letterSpacing * textScale;

		return width;
	};

	inline __attribute__((always_inline))
		void fillArc(uint16_t x, uint16_t y, uint16_t radius, uint16_t thickness, float start, float end, uint16_t color)
	{
		beginTransaction();
		if (start == 0 && end == _arcAngleMax)
			fillArcOffsetted(x, y, radius, thickness, 0, _arcAngleMax, color);
		else
			fillArcOffsetted(x, y, radius, thickness, start + (_angleOffset / (float)360)*_arcAngleMax, end + (_angleOffset / (float)360)*_arcAngleMax, color);
		endTransaction();
	}

	//int32_t cos_lookup(int32_t angle)
	//{
	//	float radians = (float)angle/_arcAngleMax * 2 * PI;
	//	//Serial << "COS_LOOKUP angle:" << (float)angle << " radians:" << radians << " cos:" << cos(radians) << " return: " << cos(radians) * (double)65535 << endl;
	//	return (cos(radians) * _arcAngleMax);
	//}

	//int32_t sin_lookup(int32_t angle)
	//{
	//	float radians = (float)angle/_arcAngleMax * 2 * PI;
	//	//Serial << "SIN_LOOKUP angle:" << (float)angle << " radians:" << radians << " sin:" << sin(radians) << " return: " << sin(radians) * (double)65535 << endl;
	//	return (sin(radians) * _arcAngleMax);
	//}


	float cosDegrees(float angle)
	{
		//float radians = angle / (float)360 * 2 * PI;
		//Serial << "COS_LOOKUP angle:" << (float)angle << " radians:" << radians << " cos:" << cos(radians) << " return: " << cos(radians) * (double)65535 << endl;
		return cos(angle * DEG_TO_RAD);
	}

	float sinDegrees(float angle)
	{
		//float radians = angle / (float)360 * 2 * PI;
		//Serial << "SIN_LOOKUP angle:" << (float)angle << " radians:" << radians << " sin:" << sin(radians) << " return: " << sin(radians) * (double)65535 << endl;
		return sin(angle * DEG_TO_RAD);
	}

protected:

	__attribute__((always_inline))
		uint16_t scaledFontHeight()	{
		return (uint16_t)(pgm_read_byte(_font + GTEXT_FONT_HEIGHT)) * (uint16_t)_textScale;
	};


	__attribute__((always_inline))
		void beginTransaction() {
#ifdef ILI_USE_SPI_TRANSACTION
#if defined ARDUINO_ARCH_AVR
		SPI.beginTransaction(_spiSettings);
#elif defined (ARDUINO_SAM_DUE)
#if SPI_MODE_NORMAL
		SPI.beginTransaction(_spiSettings);
#elif SPI_MODE_EXTENDED
		SPI.beginTransaction(_cs, _spiSettings);
#elif SPI_MODE_DMA
		SPI.beginTransaction(_spiSettings);
		dmaInit(_spiClkDivider);
#endif
#endif
#endif
	}

	__attribute__((always_inline))
		void endTransaction() {
#ifdef ILI_USE_SPI_TRANSACTION
#if defined ARDUINO_ARCH_AVR
		SPI.endTransaction();
#elif defined (ARDUINO_SAM_DUE)
		SPI.endTransaction();
#endif
#endif
	}

	__attribute__((always_inline))
		void spiwrite(uint8_t c) {
#if defined ARDUINO_ARCH_AVR
		SPDR = c;
		asm volatile("nop");
		while (!(SPSR & _BV(SPIF))); // wait
#elif defined (ARDUINO_SAM_DUE)
#if SPI_MODE_NORMAL
		SPI.transfer(c);
#endif
#endif
	}

	__attribute__((always_inline))
		void spiwrite16(uint16_t d) {
#if defined ARDUINO_ARCH_AVR
		SPDR = highByte(d);
		while (!(SPSR & _BV(SPIF)));
		SPDR = lowByte(d);
		while (!(SPSR & _BV(SPIF)));
#elif defined (ARDUINO_SAM_DUE)
#if SPI_MODE_NORMAL
		SPI.transfer(highByte(d));
		SPI.transfer(lowByte(d));
#endif
#endif
	}

	// writes 1 byte
	// CS and DC have to be set prior to calling this method
	__attribute__((always_inline))
		void write8_cont(uint8_t c){
#if SPI_MODE_NORMAL
		spiwrite(c);
#elif SPI_MODE_EXTENDED
		SPI.transfer(_cs, c, SPI_CONTINUE);
#elif SPI_MODE_DMA
		dmaSend(c);
#endif
	}

	// writes 1 byte and disables CS
	// CS and DC have to be set prior to calling this method
	inline __attribute__((always_inline))
		void write8_last(uint8_t c) {
#if SPI_MODE_NORMAL
		spiwrite(c);
		disableCS();
#elif SPI_MODE_EXTENDED
		SPI.transfer(_cs, c, SPI_LAST);
#elif SPI_MODE_DMA
		dmaSend(c);
		disableCS();
#endif
	}

	// Writes 2 bytes
	// CS, DC have to be set prior to calling this method
	__attribute__((always_inline))
		void write16_cont(uint16_t d) {
#if SPI_MODE_NORMAL
		spiwrite16(d);
#elif SPI_MODE_EXTENDED
		SPI.transfer(_cs, highByte(d), SPI_CONTINUE);
		SPI.transfer(_cs, lowByte(d), SPI_CONTINUE);
#elif SPI_MODE_DMA
		dmaSend(highByte(d));
		dmaSend(lowByte(d));
#endif
	}

	__attribute__((always_inline))
		void write16_last(uint16_t d) {
#if SPI_MODE_NORMAL
		spiwrite16(d);
		disableCS();
#elif SPI_MODE_EXTENDED
		SPI.transfer(_cs, highByte(d), SPI_CONTINUE);
		SPI.transfer(_cs, lowByte(d), SPI_LAST);
#elif SPI_MODE_DMA
		dmaSend(highByte(d));
		dmaSend(lowByte(d));
		disableCS();
#endif
	}

	// Writes commands to set the GRAM area where data/pixels will be written
	void setAddr_cont(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
		__attribute__((always_inline)) {
		writecommand_cont(ILI9341_CASET); // Column addr set
		setDCForData();
		write16_cont(x);   // XSTART
		write16_cont(x+w-1);   // XEND
		writecommand_cont(ILI9341_PASET); // Row addr set
		setDCForData();
		write16_cont(y);   // YSTART
		write16_cont(y+h-1);   // YEND
	}

	//__attribute__((always_inline))
	//void setAddrAndRW_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	//	 {
	//		writecommand_cont(ILI9341_CASET); // Column addr set
	//		writedata16_cont(x0);   // XSTART
	//		writedata16_cont(x1);   // XEND
	//		writecommand_cont(ILI9341_PASET); // Row addr set
	//		writedata16_cont(y0);   // YSTART
	//		writedata16_cont(y1);   // YEND
	//		writecommand_cont(ILI9341_RAMWR); // write to RAM
	//}

	// Enables CS, writes commands to set the GRAM area where data/pixels will be written
	//__attribute__((always_inline))
	//	void setAddr_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	//{
	//	enableCS();
	//	setDCForCommand();
	//	write8_cont(ILI9341_CASET); // Column addr set
	//	setDCForData();
	//	write16_cont(x0);   // XSTART
	//	write16_cont(x1);   // XEND
	//	setDCForCommand();
	//	write8_cont(ILI9341_PASET); // Row addr set
	//	setDCForData();
	//	write16_cont(y0);   // XSTART
	//	write16_cont(y1);   // XEND
	//}

	// Enables CS, writes commands to set the GRAM area where data/pixels will be written
	// Also sends RAM WRITE command which should be followed by writing data/pixels
#ifdef ARDUINO_SAM_DUE
	inline __attribute__((always_inline))
#endif
		void setAddrAndRW_cont(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
	{
		setDCForCommand();
		write8_cont(ILI9341_CASET); // Column addr set
		setDCForData();
		write16_cont(x);   // XSTART
		write16_cont(x+w-1);   // XEND
		setDCForCommand();
		write8_cont(ILI9341_PASET); // Row addr set
		setDCForData();
		write16_cont(y);   // YSTART
		write16_cont(y+h-1);   // YEND
		setDCForCommand();
		write8_cont(ILI9341_RAMWR); // RAM write
	}

	inline __attribute__((always_inline))
		void setAddrAndRW_cont_inline(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
	{
		setDCForCommand();
		write8_cont(ILI9341_CASET); // Column addr set
		setDCForData();
		write16_cont(x);   // XSTART
		write16_cont(x+w-1);   // XEND
		setDCForCommand();
		write8_cont(ILI9341_PASET); // Row addr set
		setDCForData();
		write16_cont(y);   // YSTART
		write16_cont(y+h-1);   // YEND
		setDCForCommand();
		write8_cont(ILI9341_RAMWR); // RAM write
	}

#ifdef ARDUINO_SAM_DUE
	inline __attribute__((always_inline))
#endif
		void setColumnAddr(uint16_t x, uint16_t w)
	{
		setDCForCommand();
		write8_cont(ILI9341_CASET); // Column addr set
		setDCForData();
		write16_cont(x);   // XSTART
		write16_cont(x+w-1);   // XEND
	}

#ifdef ARDUINO_SAM_DUE
	inline __attribute__((always_inline))
#endif
		void setRowAddr(uint16_t y, uint16_t h)
	{
		setDCForCommand();
		write8_cont(ILI9341_PASET); // Row addr set
		setDCForData();
		write16_cont(y);   // YSTART
		write16_cont(y+h-1);   // YEND
	}

	inline __attribute__((always_inline))
		void setRW()	// RAM Write
	{
		setDCForCommand();
		write8_cont(ILI9341_RAMWR); // RAM write
	}

	// Writes commands to set the GRAM area where data/pixels will be written
	// Also sends RAM WRITE command which should be followed by writing data/pixels 
	// CS has to be enabled prior to calling this method
	//__attribute__((always_inline))
	//	void setAddrAndRW_cont_noCS(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	//{
	//	setDCForCommand();
	//	write8_cont(ILI9341_CASET); // Column addr set
	//	setDCForData();
	//	write16_cont(x0);   // XSTART
	//	write16_cont(x1);   // XEND
	//	setDCForCommand();
	//	write8_cont(ILI9341_PASET); // Row addr set
	//	setDCForData();
	//	write16_cont(y0);   // XSTART
	//	write16_cont(y1);   // XEND
	//	setDCForCommand();
	//	write8_cont(ILI9341_RAMWR); // Row addr set
	//}

	// Enables CS, sets DC for Command, writes 1 byte
	// Does not disable CS
	//	__attribute__((always_inline))
	//	void writecommand_cont(uint8_t c) {
	//		setDCForCommand();
	//#if SPI_MODE_NORMAL
	//		enableCS();
	//		SPI.transfer(c);
	//#elif SPI_MODE_EXTENDED
	//		SPI.transfer(_cs, c, SPI_CONTINUE); 
	//#elif SPI_MODE_DMA
	//		enableCS();
	//		dmaSend(c);
	//#endif
	//	}

	// Enables CS, sets DC for Command, writes 1 byte
	// Does not disable CS
	inline __attribute__((always_inline))
		void writecommand_cont(uint8_t c) {
		setDCForCommand();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		write8_cont(c);
	}

	// Enables CS, sets DC for Command, writes 1 byte, disables CS
	inline __attribute__((always_inline))
		void writecommand_last(uint8_t c) {
		setDCForCommand();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		write8_last(c);
	}

	// Enables CS, sets DC to Data, writes 1 byte
	// Does not disable CS
	inline __attribute__((always_inline))
		void writedata8_cont(uint8_t c) {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		write8_cont(c);
	}

	// Enables CS, sets DC to Data, writes 1 byte, disables CS
	__attribute__((always_inline))
		void writedata8_last(uint8_t c) {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		write8_last(c);
	}

	//	// Enables CS, sets DC to Data, writes 2 bytes
	//	// Does not disable CS
	//	__attribute__((always_inline))
	//		void writedata16_cont(uint16_t d) {
	//		setDCForData();
	//#if SPI_MODE_NORMAL | SPI_MODE_DMA
	//		enableCS();
	//#endif
	//		write16_cont(d);
	//	}
	//
	//	// Enables CS, sets DC to Data, writes 2 bytes, disables CS
	//	__attribute__((always_inline))
	//		void writedata16_last(uint16_t d) {
	//		setDCForData();
	//#if SPI_MODE_NORMAL | SPI_MODE_DMA
	//		enableCS();
	//#endif
	//		write16_last(d);
	//	}

	//#ifdef ARDUINO_SAM_DUE
	// Enables CS, sets DC and writes n-bytes from the buffer via DMA
	// Does not disable CS
	/*inline __attribute__((always_inline))
	void writedata_cont(const uint8_t* buf , uint32_t n) {
	enableCS();
	setDCForData();
	dmaSend(buf, n);
	}*/

	// Enables CS, sets DC, writes n-bytes from the buffer via DMA, disables CS
	/*inline __attribute__((always_inline))
	void writedata_last(const uint8_t* buf , uint32_t n) {
	setDCForData();
	enableCS();
	dmaSend(buf, n);
	disableCS();
	}*/

	// Writes n-bytes from the buffer via DMA
	// CS and DC have to be set prior to calling this method
	//	inline __attribute__((always_inline))
	//		void write_cont(const uint8_t* buf, uint32_t n) {
	//#if SPI_MODE_NORMAL
	//		spiTransfer(buf, n);
	//#elif SPI_MODE_EXTENDED
	//		spiTransfer(_cs, buf, n, SPI_CONTINUE);
	//#elif SPI_MODE_DMA
	//		dmaSend(buf, n);
	//#endif
	//	}

	inline __attribute__((always_inline))
		void write_cont(uint16_t* buf, uint32_t n) {
#if SPI_MODE_NORMAL
		spiTransfer(buf, n);
#elif SPI_MODE_EXTENDED
		spiTransfer(_cs, buf, n, SPI_CONTINUE);
#elif SPI_MODE_DMA
		dmaSend(buf, n);
#endif
	}

	inline __attribute__((always_inline))
		void write_cont(const uint16_t* buf, uint32_t n) {
#if SPI_MODE_NORMAL
		spiTransfer(buf, n);
#elif SPI_MODE_EXTENDED
		spiTransfer(_cs, buf, n, SPI_CONTINUE);
#elif SPI_MODE_DMA
		dmaSend(buf, n);
#endif
	}

	inline __attribute__((always_inline))
		void read_cont(uint8_t* buf, uint32_t n) {
#if SPI_MODE_DMA
		dmaReceive(buf, n);
#endif
	}

	// Writes n-bytes from the buffer via DMA and disables CS
	// DC has to be set prior to calling this method
	/*inline __attribute__((always_inline))
	void write_last(const uint8_t* buf , uint32_t n) {
	dmaSend(buf, n << 1);
	disableCS();
	}*/

	//	inline __attribute__((always_inline))
	//		void writeScanline(uint16_t d) {
	//		/*setDCForData();
	//		enableCS();*/
	//#if SPI_MODE_NORMAL
	//		Serial.println(_scanlineCounter);
	//		if (_scanlineCounter == SCANLINE_BUFFER_SIZE)
	//		{
	//			spiTransfer(_scanline, SCANLINE_BUFFER_SIZE);
	//			_scanlineCounter = 0;
	//		}
	//
	//		_scanline[_scanlineCounter++] = highByte(d);
	//		_scanline[_scanlineCounter++] = lowByte(d);
	//		
	//#elif SPI_MODE_EXTENDED
	//		SPI.transfer(_cs, _scanline, n << 1, SPI_CONTINUE);
	//#elif SPI_MODE_DMA
	//		dmaSend(_scanline, n << 1);	// each pixel is 2 bytes
	//#endif
	//
	//		//dmaSend(_scanline, n); // DMA16
	//	}

	// Writes n-bytes from the scanline buffer via DMA
	// Does not disable CS
//	inline __attribute__((always_inline))
//		void writeScanline(uint32_t n) {
//		/*setDCForData();
//		enableCS();*/
//#if SPI_MODE_NORMAL
//		spiTransfer(_scanline, n);
//#elif SPI_MODE_EXTENDED
//		SPI.transfer(_cs, _scanline, n << 1, SPI_CONTINUE);
//#elif SPI_MODE_DMA
//		dmaSend(_scanline, n << 1);	// each pixel is 2 bytes
//#endif
//
//		//dmaSend(_scanline, n); // DMA16
//	}

	inline __attribute__((always_inline))
		void writeScanline16(uint32_t n) {
		/*setDCForData();
		enableCS();*/
#if SPI_MODE_NORMAL
		spiTransfer(_scanline16, n);
#elif SPI_MODE_EXTENDED
		spiTransfer(_cs, _scanline16, n, SPI_CONTINUE);
#elif SPI_MODE_DMA
		dmaSend(_scanline16, n);	// each pixel is 2 bytes
#endif

		//dmaSend(_scanline, n); // DMA16
	}

	inline __attribute__((always_inline))
		void writeScanlineLooped(uint32_t n) {

		if (n > SCANLINE_PIXEL_COUNT) {
			const uint32_t numLoops = n / (uint32_t)SCANLINE_PIXEL_COUNT;
			for (uint32_t l = 0; l < numLoops; l++)
			{
				writeScanline16(SCANLINE_PIXEL_COUNT);
			}	
		}

		uint16_t remainingPixels = n == SCANLINE_PIXEL_COUNT ? SCANLINE_PIXEL_COUNT : n % SCANLINE_PIXEL_COUNT;
		if (remainingPixels > 0) {
			writeScanline16(remainingPixels);
		}
	}

	// writes n-bytes from the scanline buffer via DMA
	// Does not enable CS nor sets DS nor disables CS
	//inline __attribute__((always_inline))
	//	void writeScanline_cont_noCS_noDC(uint32_t n) {
	//		dmaSend(_scanline, n << 1);	// each pixel is 2 bytes
	//}

	// Writes n-bytes from the scanline buffer via DMA and disabled CS
	//	inline __attribute__((always_inline))
	//		void writeScanline_last(uint32_t n) {
	//#if SPI_MODE_EXTENDED
	//		SPI.transfer(_cs, _scanline, n << 1, SPI_LAST);
	//#elif SPI_MODE_DMA
	//		dmaSend(_scanline, n << 1);	// each pixel is 2 bytes
	//		//dmaSend(_scanline, n);	// DMA16
	//		disableCS();
	//#endif
	//	}

	//#endif

	// Reads 1 byte
	__attribute__((always_inline))
		uint8_t read8_cont()  {
#if SPI_MODE_NORMAL
		return SPI.transfer(ILI9341_NOP);
#elif SPI_MODE_EXTENDED
		return SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
#elif SPI_MODE_DMA
		return dmaReceive();
#endif
	}

	__attribute__((always_inline))
		uint8_t read8_last() {
#if SPI_MODE_NORMAL
		uint8_t r = SPI.transfer(ILI9341_NOP);
		disableCS();
		return r;
#elif SPI_MODE_EXTENDED
		return SPI.transfer(_cs, ILI9341_NOP, SPI_LAST);
#elif SPI_MODE_DMA
		uint8_t r = dmaReceive();
		disableCS();
		return r;
#endif
	}

	// Reads 2 bytes
	__attribute__((always_inline))
		uint16_t read16()  {
#if SPI_MODE_NORMAL
		uint16_t r = SPI.transfer(ILI9341_NOP);
		r <<= 8;
		r |= SPI.transfer(ILI9341_NOP);
#elif SPI_MODE_EXTENDED
		uint16_t r = SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
		r <<= 8;
		r |= SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
#elif SPI_MODE_DMA
		uint16_t r = dmaReceive();
		r <<= 8;
		r |= dmaReceive();
#endif
		return r;
	}

	// Reads 2 bytes
	__attribute__((always_inline))
		uint16_t readPixel_start_cont() {
#if SPI_MODE_NORMAL
		uint16_t r = SPI.transfer(ILI9341_NOP);
		r <<= 8;
		r |= SPI.transfer(ILI9341_NOP);
#elif SPI_MODE_EXTENDED
		uint16_t r = SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
		r <<= 8;
		r |= SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
#elif SPI_MODE_DMA
		uint16_t r = dmaReceive();
		r <<= 8;
		r |= dmaReceive();
#endif
		return r;
	}

	// Enables CS, sets DC for Data and reads 1 byte
	// Does not disable CS
	__attribute__((always_inline))
		uint8_t readdata8_cont() {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		return read8_cont();
	}

	// Enables CS, sets DC for Data, reads 1 byte and disables CS
	__attribute__((always_inline))
		uint8_t readdata8_last() {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		return read8_last();
	}

	// Enables CS, sets DC for Data and reads 2 bytes
	// Does not disable CS
	__attribute__((always_inline))
		uint16_t readdata16_cont() {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		return read16();
	}

	// Enables CS, sets DC for Data, reads 2 bytes and disables CS
	__attribute__((always_inline))
		uint8_t readdata16_last() {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		uint16_t r = read16();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		disableCS();
#endif
		//TOTRY
		/*uint8_t buff[2];	// not tested yet
		enableCS();
		dmaReceive(buff, 2);
		disableCS();
		uint16_t r = makeWord(buff[1], buff[0]);*/

		return r;
	}

	//#if SPI_MODE_DMA | SPI_MODE_EXTENDED
	//void fillScanline(uint8_t color) __attribute__((always_inline)) {
	//	//for(uint16_t i=0; i<sizeof(_scanline); i++)
	//	//{
	//	//	_scanline[i] = color;
	//	//}
	//	_scanline[0]=color;
	//	memmove(_scanline+sizeof(_scanline[0]), _scanline, sizeof(_scanline)-sizeof(_scanline[0]));
	//	//arr[0]=color;
	//	//memcpy( ((uint8_t*)arr)+sizeof(arr[0]), arr, sizeof(arr)-sizeof(arr[0]) ); // this hack does not seem to work on Due
	//}

	//void fillScanline(uint8_t color, uint32_t seedCopyCount) __attribute__((always_inline)) {
	//	//for(uint16_t i=0; i<seedCopyCount; i++)
	//	//{
	//	//	_scanline[i] = color;
	//	//}
	//	_scanline[0]=color;
	//	memmove(_scanline+sizeof(_scanline[0]), _scanline, seedCopyCount-sizeof(_scanline[0]));
	//	//arr[0]=color;
	//	//memcpy( ((uint8_t*)arr)+sizeof(arr[0]), arr, seedCopyCount-sizeof(arr[0]) );
	//}

	__attribute__((always_inline))
		void fillScanline16(uint16_t color) {

		for (uint16_t i = 0; i < SCANLINE_PIXEL_COUNT; i++)
		{
			_scanline16[i] = color;
		}
	}

	__attribute__((always_inline))
		void fillScanline16(uint16_t color, uint16_t len) {

		for (uint16_t i = 0; i < len; i++)
		{
			_scanline16[i] = color;
		}
//	}
//	// Sets all pixels in the scanline buffer to the specified color
//	__attribute__((always_inline))
//		void fillScanline(uint16_t color) {
//#if SPI_MODE_DMA | SPI_MODE_EXTENDED
//		_hiByte = highByte(color);
//		_loByte = lowByte(color);
//		for (uint16_t i = 0; i < SCANLINE_BUFFER_SIZE; i += 2)
//		{
//			_scanline[i] = _hiByte;
//			_scanline[i + 1] = _loByte;
//		}
//#elif SPI_MODE_NORMAL
//		for (uint16_t i = 0; i < SCANLINE_PIXEL_COUNT; i++)
//		{
//			_scanline[i] = color;
//		}
//#endif

		// DMA16
		//for (uint16_t i = 0; i < sizeof(_scanline); i++)
		//{
		//	_scanline[i] = color;
		//}

		/*_scanline[0]=highByte(color);
		_scanline[1]=lowByte(color);
		_scanline[2]=_scanline[0];
		_scanline[3]=_scanline[1];

		memcpy(_scanline+4, _scanline, 4);
		memcpy(_scanline+8, _scanline, 8);
		memcpy(_scanline+16, _scanline, 16);
		memcpy(_scanline+32, _scanline, 32);
		memcpy(_scanline+64, _scanline, 64);
		memcpy(_scanline+128, _scanline, 128);
		memcpy(_scanline+256, _scanline, 256);
		memcpy(_scanline+512, _scanline, 128);*/

		/*for(uint16_t i=0; i<sizeof(_scanline); i+=2)
		{
		memcpy(_scanline+i, _scanline, 2);
		}*/

		//arr[0]=highByte(color);
		//arr[1]=lowByte(color);
		//memcpy( ((uint8_t*)arr)+2*sizeof(arr[0]), arr, sizeof(arr)-2*sizeof(arr[0]) );
	}

	// Sets first n pixels in scanline buffer to the specified color
	//__attribute__((always_inline))
	//	void fillScanline(uint16_t color, uint32_t n){
	//	_hiByte = highByte(color);
	//	_loByte = lowByte(color);
	//	for (uint16_t i = 0; i < (n << 1); i += 2)
	//	{
	//		_scanline[i] = _hiByte;
	//		_scanline[i + 1] = _loByte;
	//	}

	//	// DMA16
	//	//for (uint16_t i = 0; i < n; i++)
	//	//{
	//	//	_scanline[i] = color;
	//	//}

	//	/*_scanline[0]=highByte(color);
	//	_scanline[1]=lowByte(color);
	//	memmove(((uint8_t*)_scanline)+2*sizeof(_scanline[0]), _scanline, (n << 1) -2*sizeof(_scanline[0]));
	//	*/
	//}

	//inline __attribute__((always_inline))
	//	void *memcpy_forward(uint8_t *dst, const uint8_t *src, uint32_t n)
	//{
	//	uint8_t *pdst;
	//	const uint8_t *psrc;

	//	for (pdst = dst, psrc = src; n > 0; --n, ++pdst, ++psrc)
	//		*pdst = *psrc;

	//	return dst;
	//}

	//#endif

	// Writes a sequence that will render a horizontal line
	// CS must be set prior to calling this method
	// for DMA mode, scanline buffer must be filled with the desired color
	// Advantage over writeHLine_cont is that CS line is not being set and 
	// the scanlineBuffer not filled on every call
	__attribute__((always_inline))
		void writeHLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t w)
	{
#ifdef ARDUINO_ARCH_AVR
		const uint32_t numLoops = (uint32_t)w / (uint32_t)SCANLINE_PIXEL_COUNT;
		setAddrAndRW_cont(x, y, w, 1);
		setDCForData();
		for (uint32_t l = 0; l < numLoops; l++)
		{
			writeScanline16(SCANLINE_PIXEL_COUNT);
		}
		uint16_t remainingPixels = w % SCANLINE_PIXEL_COUNT;
		if (remainingPixels > 0)
			writeScanline16(remainingPixels);
#elif defined ARDUINO_SAM_DUE
		setAddrAndRW_cont(x, y, w, 1);
		setDCForData();
		writeScanline16(w);
#endif
	}

	__attribute__((always_inline))
		void writeHLine_cont_noCS_noScanline(int16_t x, int16_t y, int16_t w, uint16_t color)
	{
		setAddrAndRW_cont(x, y, w, 1);
		setDCForData();
		while (w-- > 0) {
			write16_cont(color);
		}
	}

	// Writes a sequence that will render a vertical line
	// CS must be set prior to calling this method
	// for DMA mode, scanline buffer must be filled with the desired color
	// Advantage over writeVLine_cont is that CS line is not being set and 
	// the scanlineBuffer not filled on every call
	__attribute__((always_inline))
		void writeVLine_cont_noCS_noFill(int16_t x, int16_t y, int16_t h)
	{
#ifdef ARDUINO_ARCH_AVR
		const uint32_t numLoops = (uint32_t)h / (uint32_t)SCANLINE_PIXEL_COUNT;
		setAddrAndRW_cont(x, y, 1, h);
		setDCForData();
		for (uint32_t l = 0; l < numLoops; l++)
		{
			writeScanline16(SCANLINE_PIXEL_COUNT);
		}
		uint16_t remainingPixels = h % SCANLINE_PIXEL_COUNT;
		if (remainingPixels > 0)
			writeScanline16(remainingPixels);
#elif defined ARDUINO_SAM_DUE
		setAddrAndRW_cont(x, y, 1, h);
		setDCForData();
		writeScanline16(h);
#endif
	}

	__attribute__((always_inline))
		void writeVLine_cont_noCS_noScanline(int16_t x, int16_t y, int16_t h, uint16_t color)
	{
		setAddrAndRW_cont(x, y, 1, h);
		setDCForData();
		while (h-- > 0) {
			write16_cont(color);
		}
	}

	inline __attribute__((always_inline))
		void writePixel_cont(int16_t x, int16_t y, uint16_t color)
	{
		setAddrAndRW_cont_inline(x, y, 1, 1);
		setDCForData();
		write16_cont(color);
	}

	// Enables CS, writes a sequence that will render a pixel and disables CS
	inline __attribute__((always_inline))
		void writePixel_last(int16_t x, int16_t y, uint16_t color)
	{
		setAddrAndRW_cont(x, y, 1, 1);
		setDCForData();
		write16_last(color);
	}

#ifdef ARDUINO_SAM_DUE
	inline __attribute__((always_inline))
#endif
		void drawPixel_cont(int16_t x, int16_t y, uint16_t color) {

		if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
		writePixel_cont(x, y, color);
	}

	inline __attribute__((always_inline))
		void drawPixel_last(int16_t x, int16_t y, uint16_t color) {

		if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
		writePixel_last(x, y, color);
	}

	// Enables CS
	inline __attribute__((always_inline))
		void enableCS(){
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		*_csport &= ~_cspinmask;
#endif
	}

	// Disables CS
	inline __attribute__((always_inline))
		void disableCS() {
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		*_csport |= _cspinmask;
		//csport->PIO_SODR  |=  cspinmask;
#elif SPI_MODE_EXTENDED
		writecommand_last(ILI9341_NOP);	// have to send a byte to disable CS
#endif
	}

	// Sets DC to Data (1)
	inline __attribute__((always_inline))
		void setDCForData() {
		*_dcport |= _dcpinmask;
		//_dcport->PIO_SODR |= _dcpinmask;
	}

	// Sets DC to Command (0)	
	inline __attribute__((always_inline))
		void setDCForCommand(){
		*_dcport &= ~_dcpinmask;
	}
#ifdef ARDUINO_ARCH_AVR
	inline __attribute__((always_inline))
		void spiTransfer(const uint8_t *buf, uint32_t count) {
		SPDR = *buf;
		while (--count > 0) {
			uint8_t out = *(buf + 1);
			while (!(SPSR & _BV(SPIF)));
			//uint8_t in = SPDR;
			SPDR = out;
			buf++;
		}
		while (!(SPSR & _BV(SPIF)));
		//*buf = SPDR;
	}

	inline __attribute__((always_inline))
		void spiTransfer(uint16_t *buf, uint32_t count) {
		uint16_t pixel = *buf;
		SPDR = highByte(pixel);
		while (!(SPSR & _BV(SPIF)));
		SPDR = lowByte(pixel);
		while (--count > 0) {
			uint16_t out = *(buf + 1);
			while (!(SPSR & _BV(SPIF)));
			//uint8_t in = SPDR;
			SPDR = highByte(out);
			while (!(SPSR & _BV(SPIF)));
			SPDR = lowByte(out);
			buf++;
		}
		while (!(SPSR & _BV(SPIF)));
		//*buf = SPDR;
	}

	void spiTransfer(const uint16_t *buf, uint32_t count) {
		uint16_t pixel = pgm_read_word(buf);
		SPDR = highByte(pixel);
		while (!(SPSR & _BV(SPIF)));
		SPDR = lowByte(pixel);
		while (--count > 0) {
			uint16_t out = pgm_read_word(buf + 1);
			while (!(SPSR & _BV(SPIF)));
			//uint8_t in = SPDR;
			SPDR = highByte(out);
			while (!(SPSR & _BV(SPIF)));
			SPDR = lowByte(out);
			buf++;
		}
		while (!(SPSR & _BV(SPIF)));
		//*buf = SPDR;
	}
#elif defined ARDUINO_SAM_DUE
#if SPI_MODE_NORMAL
	void spiTransfer(uint8_t _data) {
		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS);
		uint32_t d = _data | SPI_PCS(ch);
		d |= SPI_TDR_LASTXFER;

		// SPI_Write(spi, _channel, _data);
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d;

		// return SPI_Read(spi);
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
	}

	void spiTransfer(uint16_t _data) {
		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS);

		SPI0->SPI_CSR[ch] = (SPI0->SPI_CSR[ch] &= 0xFFFFFF0F) | 0x00000080;	//set 16 bit
		uint32_t d = _data | SPI_PCS(ch);

		d |= SPI_TDR_LASTXFER;

		// SPI_Write(spi, _channel, _data);
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d;


		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
		SPI0->SPI_CSR[ch] &= 0xFFFFFF0F; //restore 8bit
	}

	void spiTransfer(const uint8_t *_buf, uint32_t _count) {
		if (_count == 0)
			return;

		if (_count == 1) {
			spiTransfer(*_buf);
			return;
		}

		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS);

		// Send the first byte
		uint32_t d = *_buf;

		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d | SPI_PCS(ch);

		while (_count > 1) {
			// Prepare next byte
			d = *(_buf + 1);

			if (_count == 2)
				d |= SPI_TDR_LASTXFER;

			// Read transferred byte and send next one straight away
			while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
				;
			SPI0->SPI_RDR;
			SPI0->SPI_TDR = d | SPI_PCS(ch);

			// Save read byte

			//*_buf = r;
			_buf++;
			_count--;
		}


		// Receive the last transferred byte
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
		//*_buf = r;
	}

	void spiTransfer(const uint16_t *_buf, uint32_t _count) {
		if (_count == 0)
			return;

		if (_count == 1) {
			spiTransfer(*_buf);
			return;
		}

		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(BOARD_SPI_DEFAULT_SS);

		SPI0->SPI_CSR[ch] = (SPI0->SPI_CSR[ch] &= 0xFFFFFF0F) | 0x00000080;	//set 16 bit
		// Send the first byte
		uint32_t d = *_buf;

		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d | SPI_PCS(ch);

		while (_count > 1) {
			// Prepare next byte
			d = *(_buf + 1);

			if (_count == 2)
				d |= SPI_TDR_LASTXFER;

			// Read transferred byte and send next one straight away
			while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
				;
			SPI0->SPI_RDR;
			SPI0->SPI_TDR = d | SPI_PCS(ch);

			// Save read byte

			//*_buf = r;
			_buf++;
			_count--;
		}


		// Receive the last transferred byte
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
		//*_buf = r;
		SPI0->SPI_CSR[ch] &= 0xFFFFFF0F; //restore 8bit
	}
#elif SPI_MODE_EXTENDED


	void spiTransfer(byte _pin, const uint8_t *_buf, uint32_t _count, SPITransferMode _mode) {
		if (_count == 0)
			return;

		if (_count == 1) {
			SPI.transfer(_pin, *_buf, _mode);
			return;
		}

		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

		// Send the first byte
		uint32_t d = *_buf;

		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d | SPI_PCS(ch);

		while (_count > 1) {
			// Prepare next byte
			d = *(_buf + 1);

			if (_count == 2 && _mode == SPI_LAST)
				d |= SPI_TDR_LASTXFER;

			// Read transferred byte and send next one straight away
			while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
				;
			SPI0->SPI_RDR;
			SPI0->SPI_TDR = d | SPI_PCS(ch);

			// Save read byte

			//*_buf = r;
			_buf++;
			_count--;
		}

		// Receive the last transferred byte
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;

		//*_buf = r;
	}

	void spiTransfer(byte _pin, uint16_t _data, SPITransferMode _mode) {
		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

		SPI0->SPI_CSR[ch] = (SPI0->SPI_CSR[ch] &= 0xFFFFFF0F) | 0x00000080;	//set 16 bit
		uint32_t d = _data | SPI_PCS(ch);
		if (_mode == SPI_LAST)
			d |= SPI_TDR_LASTXFER;

		// SPI_Write(spi, _channel, _data);
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d;


		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
		SPI0->SPI_CSR[ch] &= 0xFFFFFF0F; //restore 8bit
	}

	void spiTransfer(byte _pin, const uint16_t *_buf, uint32_t _count, SPITransferMode _mode) {
		if (_count == 0)
			return;

		if (_count == 1) {
			spiTransfer(_pin, *_buf, _mode);
			return;
		}

		uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

		SPI0->SPI_CSR[ch] = (SPI0->SPI_CSR[ch] &= 0xFFFFFF0F) | 0x00000080;	//set 16 bit
		// Send the first byte
		uint32_t d = *_buf;

		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0)
			;
		SPI0->SPI_TDR = d | SPI_PCS(ch);

		while (_count > 1) {
			// Prepare next byte
			d = *(_buf + 1);

			if (_count == 2 && _mode == SPI_LAST)
				d |= SPI_TDR_LASTXFER;

			// Read transferred byte and send next one straight away
			while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
				;
			SPI0->SPI_RDR;
			SPI0->SPI_TDR = d | SPI_PCS(ch);

			// Save read byte

			//*_buf = r;
			_buf++;
			_count--;
		}


		// Receive the last transferred byte
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0)
			;
		SPI0->SPI_RDR;
		//*_buf = r;
		SPI0->SPI_CSR[ch] &= 0xFFFFFF0F; //restore 8bit
	}
#elif SPI_MODE_DMA
	/** Use SAM3X DMAC if nonzero */
#define ILI_USE_SAM3X_DMAC 1
	/** Use extra Bus Matrix arbitration fix if nonzero */
#define ILI_USE_SAM3X_BUS_MATRIX_FIX 0
	/** Time in ms for DMA receive timeout */
#define ILI_SAM3X_DMA_TIMEOUT 100
	/** chip select register number */
#define ILI_SPI_CHIP_SEL 3
	/** DMAC receive channel */
#define ILI_SPI_DMAC_RX_CH  1
	/** DMAC transmit channel */
#define ILI_SPI_DMAC_TX_CH  0
	/** DMAC Channel HW Interface Number for SPI TX. */
#define ILI_SPI_TX_IDX  1
	/** DMAC Channel HW Interface Number for SPI RX. */
#define ILI_SPI_RX_IDX  2
	//------------------------------------------------------------------------------
	/** Disable DMA Controller. */
	static void dmac_disable() {
		DMAC->DMAC_EN &= (~DMAC_EN_ENABLE);
	}
	/** Enable DMA Controller. */
	static void dmac_enable() {
		DMAC->DMAC_EN = DMAC_EN_ENABLE;
	}
	/** Disable DMA Channel. */
	static void dmac_channel_disable(uint32_t ul_num) {
		DMAC->DMAC_CHDR = DMAC_CHDR_DIS0 << ul_num;
	}
	/** Enable DMA Channel. */
	static void dmac_channel_enable(uint32_t ul_num) {
		DMAC->DMAC_CHER = DMAC_CHER_ENA0 << ul_num;
	}
	/** Poll for transfer complete. */
	static bool dmac_channel_transfer_done(uint32_t ul_num) {
		return (DMAC->DMAC_CHSR & (DMAC_CHSR_ENA0 << ul_num)) ? false : true;
	}

	static void spi_set_16bit_transfer() {
		SPI0->SPI_CSR[ILI_SPI_CHIP_SEL] = (SPI0->SPI_CSR[ILI_SPI_CHIP_SEL] &= 0xFFFFFF0F) | 0x00000080;
	}

	static void spi_set_8bit_transfer() {
		SPI0->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI0->SPI_CSR[ILI_SPI_CHIP_SEL] &= 0xFFFFFF0F;
	}
	//------------------------------------------------------------------------------
	void dmaBegin() {
		PIO_Configure(
			g_APinDescription[PIN_SPI_MOSI].pPort,
			g_APinDescription[PIN_SPI_MOSI].ulPinType,
			g_APinDescription[PIN_SPI_MOSI].ulPin,
			g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
		PIO_Configure(
			g_APinDescription[PIN_SPI_MISO].pPort,
			g_APinDescription[PIN_SPI_MISO].ulPinType,
			g_APinDescription[PIN_SPI_MISO].ulPin,
			g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
		PIO_Configure(
			g_APinDescription[PIN_SPI_SCK].pPort,
			g_APinDescription[PIN_SPI_SCK].ulPinType,
			g_APinDescription[PIN_SPI_SCK].ulPin,
			g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
		pmc_enable_periph_clk(ID_SPI0);
#if ILI_USE_SAM3X_DMAC
		pmc_enable_periph_clk(ID_DMAC);
		dmac_disable();
		DMAC->DMAC_GCFG = DMAC_GCFG_ARB_CFG_FIXED;
		dmac_enable();
#if ILI_USE_SAM3X_BUS_MATRIX_FIX
		MATRIX->MATRIX_WPMR = 0x4d415400;
		MATRIX->MATRIX_MCFG[1] = 1;
		MATRIX->MATRIX_MCFG[2] = 1;
		MATRIX->MATRIX_SCFG[0] = 0x01000010;
		MATRIX->MATRIX_SCFG[1] = 0x01000010;
		MATRIX->MATRIX_SCFG[7] = 0x01000010;
#endif  // ILI_USE_SAM3X_BUS_MATRIX_FIX
#endif  // ILI_USE_SAM3X_DMAC
	}
	//------------------------------------------------------------------------------
	//  initialize SPI controller
	void dmaInit(uint8_t sckDivisor) {
		uint8_t scbr = sckDivisor;
		Spi* pSpi = SPI0;
		//  disable SPI
		pSpi->SPI_CR = SPI_CR_SPIDIS;
		// reset SPI
		pSpi->SPI_CR = SPI_CR_SWRST;
		// no mode fault detection, set master mode
		pSpi->SPI_MR = SPI_PCS(ILI_SPI_CHIP_SEL) | SPI_MR_MODFDIS | SPI_MR_MSTR;
		// mode 0, 8-bit,
		pSpi->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI_CSR_SCBR(scbr) | SPI_CSR_NCPHA | SPI_CSR_BITS_8_BIT;
		// enable SPI
		pSpi->SPI_CR |= SPI_CR_SPIEN;
	}

	void dmaInit16(uint8_t sckDivisor) {
		uint8_t scbr = sckDivisor;
		Spi* pSpi = SPI0;
		//  disable SPI
		pSpi->SPI_CR = SPI_CR_SPIDIS;
		// reset SPI
		pSpi->SPI_CR = SPI_CR_SWRST;
		// no mode fault detection, set master mode
		pSpi->SPI_MR = SPI_PCS(ILI_SPI_CHIP_SEL) | SPI_MR_MODFDIS | SPI_MR_MSTR;
		// mode 0, 16-bit,
		pSpi->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI_CSR_SCBR(scbr) | SPI_CSR_NCPHA | SPI_CSR_BITS_16_BIT;
		// enable SPI
		pSpi->SPI_CR |= SPI_CR_SPIEN;
	}
	//------------------------------------------------------------------------------
	// start RX DMA
	void spiDmaRX(uint8_t* dst, uint16_t count) {
		dmac_channel_disable(ILI_SPI_DMAC_RX_CH);
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_SADDR = (uint32_t)&SPI0->SPI_RDR;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_DADDR = (uint32_t)dst;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_DSCR = 0;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CTRLA = count |
			DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CTRLB = DMAC_CTRLB_SRC_DSCR |
			DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_PER2MEM_DMA_FC |
			DMAC_CTRLB_SRC_INCR_FIXED | DMAC_CTRLB_DST_INCR_INCREMENTING;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_RX_CH].DMAC_CFG = DMAC_CFG_SRC_PER(ILI_SPI_RX_IDX) |
			DMAC_CFG_SRC_H2SEL | DMAC_CFG_SOD | DMAC_CFG_FIFOCFG_ASAP_CFG;
		dmac_channel_enable(ILI_SPI_DMAC_RX_CH);
	}
	//------------------------------------------------------------------------------
	// start TX DMA
	void spiDmaTX(const uint8_t* src, uint16_t count) {
		static uint8_t ff = 0XFF;
		uint32_t src_incr = DMAC_CTRLB_SRC_INCR_INCREMENTING;
		if (!src) {
			src = &ff;
			src_incr = DMAC_CTRLB_SRC_INCR_FIXED;
		}
		dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_SADDR = (uint32_t)src;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DADDR = (uint32_t)&SPI0->SPI_TDR;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DSCR = 0;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLA = count |
			DMAC_CTRLA_SRC_WIDTH_BYTE | DMAC_CTRLA_DST_WIDTH_BYTE;

		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLB = DMAC_CTRLB_SRC_DSCR |
			DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_MEM2PER_DMA_FC |
			src_incr | DMAC_CTRLB_DST_INCR_FIXED;

		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CFG = DMAC_CFG_DST_PER(ILI_SPI_TX_IDX) |
			DMAC_CFG_DST_H2SEL | DMAC_CFG_SOD | DMAC_CFG_FIFOCFG_ALAP_CFG;

		dmac_channel_enable(ILI_SPI_DMAC_TX_CH);
	}

	void spiDmaTX16(const uint16_t* src, uint16_t count) {
		static uint16_t ff = 0XFFFF;
		uint32_t src_incr = DMAC_CTRLB_SRC_INCR_INCREMENTING;
		if (!src) {
			src = &ff;
			src_incr = DMAC_CTRLB_SRC_INCR_FIXED;
		}
		dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_SADDR = (uint32_t)src;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DADDR = (uint32_t)&SPI0->SPI_TDR;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_DSCR = 0;
		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLA = count |
			DMAC_CTRLA_SRC_WIDTH_HALF_WORD | DMAC_CTRLA_DST_WIDTH_HALF_WORD;

		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CTRLB = DMAC_CTRLB_SRC_DSCR |
			DMAC_CTRLB_DST_DSCR | DMAC_CTRLB_FC_MEM2PER_DMA_FC |
			src_incr | DMAC_CTRLB_DST_INCR_FIXED;

		DMAC->DMAC_CH_NUM[ILI_SPI_DMAC_TX_CH].DMAC_CFG = DMAC_CFG_DST_PER(ILI_SPI_TX_IDX) |
			DMAC_CFG_DST_H2SEL | DMAC_CFG_SOD | DMAC_CFG_FIFOCFG_ALAP_CFG;

		dmac_channel_enable(ILI_SPI_DMAC_TX_CH);
	}
	//------------------------------------------------------------------------------
	__attribute__((always_inline))
		uint8_t dmaSpiTransfer(uint8_t b) {
		Spi* pSpi = SPI0;

		pSpi->SPI_TDR = b;
		while ((pSpi->SPI_SR & SPI_SR_RDRF) == 0) {}
		b = pSpi->SPI_RDR;
		return b;
	}

	__attribute__((always_inline))
		uint16_t dmaSpiTransfer(uint16_t w) {
		spi_set_16bit_transfer();
		SPI0->SPI_TDR = w;
		while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0) {}
		w = SPI0->SPI_RDR;
		spi_set_8bit_transfer();
		return w;
	}
	//------------------------------------------------------------------------------
	/** SPI receive a byte */
	__attribute__((always_inline))
		uint8_t dmaReceive() {
		return dmaSpiTransfer((uint8_t)0XFF);
	}
	//------------------------------------------------------------------------------
	/** SPI receive multiple bytes */
	uint8_t dmaReceive(uint8_t* buf, uint32_t n) {
		Spi* pSpi = SPI0;
		int rtn = 0;
#if ILI_USE_SAM3X_DMAC
		// clear overrun error
		pSpi->SPI_SR;

		spiDmaRX(buf, n);
		spiDmaTX(0, n);

		uint32_t m = millis();
		while (!dmac_channel_transfer_done(ILI_SPI_DMAC_RX_CH)) {
			if ((millis() - m) > ILI_SAM3X_DMA_TIMEOUT)  {
				dmac_channel_disable(ILI_SPI_DMAC_RX_CH);
				dmac_channel_disable(ILI_SPI_DMAC_TX_CH);
				rtn = 2;
				break;
			}
		}
		if (pSpi->SPI_SR & SPI_SR_OVRES) rtn |= 1;
#else  // ILI_USE_SAM3X_DMAC
		for (uint32_t i = 0; i < n; i++) {
			pSpi->SPI_TDR = 0XFF;
			while ((pSpi->SPI_SR & SPI_SR_RDRF) == 0) {}
			buf[i] = pSpi->SPI_RDR;
		}
#endif  // ILI_USE_SAM3X_DMAC
		return rtn;
	}
	//------------------------------------------------------------------------------
	/** SPI send a byte */
	__attribute__((always_inline))
		void dmaSend(uint8_t b) {
		dmaSpiTransfer(b);
	}

	__attribute__((always_inline))
		void dmaSend(uint16_t w) {
		dmaSpiTransfer(w);
	}
	//------------------------------------------------------------------------------
	void dmaSend(const uint8_t* buf, uint32_t n) {
		Spi* pSpi = SPI0;
		spiDmaTX(buf, n);
		while (!dmac_channel_transfer_done(ILI_SPI_DMAC_TX_CH)) {}
		while ((pSpi->SPI_SR & SPI_SR_TXEMPTY) == 0) {}
		// leave RDR empty
		pSpi->SPI_RDR;
	}

	void dmaSend(const uint16_t* buf, uint32_t n) {
		Spi* pSpi = SPI0;
		pSpi->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI_CSR_SCBR(_spiClkDivider) | SPI_CSR_NCPHA | SPI_CSR_BITS_16_BIT;
		spiDmaTX16(buf, n);
		while (!dmac_channel_transfer_done(ILI_SPI_DMAC_TX_CH)) {}
		while ((pSpi->SPI_SR & SPI_SR_TXEMPTY) == 0) {}
		// leave RDR empty
		pSpi->SPI_RDR;
		pSpi->SPI_CSR[ILI_SPI_CHIP_SEL] = SPI_CSR_SCBR(_spiClkDivider) | SPI_CSR_NCPHA | SPI_CSR_BITS_8_BIT;
	}
#endif
#endif
};



#endif
