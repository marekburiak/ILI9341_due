ILI9341_Due
===========

Arduino Due library for interfacing with ILI9341-based TFT's in SPI, Extended SPI and DMA SPI mode.

This library is based on 3 libraries:

<b>ili9341_t3</b> from Paul Stoffregen - https://github.com/PaulStoffregen/ILI9341_t3<br>
<b>SdFat</b> from Bill Greiman - https://github.com/greiman/SdFat<br>
<b>GLCD</b> from Michael Margolis and Bill Perry - https://code.google.com/p/glcd-arduino<br>

ili9341_t3 library where various optimizations for Adafruit's ili9341 and GFX libraries were implemented was used as a base for ili9341_due.<br>
One class from SdFat library is used for utilizing Due's DMA in SPI transfers which provides the main speed boost.<br>
gText class from GLCD library was used as a base for rendering custom fonts. Custom font .h files can be generated with GLCDFontCreator2 tool:<br>
https://code.google.com/p/glcd-arduino/downloads/detail?name=GLCDFontCreator2.zip&can=2&q=

As for the wiring, use Due's HW SPI pins.
If you look into ILI9341_due.h, you should find this:
```Arduino
// comment out the SPI mode you want to use
//#define ILI9341_SPI_MODE_NORMAL
//#define ILI9341_SPI_MODE_EXTENDED
#define ILI9341_SPI_MODE_DMA
```

Uncomment the line depending on the SPI mode you want to use. As you can see, DMA mode is the default.

<b>SPI_MODE_NORMA</b>L is the standard SPI mode where you can use any CS line but you have to drive it yourself.<br>
<b>SPI_MODE_EXTENDED</b> is the extended SPI mode available in Due where CS line is handled by the chip (which is faster than handling it yourself). You are restricted to these CS pins though - 4, 10 and 52 (as described here: http://arduino.cc/en/Reference/DueExtendedSPI).<br>
<b>SPI_MODE_DMA</b> utilized DMA to do SPI transfers. You should be able to use any digital pin for CS.

```Arduino
#define ILI9341_SPI_CLKDIVIDER 2
```

This is used to set the SPI clock divider, which at 2 means 84/2=42MHz (the max). If you are getting glitches on the screen or it just does not work try to use a higher divider to bring the frequency down (e.g. 11 to run it at 8-ish MHz).



```
------------------------------------------

This is a library for the Adafruit ILI9341 display products

This library works with the Adafruit 2.8" Touch Shield V2 (SPI)
  ----> http://www.adafruit.com/products/1651
 
Check out the links above for our tutorials and wiring diagrams.
These displays use SPI to communicate, 4 or 5 pins are required
to interface (RST is optional).

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
MIT license, all text above must be included in any redistribution
```
