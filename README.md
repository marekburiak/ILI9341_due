ILI9341_due
===========

Please see http://marekburiak.github.io/ILI9341_due


Version History:
```
v0.92.000 - Added drawArc method for drawing arcs and circles with thickness and pies
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