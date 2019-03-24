ILI9341_due
===========

Please see http://marekburiak.github.io/ILI9341_due


Version History:
```
v1.01.009 - added QWERTY touch keyboard example by Mark Miles (ZioGuido), thanks
v1.01.008 - fixed buffered line drawing in cases where the line length is equal to buffer size (thanks doppelT)
v1.01.007 - fixed fillRect function ambiguity when compiling some examples (thanks MartyMacGyver)
v1.01.006 - fixed font rendering for font heights that are multiples of 8 (thanks Wolf)
v1.01.005 - added getTextArea()
          - renamed charWidth() -> getCharWidth() (previously undocummented function)
          - renamed stringWidth() -> getStringWidth() (previously undocummented function)
          - renamed fontHeight() -> getFontHeight() (previously undocummented function)
          - added documentation for functions
               - cursorToXY
               - cursorTo
               - getCharWidth
               - getStringWidth
               - getFontHeight
v1.01.004 - added some getters:
               - getCursorX
               - getCursorY
               - getFont
               - getTextColor
               - getTextBackgroundColor
               - getFontMode
               - getTextScale
v1.01.003 - removed the reference to UTFT.h from uTouch* examples
v1.01.002 - improved the speed of fillCircleHelper (thanks to Chris_CT for this optimization)
v1.01.001 - added getDisplayStatus function
v1.01.000 - added UTouch examples
v1.00.001 - removed some unused variables and functions, fixed compiler warnings
v1.00.000 - Breaking changes:
                - functions renamed:
                    drawArc -> fillArc
                    selectFont -> setFont
                    defineArea -> setTextArea
                    setFontColor -> setTextColor
                    setCursor -> cursorToXY
                    setTextSize -> setTextScale
                    setFontLetterSpacing -> setTextLetterSpacing
                    getFontLetterSpacing -> getTextLetterSpacing
                    drawString -> print, printAt, printAligned
                    drawStringOffseted -> printAlignedOffseted
                    drawStringPivoted -> printAlignedPivoted
                    drawStringPivotedOffseted -> printAlignedPivotedOffseted
                - setTextArea (previously defineArea) and all rectangle functions are now set 
                  by x,y,width,height (previously x1,y1,x2,y2)
                - images in .565 format must be generated again with the current BMP24toILI565
                  tool
                - removed glcdfont, use SystemFont5x7 instead
          - New functions
                - setAngleOffset (extracted from setArcParams)
                - drawLineByAngle
                - printAtOffseted
                - clearArea
                - drawImage
                - setTextLineSpacing
                - drawBitmap - well not a new function but now it actually works
          - New additions
                - SPI transactions support
                - added support for String and FlashStringHelper*
                - supporting '\n' in strings
                - much more predefined colors like ILI9341_CHOCOLATE or ILI9341_SKYBLUE
                - BMP24toILI9341Array - a tool to convert BMP image to an array
                  (so you can draw small images directly from memory, no need for an SD card)
                - a few new example sketches
          - Other changes
                - many speed improvements
                - removed ILI9341_due_gText.h, ILI_SDSpi.h, ILI_SdFatConfig.h
                - added ILI9341_due_config.h
                - gText is now embedded directly in the ILI9341_due library so you do not
                  need to create ILI9341_due_gText objects anymore. Just call tft.print
                - updated github.io page, documented all functions with examples and 
                  pictures!
                - ILIScreenshotViewer has embedded console and automatically loads
                  screenshots (no need to copy/paste strings)
                - everything else I forgot about (let me know in the arduino forum)


v0.94.000 - Added AVR compatibility, the library can now be also used on Uno, Mega and alike.
			Please check the github.io page for more information (especially if you want to
			use gText)
v0.93.000 - Breaking changes:
                - setRotation now needs iliRotation enum as a parameter (instead of an int)
                - the meaning of some gText drawString parameters have changed 
                  (event though the parameter type is the same)
          - New additions:
                - gText drawString with new parameters
                - new gText drawStringOffseted, drawStringPivoted, drawStringPivotedOffseted 
                  functions
          - gText fontLetterSpacing default value is now 2 (previously 0)
          - examples updated

v0.92.002 - Fixed drawArc
		  - Added setArcParams function to change maxArcAngle and arcAngleOffset at runtime

v0.92.001 - Added fontHeight function in gText
          - fixes for NORMAL and EXTENDED mode

v0.92.000 - Added drawArc function for drawing arcs and circles with thickness and pies
          - Added screenshotToConsole method and ILIScreenShotViewer app for taking 
            screenshots from the display
          - Added alignment options for drawString in ILI9341_due_gText
	  
v0.91.002 - Updated graphicstestWithStats example sketch so it does not use Streaming.h

v0.91.001 - Performance improvements (especially fill circle)
          - Fixed a scanline fill bug, clean up commented out code

v0.91.000 - Added functions for controlling TFT power levels (sleep, idle, setPowerLevel)

v0.90.026 - Fixed fillScreen after recent changes

v0.90.025 - Added PushColors565 function
          - Added BMP24toILI565 tool
          - Updated sdFatTftBitmap example

v0.90.010 - Initial version
```