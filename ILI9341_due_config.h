/*
v0.94.000

ILI9341_due_config.h - Arduino Due library for interfacing with ILI9341-based TFTs

Copyright (c) 2014  Marek Buriak

*/

#ifndef _ILI9341_due_configH_
#define _ILI9341_due_configH_

// comment out the SPI mode you want to use (does not matter for AVR)
//#define ILI9341_SPI_MODE_NORMAL	// uses SPI library
//#define ILI9341_SPI_MODE_EXTENDED	// uses Extended SPI in Due, make sure you use pin 4, 10 or 52 for CS
#define ILI9341_SPI_MODE_DMA		// uses DMA in Due

// set the clock divider
#if defined __SAM3X8E__
#define ILI9341_SPI_CLKDIVIDER 10	// for Due
#elif defined __AVR__
#define ILI9341_SPI_CLKDIVIDER SPI_CLOCK_DIV2	// for Uno, Mega,...
#endif

// uncomment if you want to use SPI transactions. Uncomment it if the library does not work when used with other libraries.
//#define ILI_USE_SPI_TRANSACTION


// comment out the features you do not need to save flash memory and RAM (especially on AVR)

#define FEATURE_GTEXT_ENABLED
// commenting out/disabling the ARC feature will exclude the drawArc function. It is going to save a few ROM bytes.
#define FEATURE_ARC_ENABLED
// commenting out/disabling the PRINT feature will exclude the drawChars and print functions, it saves around 3.6kB ROM and 1.3kB RAM(!)
// I recommend using gText for drawing the text.
//#define FEATURE_PRINT_ENABLED
#define FEATURE_GTEXT_PRINT_ENABLED

// 
//#define LINE_SPACING_AS_PART_OF_LETTERS

// comment out if you do need to use scaled text. The text will draw then faster.
#define TEXT_SCALING_ENABLED

// number representing the maximum angle (e.g. if 100, then if you pass in start=0 and end=50, you get a half circle)
// this can be changed with setArcParams function at runtime
#define ARC_ANGLE_MAX 360		
// rotational offset in degrees defining position of value 0 (-90 will put it at the top of circle)
// this can be changed with setAngleOffset function at runtime
#define ANGLE_OFFSET -90	


#endif
