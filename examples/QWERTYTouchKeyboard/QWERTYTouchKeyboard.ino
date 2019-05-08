//////////////////////////////////////////////////////////////////////
// QWERTY Touch Keyboard Example For Arduino DUE
// by Guido Scognamiglio - Mar 2019
// Based on the ILI9341 library by Marek Buriak
// https://github.com/marekburiak/ILI9341_due
// Uses URTouch library for touch controller XPT2046
//

#include <SPI.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include "fonts/Arial_bold_14.h"

#include <URTouch.h>

// For the Adafruit shield, these are the default.
#define TFT_RST 8
#define TFT_DC  9
#define TFT_CS 10

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

URTouch  myTouch(30, 28, 26, 24, 22);

#define STRING_MAX_LENGTH   32
#define KEY_ROWS            3
#define KEY_WIDTH           32
#define KEY_HEIGHT          40
#define KEY_PADDING_LEFT    12
#define KEY_PADDING_TOP     10
#define KEYBOARD_OFFSET_Y   40

int x, y, scanx, scany;
int Charset = 0;
String stCurrent = "";
int stCurrentLen = 0;

const String Keys[3][4][10] = 
{
  { // Lowecase
    { "q", "w", "e", "r", "t", "y", "u", "i", "o", "p" }, 
    { "a", "s", "d", "f", "g", "h", "j", "k", "l", "'" }, 
    { "z", "x", "c", "v", "b", "n", "m", ",", ".", "-" },
    { "SY", "SH", "SH"," "," "," "," "," ", "DL", "DL" }
  },

  { // Uppercase
    { "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P" }, 
    { "A", "S", "D", "F", "G", "H", "J", "K", "L", "?" }, 
    { "Z", "X", "C", "V", "B", "N", "M", ";", ":", "_" },
    { "SY", "SH", "SH"," "," "," "," "," ", "DL", "DL" }
  },

  { // Symbols
    { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" }, 
    { "!", "\"", "~", "$", "%", "&", "/", "(", ")", "=" }, 
    { "|", "<", ">", "[", "]", "^", "+", "*", "@", "#" },
    { "SY", "SH", "SH"," "," "," "," "," ", "DL", "DL" }
  }  
};

void drawScreen()
{
  // Text box
  tft.drawRoundRect(2, 2, 316, 36, 5, ILI9341_WHITE);

  // CANCEL
  tft.fillRoundRect(0, 240-36, 120, 36, 5, ILI9341_YELLOW);
  tft.drawRoundRect(0, 240-36, 120, 36, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK, ILI9341_YELLOW);
  tft.printAt("CANCEL", 28, 240-36+12);
  
  // DONE
  tft.fillRoundRect(320-120, 240-36, 120, 36, 5, ILI9341_GREEN);
  tft.drawRoundRect(320-120, 240-36, 120, 36, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_WHITE, ILI9341_GREEN);
  tft.printAt("DONE", 320-120+40, 240-36+12);
}

void drawKeyboard()
{
  tft.setTextColor(ILI9341_BLACK, ILI9341_WHITE);
 
  for (y = 0; y < KEY_ROWS; ++y)
  {
    for (x = 0; x < 10; ++x)
    {
      tft.fillRoundRect((x * KEY_WIDTH), KEYBOARD_OFFSET_Y + (y*KEY_HEIGHT), KEY_WIDTH, KEY_HEIGHT, 5, ILI9341_WHITE);
      tft.drawRoundRect((x * KEY_WIDTH), KEYBOARD_OFFSET_Y + (y*KEY_HEIGHT), KEY_WIDTH, KEY_HEIGHT, 5, ILI9341_BLACK);
      
      // Print characters
      tft.printAt(Keys[Charset][y][x], KEY_PADDING_LEFT + (x * KEY_WIDTH), KEYBOARD_OFFSET_Y + KEY_PADDING_TOP + (y * KEY_HEIGHT));
    }
  }
  
  // SYMBOLS
  tft.fillRoundRect(0, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH, KEY_HEIGHT, 5, ILI9341_WHITE);
  tft.drawRoundRect(0, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH, KEY_HEIGHT, 5, ILI9341_BLACK);
  tft.printAt(Charset == 2 ? "ABC" : "123", 2, KEYBOARD_OFFSET_Y + KEY_PADDING_TOP + (KEY_ROWS * KEY_HEIGHT));

  // SHIFT
  tft.fillRoundRect(KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*2, KEY_HEIGHT, 5, ILI9341_WHITE);
  tft.drawRoundRect(KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*2, KEY_HEIGHT, 5, ILI9341_BLACK);
  tft.printAt(Charset ? "SHIFT" : "Shift", 14 + KEY_WIDTH, KEYBOARD_OFFSET_Y + KEY_PADDING_TOP + (KEY_ROWS * KEY_HEIGHT));
  
  // SPACE
  tft.fillRoundRect(3*KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*5, KEY_HEIGHT, 5, ILI9341_WHITE);
  tft.drawRoundRect(3*KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*5, KEY_HEIGHT, 5, ILI9341_BLACK);
  tft.printAt(Charset ? "SPACE" : "Space", 16 + 4*KEY_WIDTH, KEYBOARD_OFFSET_Y + KEY_PADDING_TOP + (KEY_ROWS * KEY_HEIGHT));
  
  // DELETE
  tft.fillRoundRect(8*KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*2, KEY_HEIGHT, 5, ILI9341_WHITE);
  tft.drawRoundRect(8*KEY_WIDTH, KEYBOARD_OFFSET_Y + (KEY_ROWS*KEY_HEIGHT), KEY_WIDTH*2, KEY_HEIGHT, 5, ILI9341_BLACK);
  tft.printAt(Charset ? "DELETE" : "Delete", 6 + 8*KEY_WIDTH, KEYBOARD_OFFSET_Y + KEY_PADDING_TOP + (KEY_ROWS * KEY_HEIGHT));
}

void ProcessKeyTouch(int x, int y)
{
  int TouchX = -1, TouchY = -1;
  
  // Scan X
  for (scanx = 0; scanx < 10; ++scanx)
  {
    if ((x >= (scanx * KEY_WIDTH)) && (x <= (scanx * KEY_WIDTH + KEY_WIDTH)))
    {
      TouchX = scanx;
      break;
    }
  }

  // Scan Y
  for (scany = 0; scany < KEY_ROWS+1; ++scany)
  {
    if ((y >= (KEYBOARD_OFFSET_Y + scany * KEY_HEIGHT)) && (y <= (KEYBOARD_OFFSET_Y + scany * KEY_HEIGHT + KEY_HEIGHT)))
    {
      TouchY = scany;
      break;
    }
  }

  // Hit a key?
  if (TouchX < 0 || TouchY < 0)
    return;
  
  // Shift, change character set
  if (Keys[Charset][TouchY][TouchX] == "SH")
  {
    Charset = !Charset;
    drawKeyboard();
  }

  // Switch to Symbols
  else
  if (Keys[Charset][TouchY][TouchX] == "SY")
  {
    Charset = Charset < 2 ? 2 : 0;
    drawKeyboard();
  }

  // Type character
  else
  {
    String val = Keys[Charset][TouchY][TouchX];
    
    // Delete last character
    if (val.equals("DL"))
    {
      if (stCurrentLen > 0)
      {
        stCurrent = stCurrent.substring(0, stCurrent.length() - 1);
        stCurrentLen--;
      }
    // Add character to the string  
    } else {
      if (stCurrentLen < STRING_MAX_LENGTH)
      {
        stCurrent += val;
        stCurrentLen++;
      } else 
        return;
    }
      
    
    // Print the typed text
    tft.setTextColor(ILI9341_LIME, ILI9341_BLACK);
    tft.printAlignedOffseted(stCurrent, gTextAlignTopCenter, 0, 16, gTextEraseFullLine);
    
    // Redraw text box
    tft.drawRoundRect(2, 2, 316, 36, 5, ILI9341_WHITE);
  }
  
  // Pause until the touch is released
  while (myTouch.dataAvailable())
    myTouch.read();
  
  // For debouncing
  delay(100);
}


void setup()
{
  // Backlight on A0
  pinMode(A0, OUTPUT);
  analogWrite(A0, 255);

  Serial.begin(9600);
  
  // Initial setup
  tft.begin();
  tft.setRotation(iliRotation270);  // landscape
  tft.fillScreen(ILI9341_BLACK);

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  tft.setFont(Arial_bold_14);
  drawScreen();
  drawKeyboard();
}


void loop()
{
  while (true)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x = myTouch.getX();
      y = myTouch.getY();
      
      ProcessKeyTouch(x, y); 
    }
  }
}

