/**************************************************************************
 *                                                                       *
     NeoPixel Ring Clock
     by Andy Doro (mail@andydoro.com)
     http://andydoro.com/ringclock/
 *                                                                      *
 **************************************************************************


   Revision History

   Date      By    What
   20140320    AFD   First draft
   20160105    AFD   Faded arcs
   20160916    AFD   Trinket compatible
   20170727    AFD   added STARTPIXEL for 3D enclosure, variable starting point, added automatic DST support
*/

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

// define pins
#define NEOPIN 3

#define BRIGHTNESS 64 // set max brightness

#define STARTPIXEL 0 // where do we start on the loop? use this to shift the arcs if the wiring does not start at the "12" point

// Do you live in a country or territory that observes Daylight Saving Time?
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// Use 1 if you observe DST, 0 if you don't. This is programmed for DST in the US / Canada. If your territory's DST operates differently,
// you'll need to modify the code in the calcTheTime() function to make this work properly.
#define OBSERVE_DST 1

RTC_DS1307 RTC; // Establish clock object
DateTime theTime; // Holds current clock time

Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, NEOPIN, NEO_GRB + NEO_KHZ800); // strip object

byte hourval, minuteval, secondval; // holds the time

byte pixelColorRed, pixelColorGreen, pixelColorBlue; // holds color values

void setup () {
  Wire.begin();  // Begin I2C
  RTC.begin();   // begin clock

  //Serial.begin(9600);
  // set pinmodes
  pinMode(NEOPIN, OUTPUT);


  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
    theTime = RTC.now();
    // DST? If we're in it, let's subtract an hour from the RTC time to keep our DST calculation correct. This gives us
    // Standard Time which our DST check will add an hour back to if we're in DST.
    if (OBSERVE_DST == 1) {
      if (checkDst() == true) { // check whether we're in DST right now. If we are, subtract an hour.
        theTime = theTime.unixtime() - 3600;
      }
    }
    RTC.adjust(theTime);
  }

  strip.begin();
  //strip.show(); // Initialize all pixels to 'off'

  strip.setBrightness(BRIGHTNESS); // set brightness

  // startup sequence
  delay(500);
  colorWipe(strip.Color(255, 0, 0), 20); // Red
  colorWipe(strip.Color(0, 255, 0), 20); // Green
  colorWipe(strip.Color(0, 0, 255), 20); // Blue
  delay(500);

}

void loop () {


  char* colon = ":"; // static characters save a bit
  char* slash = "/"; // of memory

  // get time
  //Clock = RTC.now(); // get the RTC time
  theTime = calculateTime(); // takes into account DST

  secondval = theTime.second();  // get seconds
  minuteval = theTime.minute();  // get minutes
  hourval = theTime.hour();   // get hours
  //if (hourval > 11) hourval -= 12;
  hourval = hourval % 12; // This clock is 12 hour, if 13-23, convert to 0-11`

  hourval = (hourval * 60 + minuteval) / 12; //each red dot represent 24 minutes.

  // arc mode
  for (uint8_t i = 0; i < strip.numPixels(); i++) {

    if (i <= secondval) {
      // calculates a faded arc from low to maximum brightness
      pixelColorBlue = (i + 1) * (255 / (secondval + 1));
      //pixelColorBlue = 255;
    }
    else {
      pixelColorBlue = 0;
    }

    if (i <= minuteval) {
      pixelColorGreen = (i + 1) * (255 / (minuteval + 1));
      //pixelColorGreen = 255;
    }
    else {
      pixelColorGreen = 0;
    }

    if (i <= hourval) {
      pixelColorRed = (i + 1) * (255 / (hourval + 1));
      //pixelColorRed = 255;
    }
    else {
      pixelColorRed = 0;
    }

    strip.setPixelColor((i + STARTPIXEL) % 60, strip.Color(pixelColorRed, pixelColorGreen, pixelColorBlue));
  }

  /*
    // for serial debugging
    Serial.print(hourval, DEC);
    Serial.print(':');
    Serial.print(minuteval, DEC);
    Serial.print(':');
    Serial.println(secondval, DEC);
  */

  //display
  strip.show();

  // wait
  delay(100);

}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    //strip.setPixelColor(i, c);
    strip.setPixelColor((i + STARTPIXEL) % 60, c);
    strip.show();
    delay(wait);
  }
}
