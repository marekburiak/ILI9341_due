/*
v.0.91

ILI9341_due_.h - Arduino Due library for interfacing with ILI9341-based TFTs

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
MIT license, all text above must be included in any redistribution
****************************************************/

#ifndef _ILI9341_dueH_
#define _ILI9341_dueH_



// comment out the SPI mode you want to use
//#define ILI9341_SPI_MODE_NORMAL
//#define ILI9341_SPI_MODE_EXTENDED	// make sure you use pin 4, 10 or 52 for CS
#define ILI9341_SPI_MODE_DMA

#define ILI9341_SPI_CLKDIVIDER 2



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

#include "Arduino.h"
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
#include <SPI.h>
#elif SPI_MODE_DMA
#include "ILI_SdSpi.h"
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
#define ILI9341_RDIMGFMT  0x0A
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
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0
#define ILI9341_WHITE   0xFFFF

typedef uint8_t pwrLevel;

// Normal Mode On (full display), Idle Mode Off, Sleep Out. 
// In this mode, the display is able to show maximum 262,144 colors. 
#define PWRLEVEL_NORMAL 1

// Normal Mode On (full display), Idle Mode On, Sleep Out. 
// In this mode, the full display area is used but with 8 colors. 
#define PWRLEVEL_IDLE 2

//In this mode, the DC : DC converter, Internal oscillator and panel driver circuit are stopped. Only the MCU 
// interface and memory works with VDDI power supply. Contents of the memory are safe. 
#define PWRLEVEL_SLEEP 3

#define SCANLINE_BUFFER_SIZE 640 // 320 2-byte pixels

