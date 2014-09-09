#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <ILI9341_due_gText.h>
#include <ILI9341_due.h>

// CS and DC for the LCD
#define LCD_CS 10	// Chip Select for LCD
#define LCD_DC 9	// Command/Data for LCD

#define SD_CS 7		// Chip Select for SD card

#define BUFFPIXEL 320	// size of the buffer in pixels
#define SD_SPI_SPEED SPI_HALF_SPEED	// SD card SPI speed, try SPI_FULL_SPEED
#define FILENAME "picture.bmp"		// filename of the file you want to display from the SD card

// In the SD card, place 24 bit color BMP files (be sure they are 24-bit!)

SdFat sd; // set filesystem
SdFile bmpFile; // set filesystem
//ArduinoOutStream cout(Serial);

ILI9341_due tft(LCD_CS, LCD_DC);


// store error strings in flash to save RAM
#define error(s) sd.errorHalt_P(PSTR(s))


void setup()
{
	Serial.begin(9600);

	tft.begin();
	tft.setRotation(3);	// landscape
	progmemPrint(PSTR("Initializing SD card..."));
	
	if(!sd.begin(SD_CS, SD_SPI_SPEED)){
		progmemPrintln(PSTR("failed!"));
		return;
	}
	progmemPrintln(PSTR("OK!"));

	bmpDraw(FILENAME, 0, 0);
}

void loop()
{
	
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's RAM but
// makes loading a little faster.  

void bmpDraw(char* filename, int x, int y) {

	SdFile     bmpFile;
	int      bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
	uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
	boolean  goodBmp = false;       // Set to true on valid header parse
	boolean  flip    = true;        // BMP is stored bottom-to-top
	uint16_t w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0, startTime = millis();
	uint16_t  lcdidx = 0;
	boolean  first = true;

	if((x >= tft.width()) || (y >= tft.height())) return;

	Serial.println();
	progmemPrint(PSTR("Loading image '"));
	Serial.print(filename);
	Serial.println('\'');
	// Open requested file on SD card
	if (!bmpFile.open(filename, O_READ)) {
		Serial.println("File open failed.");
		return;
	} else {
		Serial.println("File opened.");
	}

	// Parse BMP header
	if(read16(bmpFile) == 0x4D42) { // BMP signature
		progmemPrint(PSTR("File size: ")); Serial.println(read32(bmpFile));
		(void)read32(bmpFile); // Read & ignore creator bytes
		bmpImageoffset = read32(bmpFile); // Start of image data
		progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
		// Read DIB header
		progmemPrint(PSTR("Header size: ")); Serial.println(read32(bmpFile));
		bmpWidth  = read32(bmpFile);
		bmpHeight = read32(bmpFile);
		if(read16(bmpFile) == 1) { // # planes -- must be '1'
			bmpDepth = read16(bmpFile); // bits per pixel
			progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
			if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

				goodBmp = true; // Supported BMP format -- proceed!
				progmemPrint(PSTR("Image size: "));
				Serial.print(bmpWidth);
				Serial.print('x');
				Serial.println(bmpHeight);

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if(bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip      = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;
				if((x+w-1) >= tft.width())  w = tft.width()  - x;
				if((y+h-1) >= tft.height()) h = tft.height() - y;

				// Set TFT address window to clipped image bounds
				tft.setAddrWindow(x, y, x+w-1, y+h-1);

				for (row=0; row<h; row++) { // For each scanline...
					// Seek to start of scan line.  It might seem labor-
					// intensive to be doing this on every line, but this
					// method covers a lot of gritty details like cropping
					// and scanline padding.  Also, the seek only takes
					// place if the file position actually needs to change
					// (avoids a lot of cluster math in SD library).
					
					if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + row * rowSize;
					if(bmpFile.curPosition() != pos) { // Need seek?
						bmpFile.seekSet(pos);
					}

					for (col=0; col<w; col+=BUFFPIXEL) 
					{
						// read pixels into the buffer
						bmpFile.read(sdbuffer, 3*BUFFPIXEL);

						// convert color
						for(int p=0; p < BUFFPIXEL; p++)
						{
							b = sdbuffer[3*p];
							g = sdbuffer[3*p+1];
							r = sdbuffer[3*p+2];
							lcdbuffer[p] = tft.color565(r,g,b);
						}
						// push buffer to TFT
						tft.pushColors(lcdbuffer, 0, BUFFPIXEL);
					}

					// render any remaining pixels that did not fully fill the buffer
					uint16_t remainingPixels = w % BUFFPIXEL;
					if(remainingPixels > 0)
					{
						bmpFile.read(sdbuffer, 3*remainingPixels);

						for(int p=0; p < remainingPixels; p++)
						{
							b = sdbuffer[3*p];
							g = sdbuffer[3*p+1];
							r = sdbuffer[3*p+2];
							lcdbuffer[p] = tft.color565(r,g,b);
						}

						tft.pushColors(lcdbuffer, 0, remainingPixels);
					}

				}
				progmemPrint(PSTR("Loaded in "));
				Serial.print(millis() - startTime);
				Serial.println(" ms");
			}
		}
	}
	Serial.println(read16(bmpFile), HEX);

	bmpFile.close();
	if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}


// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(SdFile& f) {
	uint16_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t read32(SdFile& f) {
	uint32_t result;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
	char c;
	while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
	progmemPrint(str);
	Serial.println();
}

//void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
//{
//      char tmp[length*5+1];
//      byte first;
//      byte second;
//      for (int i=0; i<length; i++) {
//            first = (data[i] >> 4) & 0x0f;
//            second = data[i] & 0x0f;
//            // base for converting single digit numbers to ASCII is 48
//            // base for 10-16 to become upper-case characters A-F is 55
//            // note: difference is 7
//            tmp[i*5] = 48; // add leading 0
//            tmp[i*5+1] = 120; // add leading x
//            tmp[i*5+2] = first+48;
//            tmp[i*5+3] = second+48;
//            tmp[i*5+4] = 32; // add trailing space
//            if (first > 9) tmp[i*5+2] += 7;
//            if (second > 9) tmp[i*5+3] += 7;
//      }
//      tmp[length*5] = 0;
//      Serial.println(tmp);
//}