// Paint application - Demonstate both TFT and Touch Screen
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#include <stdint.h>
#include <TouchScreen.h>
#include <TFT.h>
 
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // mega
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 54   // can be a digital pin, this is A0
#define XP 57   // can be a digital pin, this is A3 
 
#elif defined(__AVR_ATmega32U4__) // leonardo
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 18   // can be a digital pin, this is A0
#define XP 21   // can be a digital pin, this is A3 
 
#else //168, 328, something else
#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3 
 
#endif
 
//Measured ADC values for (0,0) and (210-1,320-1)
//TS_MINX corresponds to ADC value when X = 0
//TS_MINY corresponds to ADC value when Y = 0
//TS_MAXX corresponds to ADC value when X = 240 -1
//TS_MAXY corresponds to ADC value when Y = 320 -1
 
#define TS_MINX 140
#define TS_MAXX 900
#define TS_MINY 120
#define TS_MAXY 940
 
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// The 2.8" TFT Touch shield has 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //init TouchScreen port pins

void drawRacePage() {
  //Max X,Y and length for font size 5 with display DOWN2UP
  Tft.drawString("Race Mode", 2, 322, 4, WHITE);
}

void drawValue() {
  Tft.drawString("Temp:218", 205, 324, 5, WHITE);
}

int getTextColor(int state) {
  if (state == 0) {
    return WHITE;
  } else {
    return RED;
  }
}

String getOnOffText(int state) {
  if (state == 0) {
    return "on ";
  } else {
    return "off";
  }
}

int getBgColor(int state) {
  if (state == 0) {
    return GREEN;
  } else {
    return RED;
  }
}

#define LNCHBTNX 63
#define LNCHBTNY 140
#define LNCHBTNW 40
#define LNCHBTNH 167
void drawLaunchBtn(int state) {
  Tft.fillRectangle(LNCHBTNX+2, LNCHBTNY+LNCHBTNH-2, LNCHBTNW-4, LNCHBTNH-4, BLACK);
  Tft.drawRectangle(LNCHBTNX, LNCHBTNY, LNCHBTNW, LNCHBTNH, getBgColor(state));
  String msg = "Launch Control: " + getOnOffText(state);
  char cmsg[20];
  msg.toCharArray(cmsg, 20);
  Tft.drawString(cmsg, 80, 300, 1, getTextColor(state));
}
int inLaunchBtn(TSPoint p) {
  // returns 1 if p s within the Traction control button
  if (p.x >= LNCHBTNX && p.x <= LNCHBTNX+LNCHBTNW) {
    if (p.y >= LNCHBTNY && p.y <= LNCHBTNY+LNCHBTNH) return 1;
  }
  return 0;
}


#define TRACBTNX 113
#define TRACBTNY 126
#define TRACBTNW 40
#define TRACBTNH 180
void drawTractionBtn(int state) {
  Tft.fillRectangle(TRACBTNX+2, TRACBTNY+TRACBTNH-2, TRACBTNW-4, TRACBTNH-4, BLACK);
  Tft.drawRectangle(TRACBTNX, TRACBTNY, TRACBTNW, TRACBTNH, getBgColor(state));
  String msg = "Traction Control: " + getOnOffText(state);
  char cmsg[22];
  msg.toCharArray(cmsg, 22);
  Tft.drawString(cmsg, 130, 300, 1, getTextColor(state));
}
int inTractionBtn(TSPoint p) {
  // returns 1 if p s within the Traction control button
  if (p.x >= TRACBTNX && p.x <= TRACBTNX+TRACBTNW) {
    if (p.y >= TRACBTNY && p.y <= TRACBTNY+TRACBTNH) return 1;
  }
  return 0;
}

// Main states for buttons
int lnchBtnState = 0;
int tracBtnState = 0;

void setup()
{
    Tft.init();  //init TFT library
    Tft.setOrientation(ORIENTHORIZONTAL);
    Tft.setDisplayDirect(DOWN2UP);
    pinMode(0,OUTPUT);
    drawRacePage();
    drawValue();
    drawLaunchBtn(lnchBtnState);
    drawTractionBtn(tracBtnState);
    //Draw the pallet
}

void loop()
{
  // a point object holds x y and z coordinates.
  TSPoint p = ts.getPoint();
  //map the ADC value read to into pixel co-ordinates
  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);
  if (p.z > ts.pressureThreshhold) {
    if ( inTractionBtn(p) ) {
      tracBtnState = 1 - tracBtnState;
      drawTractionBtn(tracBtnState);
      Serial.print("Hit traction button");
    }
    if ( inLaunchBtn(p) ) {
      lnchBtnState = 1 - lnchBtnState;
      drawLaunchBtn(lnchBtnState);
      Serial.print("Hit launchctl button");
    }
  }
  //Tft.fillCircle(p.x,p.y,2,color);
}