class ILI9341_due : public Print
{
public:
	ILI9341_due(uint8_t cs, uint8_t dc, uint8_t rst = 255);
#if SPI_MODE_DMA
	ILI_SdSpi _dmaSpi;
	uint8_t _scanlineBuffer[SCANLINE_BUFFER_SIZE];
	uint8_t _hiByte, _loByte;
	bool _isIdle, _isInSleep;
#endif
	bool begin(void);
	void pushColor(uint16_t color);
	void pushColors(uint16_t *colors, uint16_t offset, uint16_t len);
	void pushColors565(uint8_t *colors, uint16_t offset, uint16_t len);
	void fillScreen(uint16_t color);
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void drawPixel_cont(int16_t x, int16_t y, uint16_t color);
	void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void setRotation(uint8_t r);
	void invertDisplay(boolean i);
	void display(boolean d);
	void normalModeOn();
	void sleep(boolean s);
	void idle(boolean i);
	void setPowerLevel(pwrLevel p);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	}

	//uint8_t readdata(void);
	uint8_t readcommand8(uint8_t reg, uint8_t index = 0);

	// KJE Added functions to read pixel data...
	uint16_t readPixel(int16_t x, int16_t y);


	// from Adafruit_GFX.h
	void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
	void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
	void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
	void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
	void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
	void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void setCursor(int16_t x, int16_t y);
	void setTextColor(uint16_t c);
	void setTextColor(uint16_t c, uint16_t bg);
	void setTextSize(uint8_t s);
	void setTextWrap(boolean w);
	virtual size_t write(uint8_t);
	int16_t width(void)  { return _width; }
	int16_t height(void) { return _height; }
	uint8_t getRotation(void);
	void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

	// writes 1 byte
	// CS and DC have to be set prior to calling this method
	inline __attribute__((always_inline))
		void write8(uint8_t c){
#if SPI_MODE_NORMAL
			SPI.transfer(c);
#elif SPI_MODE_EXTENDED
			SPI.transfer(_cs, c, SPI_CONTINUE); 
#elif SPI_MODE_DMA
			_dmaSpi.send(c);
#endif
	}

	// writes 1 byte and disables CS
	// CS and DC have to be set prior to calling this method
	inline __attribute__((always_inline))
		void write8_last(uint8_t c) {
#if SPI_MODE_NORMAL
			SPI.transfer(c);
			disableCS();
#elif SPI_MODE_EXTENDED
			SPI.transfer(_cs, c, SPI_LAST); 
#elif SPI_MODE_DMA
			_dmaSpi.send(c);
			disableCS();
#endif
	}

	// Writes 2 bytes
	// CS, DC have to be set prior to calling this method
	inline __attribute__((always_inline)) 
		void write16(uint16_t d) {
#if SPI_MODE_NORMAL
			SPI.transfer(highByte(d));
			SPI.transfer(lowByte(d));
#elif SPI_MODE_EXTENDED
			SPI.transfer(_cs, highByte(d), SPI_CONTINUE); 
			SPI.transfer(_cs, lowByte(d), SPI_CONTINUE);
#elif SPI_MODE_DMA
			_dmaSpi.send(highByte(d));
			_dmaSpi.send(lowByte(d));
#endif
	}

	inline __attribute__((always_inline)) 
		void write16_last(uint16_t d) {
#if SPI_MODE_NORMAL
			SPI.transfer(highByte(d));
			SPI.transfer(lowByte(d));
			disableCS();
#elif SPI_MODE_EXTENDED
			SPI.transfer(_cs, highByte(d), SPI_CONTINUE); 
			SPI.transfer(_cs, lowByte(d), SPI_LAST);
#elif SPI_MODE_DMA
			_dmaSpi.send(highByte(d));
			_dmaSpi.send(lowByte(d));
			disableCS();
#endif
	}

	// Writes commands to set the GRAM area where data/pixels will be written
	void setAddr_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
		__attribute__((always_inline)) {
			writecommand_cont(ILI9341_CASET); // Column addr set
			writedata16_cont(x0);   // XSTART
			writedata16_cont(x1);   // XEND
			writecommand_cont(ILI9341_PASET); // Row addr set
			writedata16_cont(y0);   // YSTART
			writedata16_cont(y1);   // YEND
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
	//	write8(ILI9341_CASET); // Column addr set
	//	setDCForData();
	//	write16(x0);   // XSTART
	//	write16(x1);   // XEND
	//	setDCForCommand();
	//	write8(ILI9341_PASET); // Row addr set
	//	setDCForData();
	//	write16(y0);   // XSTART
	//	write16(y1);   // XEND
	//}

	// Enables CS, writes commands to set the GRAM area where data/pixels will be written
	// Also sends RAM WRITE command which should be followed by writing data/pixels 
	inline __attribute__((always_inline))
		void setAddrAndRW_cont(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	{
		enableCS();
		setDCForCommand();
		write8(ILI9341_CASET); // Column addr set
		setDCForData();
		write16(x0);   // XSTART
		write16(x1);   // XEND
		setDCForCommand();
		write8(ILI9341_PASET); // Row addr set
		setDCForData();
		write16(y0);   // XSTART
		write16(y1);   // XEND
		setDCForCommand();
		write8(ILI9341_RAMWR); // RAM write
	}

	// Writes commands to set the GRAM area where data/pixels will be written
	// Also sends RAM WRITE command which should be followed by writing data/pixels 
	// CS has to be enabled prior to calling this method
	//__attribute__((always_inline))
	//	void setAddrAndRW_cont_noCS(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
	//{
	//	setDCForCommand();
	//	write8(ILI9341_CASET); // Column addr set
	//	setDCForData();
	//	write16(x0);   // XSTART
	//	write16(x1);   // XEND
	//	setDCForCommand();
	//	write8(ILI9341_PASET); // Row addr set
	//	setDCForData();
	//	write16(y0);   // XSTART
	//	write16(y1);   // XEND
	//	setDCForCommand();
	//	write8(ILI9341_RAMWR); // Row addr set
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
	//		_dmaSpi.send(c);
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
			write8(c);
	}

	// Enables CS, sets DC for Command, writes 1 byte, disables CS
	inline __attribute__((always_inline))
		void writecommand_last(uint8_t c) {
			setDCForCommand();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			enableCS();
#endif
			write8(c);
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			disableCS();
#endif
	}

	// Enables CS, sets DC to Data, writes 1 byte
	// Does not disable CS
	inline __attribute__((always_inline))
		void writedata8_cont(uint8_t c) {
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			enableCS();
#endif
			write8(c);
	}

	// Enables CS, sets DC to Data, writes 1 byte, disables CS
	__attribute__((always_inline))
		void writedata8_last(uint8_t c) {
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			enableCS();
#endif
			write8(c);
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			disableCS();
#endif
	}

	// Enables CS, sets DC to Data, writes 2 bytes
	// Does not disable CS
	__attribute__((always_inline)) 
		void writedata16_cont(uint16_t d) {
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			enableCS();
#endif
			write16(d);
	}

	// Enables CS, sets DC to Data, writes 2 bytes, disables CS
	__attribute__((always_inline))
		void writedata16_last(uint16_t d) {
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
			enableCS();
#endif
			write16_last(d);
	}

#if SPI_MODE_DMA
	// Enables CS, sets DC and writes n-bytes from the buffer via DMA
	// Does not disable CS
	/*inline __attribute__((always_inline))
	void writedata_cont(const uint8_t* buf , size_t n) {
	enableCS();
	setDCForData();
	_dmaSpi.send(buf, n);
	}*/

	// Enables CS, sets DC, writes n-bytes from the buffer via DMA, disables CS
	/*inline __attribute__((always_inline))
	void writedata_last(const uint8_t* buf , size_t n) {
	setDCForData();
	enableCS();
	_dmaSpi.send(buf, n);
	disableCS();
	}*/

	// Writes n-bytes from the buffer via DMA
	// CS and DC have to be set prior to calling this method
	inline __attribute__((always_inline))
		void write_cont(const uint8_t* buf , size_t n) {
			_dmaSpi.send(buf, n);
	}

	// Writes n-bytes from the buffer via DMA and disables CS
	// DC has to be set prior to calling this method
	/*inline __attribute__((always_inline))
	void write_last(const uint8_t* buf , size_t n) {
	_dmaSpi.send(buf, n << 1);
	disableCS();
	}*/

	// Enables CS, sets DC and writes n-bytes from the scanline buffer via DMA
	// Does not disable CS
	inline __attribute__((always_inline))
		void writeScanline_cont(size_t n) {
			setDCForData();
			enableCS();
			_dmaSpi.send(_scanlineBuffer, n << 1);	// each pixel is 2 bytes
	}

	// writes n-bytes from the scanline buffer via DMA
	// Does not enable CS nor sets DS nor disables CS
	//inline __attribute__((always_inline))
	//	void writeScanline_cont_noCS_noDC(size_t n) {
	//		_dmaSpi.send(_scanlineBuffer, n << 1);	// each pixel is 2 bytes
	//}

	// Enables CS, sets DC, writes n-bytes from the scanline buffer via DMA and disabled CS
	inline __attribute__((always_inline))
		void writeScanline_last(size_t n) {
			setDCForData();
			enableCS();
			_dmaSpi.send(_scanlineBuffer, n << 1);	// each pixel is 2 bytes
			disableCS();
	}

#endif

	// Reads 1 byte
	uint8_t read8() __attribute__((always_inline)) {
#if SPI_MODE_NORMAL
		return SPI.transfer(ILI9341_NOP);
#elif SPI_MODE_EXTENDED
		return SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE); 
#elif SPI_MODE_DMA
		return _dmaSpi.receive();
#endif
	}

	// Reads 2 bytes
	uint16_t read16() __attribute__((always_inline)) {
#if SPI_MODE_NORMAL
		uint16_t r = SPI.transfer(ILI9341_NOP);
		r <<= 8;
		r |= SPI.transfer(ILI9341_NOP);
#elif SPI_MODE_EXTENDED
		uint16_t r = SPI.transfer(_cs, ILI9341_NOP, SPI_CONTINUE);
		r <<= 8;
		r |= SPI.transfer(_cs,ILI9341_NOP, SPI_CONTINUE);
#elif SPI_MODE_DMA
		uint16_t r = _dmaSpi.receive();
		r <<= 8;
		r |= _dmaSpi.receive();
#endif
		return r;
	}

	// Enables CS, sets DC for Data and reads 1 byte
	// Does not disable CS
	uint8_t readdata8_cont() __attribute__((always_inline)) {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		return read8();
	}

	// Enables CS, sets DC for Data, reads 1 byte and disables CS
	uint8_t readdata8_last() __attribute__((always_inline)) {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		uint8_t r = read8();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		disableCS();
#endif
		return r;
	}

	// Enables CS, sets DC for Data and reads 2 bytes
	// Does not disable CS
	uint16_t readdata16_cont() __attribute__((always_inline)) {
		setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_DMA
		enableCS();
#endif
		return read16();
	}

	// Enables CS, sets DC for Data, reads 2 bytes and disables CS
	uint8_t readdata16_last() __attribute__((always_inline)) {
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
		_dmaSpi.receive(buff, 2);
		disableCS();
		uint16_t r = makeWord(buff[1], buff[0]);*/

		return r;
	}

