ILI9341_due
===========

Arduino Due library for interfacing with ILI9341-based TFT's in SPI, Extended SPI and DMA SPI mode.

This library is based on 3 libraries:

<b>ili9341_t3</b> from Paul Stoffregen - https://github.com/PaulStoffregen/ILI9341_t3<br>
<b>SdFat</b> from Bill Greiman - https://github.com/greiman/SdFat<br>
<b>GLCD</b> from Michael Margolis and Bill Perry - https://code.google.com/p/glcd-arduino<br>

ili9341_t3 library where various optimizations for Adafruit's ili9341 and GFX libraries were implemented was used as a base for ili9341_due.<br>
One class from SdFat library is used for utilizing Due's DMA in SPI transfers which provides the main speed boost.<br>
gText class from GLCD library was used as a base for rendering custom fonts. The maximum height of the font is 255px. Custom font .h files can be generated with GLCDFontCreator2 tool (requires Java). It is included in the tools folder or you can dowload it from:<br>
https://code.google.com/p/glcd-arduino/downloads/detail?name=GLCDFontCreator2.zip&can=2&q=<br>
Here is a video of rendering a 255px-high font: https://www.youtube.com/watch?v=OcTz_YBDXmo


As for the wiring, use Due's HW SPI pins (e.g. described here: http://21stdigitalhome.blogspot.com/2013/02/arduino-due-hardware-spi.html). A pin for CS depends on the SPI mode you choose.
If you look into ILI9341_due.h, you should find this:
```Arduino
// comment out the SPI mode you want to use
//#define ILI9341_SPI_MODE_NORMAL
//#define ILI9341_SPI_MODE_EXTENDED	// make sure you use pin 4, 10 or 52 for CS
#define ILI9341_SPI_MODE_DMA
```

Uncomment the line depending on the SPI mode you want to use. As you can see, DMA mode is the default.

<b>SPI_MODE_NORMAL</b> is the standard SPI mode where you can use any digital pin for CS but you/library has to drive it manually.<br>
<b>SPI_MODE_EXTENDED</b> is the extended SPI mode available in Due where CS pin is handled by the chip (which is faster than handling it manually). You are restricted to these pins for CS though - 4, 10 and 52 (as described here: http://arduino.cc/en/Reference/DueExtendedSPI).<br>
<b>SPI_MODE_DMA</b> is utilizing DMA to do SPI transfers. You should be able to use any digital pin for CS.

Here is a video where you can see the speed difference: 
https://www.youtube.com/watch?v=vnEwzN14BsU

```Arduino
#define ILI9341_SPI_CLKDIVIDER 2
```

This is used to set the SPI clock frequency, which at 2 means 84/2=42MHz (the max). If you are getting glitches on the screen or it just does not work try to use a higher divider to bring the frequency down (e.g. 11 to run it at 8-ish MHz). I was using 20cm dupont cables to connect TFT with Due and haven't had any issues running it with the divider set to 2.


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

Known bugs/issues/missing functionality:
- I am sure there are bugs
- sdFatTftBitmap demo not working properly
- new line is not handled when rendering with gText

I am not an Arduino expert so please feel free to fix/enhance this lib. I plan to add some more features (like the ones above) but I am not sure when will that happen as I have very little time to work on it (if I want to get at least 5 hours of sleep).