#if SPI_MODE_DMA
	//void fillScanline(uint8_t color) __attribute__((always_inline)) {
	//	//for(uint16_t i=0; i<sizeof(_scanlineBuffer); i++)
	//	//{
	//	//	_scanlineBuffer[i] = color;
	//	//}
	//	_scanlineBuffer[0]=color;
	//	memmove(_scanlineBuffer+sizeof(_scanlineBuffer[0]), _scanlineBuffer, sizeof(_scanlineBuffer)-sizeof(_scanlineBuffer[0]));
	//	//arr[0]=color;
	//	//memcpy( ((uint8_t*)arr)+sizeof(arr[0]), arr, sizeof(arr)-sizeof(arr[0]) ); // this hack does not seem to work on Due
	//}

	//void fillScanline(uint8_t color, size_t seedCopyCount) __attribute__((always_inline)) {
	//	//for(uint16_t i=0; i<seedCopyCount; i++)
	//	//{
	//	//	_scanlineBuffer[i] = color;
	//	//}
	//	_scanlineBuffer[0]=color;
	//	memmove(_scanlineBuffer+sizeof(_scanlineBuffer[0]), _scanlineBuffer, seedCopyCount-sizeof(_scanlineBuffer[0]));
	//	//arr[0]=color;
	//	//memcpy( ((uint8_t*)arr)+sizeof(arr[0]), arr, seedCopyCount-sizeof(arr[0]) );
	//}

	// Sets all pixels in the scanline buffer to the specified color
	__attribute__((always_inline))
		void fillScanline(uint16_t color) {
			_hiByte = highByte(ILI9341_BLACK);
			_loByte = lowByte(ILI9341_BLACK);
			for(uint16_t i=0; i<sizeof(_scanlineBuffer); i+=2)
			{
			_scanlineBuffer[i]=_hiByte;
			_scanlineBuffer[i+1]=_loByte;
			}
			/*_scanlineBuffer[0]=highByte(color);
			_scanlineBuffer[1]=lowByte(color);
			_scanlineBuffer[2]=_scanlineBuffer[0];
			_scanlineBuffer[3]=_scanlineBuffer[1];

			memcpy(_scanlineBuffer+4, _scanlineBuffer, 4);
			memcpy(_scanlineBuffer+8, _scanlineBuffer, 8);
			memcpy(_scanlineBuffer+16, _scanlineBuffer, 16);
			memcpy(_scanlineBuffer+32, _scanlineBuffer, 32);
			memcpy(_scanlineBuffer+64, _scanlineBuffer, 64);
			memcpy(_scanlineBuffer+128, _scanlineBuffer, 128);
			memcpy(_scanlineBuffer+256, _scanlineBuffer, 256);
			memcpy(_scanlineBuffer+512, _scanlineBuffer, 128);*/

			/*for(uint16_t i=0; i<sizeof(_scanlineBuffer); i+=2)
			{
				memcpy(_scanlineBuffer+i, _scanlineBuffer, 2);
			}*/

			//arr[0]=highByte(color);
			//arr[1]=lowByte(color);
			//memcpy( ((uint8_t*)arr)+2*sizeof(arr[0]), arr, sizeof(arr)-2*sizeof(arr[0]) );
	}

	// Sets first n pixels in scanline buffer to the specified color
	__attribute__((always_inline))
		void fillScanline(uint16_t color, size_t n){
			_hiByte = highByte(color);
			_loByte = lowByte(color);
			for(uint16_t i=0; i<(n << 1); i+=2)
			{
				_scanlineBuffer[i]=_hiByte;
				_scanlineBuffer[i+1]=_loByte;
			}

			/*_scanlineBuffer[0]=highByte(color);
			_scanlineBuffer[1]=lowByte(color);
			memmove(((uint8_t*)_scanlineBuffer)+2*sizeof(_scanlineBuffer[0]), _scanlineBuffer, (n << 1) -2*sizeof(_scanlineBuffer[0]));
			*/
	}

	//inline __attribute__((always_inline))
	//	void *memcpy_forward(uint8_t *dst, const uint8_t *src, size_t n)
	//{
	//	uint8_t *pdst;
	//	const uint8_t *psrc;

	//	for (pdst = dst, psrc = src; n > 0; --n, ++pdst, ++psrc)
	//		*pdst = *psrc;

	//	return dst;
	//}
#endif

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
			*_dcport |=  _dcpinmask;
			//_dcport->PIO_SODR |= _dcpinmask;
	}

	// Sets DC to Command (0)	
	inline __attribute__((always_inline)) 
		void setDCForCommand(){
			*_dcport &=  ~_dcpinmask;
	}

protected:
	int16_t
		_width, _height, // Display w/h as modified by current rotation
		cursor_x, cursor_y;
	uint16_t
		textcolor, textbgcolor;
	uint8_t
		textsize,
		rotation;
	boolean
		wrap; // If set, 'wrap' text at right edge of display


private:
	uint8_t  _rst;
	//Pio *_dcport;
	volatile RwReg *_dcport;
	uint32_t  _cs, _dc, _dcpinmask;

#if SPI_MODE_NORMAL | SPI_MODE_DMA
	//Pio *_csport;
	volatile RwReg *_csport;
	uint32_t _cspinmask;
#endif

	/*void writedata(uint8_t c);
	void writecommand(uint8_t c);
	void spiwrite(uint8_t c);
	uint8_t spiread(void) ;*/

	bool pinIsChipSelect(uint8_t cs);

	// Writes a sequence that will render a horizontal line
	// At the end CS is kept enabled.
	// In case of DMA mode, the content of scanline buffer is written
	__attribute__((always_inline))
		void writeHLine_cont(int16_t x, int16_t y, int16_t w, uint16_t color)
	{
#if SPI_MODE_DMA
		//TOTRY move this down
		fillScanline(color, w);
#endif
		setAddrAndRW_cont(x, y, x+w-1, y);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		setDCForData();
		do { write16(color); } while (--w > 0);
#elif SPI_MODE_DMA
		writeScanline_cont(w);
#endif
	}

	// Writes a sequence that will render a horizontal line
	// At the end CS is disabled.
	// In case of DMA mode, the content of scanline buffer is written
	__attribute__((always_inline))
		void writeHLine_last(int16_t x, int16_t y, int16_t w, uint16_t color)
	{
#if SPI_MODE_DMA
		//TOTRY move this down
		fillScanline(color, w);
#endif
		setAddrAndRW_cont(x, y, x+w-1, y);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		setDCForData();
		while (w-- > 1) {
			write16(color);
		}
		write16_last(color);
#elif SPI_MODE_DMA
		writeScanline_last(w);
#endif
	}

	// Writes a sequence that will render a vertical line
	// At the end CS is kept enabled.
	// In case of DMA mode, the content of scanline buffer is filled and written
	__attribute__((always_inline))
		void writeVLine_cont(int16_t x, int16_t y, int16_t h, uint16_t color)
	{
#if SPI_MODE_DMA
		// TRY move this down
		fillScanline(color, h);
#endif
		setAddrAndRW_cont(x, y, x, y+h-1);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		setDCForData();
		do { write16(color); } while (--h > 0);
#elif SPI_MODE_DMA
		writeScanline_cont(h);
#endif
	}

	// Writes a sequence that will render a vertical line
	// At the end CS is disabled.
	// In case of DMA mode, the content of scanline buffer is filled and written
	__attribute__((always_inline))
		void writeVLine_last(int16_t x, int16_t y, int16_t h, uint16_t color)
	{
#if SPI_MODE_DMA
		fillScanline(color, h);
#endif
		setAddrAndRW_cont(x, y, x, y+h-1);
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
		while (h-- > 1) {
			write16(color);
		}
		write16_last(color);
#elif SPI_MODE_DMA
		writeScanline_last(h);
#endif
	}

	// Writes a sequence that will render a horizontal line
	// CS must be set prior to calling this method
	// for DMA mode, scanline buffer must be filled with the desired color
	// Advantage over writeHLine_cont is that CS line is not being set and 
	// the scanlineBuffer not filled on every call
	inline __attribute__((always_inline))
		void writeHLine_cont_noCS(int16_t x, int16_t y, int16_t w, uint16_t color)
		__attribute__((always_inline)) {
			//setAddrAndRW_cont(x, y, x+w-1, y);
			setDCForCommand();
			write8(ILI9341_CASET); // Column addr set
			setDCForData();
			write16(x);   // XSTART
			write16(x+w-1);   // XEND
			setDCForCommand();
			write8(ILI9341_PASET); // Row addr set
			setDCForData();
			write16(y);   // XSTART
			write16(y);   // XEND
			setDCForCommand();
			write8(ILI9341_RAMWR);
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
			do { write16(color); } while (--w > 0);
#elif SPI_MODE_DMA
			write_cont(_scanlineBuffer, w << 1);
#endif
	}

	// Writes a sequence that will render a vertical line
	// CS must be set prior to calling this method
	// for DMA mode, scanline buffer must be filled with the desired color
	// Advantage over writeVLine_cont is that CS line is not being set and 
	// the scanlineBuffer not filled on every call
	inline __attribute__((always_inline))
		void writeVLine_cont_noCS(int16_t x, int16_t y, int16_t h, uint16_t color)
		__attribute__((always_inline)) {
			setDCForCommand();
			write8(ILI9341_CASET); // Column addr set
			setDCForData();
			write16(x);   // XSTART
			write16(x);   // XEND
			setDCForCommand();
			write8(ILI9341_PASET); // Row addr set
			setDCForData();
			write16(y);   // XSTART
			write16(y+h-1);   // XEND
			setDCForCommand();
			write8(ILI9341_RAMWR);
			setDCForData();
#if SPI_MODE_NORMAL | SPI_MODE_EXTENDED
			do { write16(color); } while (--h > 0);
#elif SPI_MODE_DMA
			write_cont(_scanlineBuffer, h << 1);
#endif
	}


	//void HLine_cont(int16_t x, int16_t y, int16_t w, uint16_t color)
	//	__attribute__((always_inline)) {
	//		setAddrAndRW_cont(x, y, x+w-1, y);
	//		do { writedata16_cont(color); } while (--w > 0);
	//}
	//void VLine_cont(int16_t x, int16_t y, int16_t h, uint16_t color)
	//	__attribute__((always_inline)) {
	//		setAddrAndRW_cont(x, y, x, y+h-1);
	//		do { writedata16_cont(color); } while (--h > 0);
	//}

	// Writes a sequence that will render a pixel
	// CS must be enabled prior to calling this method
	// Advantage over writePixel_cont is that CS line is not set on every call
	inline __attribute__((always_inline))
		void writePixel_cont_noCS(int16_t x, int16_t y, uint16_t color)
	{
		setDCForCommand();
		write8(ILI9341_CASET); // Column addr set
		setDCForData();
		write16(x);   // XSTART
		write16(x);   // XEND
		setDCForCommand();
		write8(ILI9341_PASET); // Row addr set
		setDCForData();
		write16(y);   // XSTART
		write16(y);   // XEND
		setDCForCommand();
		write8(ILI9341_RAMWR);
		setDCForData();
		write16(color);
	}

	inline __attribute__((always_inline)) 
		void writePixel_cont(int16_t x, int16_t y, uint16_t color)
	{
		enableCS();
		setDCForCommand();
		write8(ILI9341_CASET); // Column addr set
		setDCForData();
		write16(x);   // XSTART
		write16(x);   // XEND
		setDCForCommand();
		write8(ILI9341_PASET); // Row addr set
		setDCForData();
		write16(y);   // XSTART
		write16(y);   // XEND
		setDCForCommand();
		write8(ILI9341_RAMWR);
		setDCForData();
		write16(color);
	}

	// Enables CS, writes a sequence that will render a pixel and disables CS
	inline __attribute__((always_inline))
		void writePixel_last(int16_t x, int16_t y, uint16_t color)
	{
		setAddrAndRW_cont(x, y, x, y);
		setDCForData();
		write16_last(color);
	}


	bool waitNotBusy(uint16_t timeoutMillis) {
		uint16_t t0 = millis();
		while (read8() != 0XFF) {
			if (((uint16_t)millis() - t0) >= timeoutMillis) goto fail;
			//spiYield();
		}
		return true;

fail:
		return false;
	}


};

#ifndef swap
#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
#endif

#endif
