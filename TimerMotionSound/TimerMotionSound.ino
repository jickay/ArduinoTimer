#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    24
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          75
#define FRAMES_PER_SECOND   60

//const int buttonPin = 3;     // the number of the pushbutton pin
int buttonState = 0;

const int motionPin = 6;
int motionState = 0;

const int buzzerPin = 8;

void setup() {
//  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  Serial.begin(115200);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
//typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
//SimplePatternList gPatterns = { count };


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

unsigned long changemillis;
unsigned long totalTime; // in millis
unsigned long lastClickTime;
unsigned long ledLength = 2000; // Time amount for each LED in ms

int clickCount = 0;
int countMax = 24;

unsigned long inputDelay = 500;
unsigned long dimDelay = ledLength/255;

unsigned long baseLEDBrightness = 200;
unsigned long lastLEDValue = baseLEDBrightness;

bool motionOn = true;

unsigned long sum = 0;
unsigned long lastNoise = 0;
unsigned long threshold = 750;
const int buttonPin = A6;

void soundLoop() {
  sum = 0;
  int shift = 5;
  int sampleSize = pow(2,shift);
  for(int i=0; i<sampleSize; i++)
  {
    unsigned long signal = analogRead(buttonPin);
//    Serial.println(signal);
 //   int threshold = 900;
//    if (signal > threshold) {
//      signal = 0;
//    }
//      if (signal < threshold) {
//        signal = signal * 10;
//      }
////     Serial.println(signal);
      sum += signal;
  }
  sum >>= shift;
  lastNoise = sum;
  Serial.println(sum);
  delay(10);
}

bool prevIsHigh = false;
void loop()
{
//  buttonState = digitalRead(buttonPin);
  motionState = digitalRead(motionPin);

//  Serial.println(buttonState);

  soundLoop();
  
  if ((motionState == HIGH && prevIsHigh) || (lastNoise < threshold)) {
   
//    Serial.println("Motion detected!");
//    Serial.println(motionState);
    // If click outside of input delay length light up new LED
  if (millis() > lastClickTime + inputDelay) {
        if (clickCount <= NUM_LEDS) {
            clickCount++;
//            Serial.println(clickCount);
        }
        gHue = clickCount * 6;

        lastLEDValue = baseLEDBrightness;
        leds[clickCount] += CHSV( gHue, 255, lastLEDValue);
        lastClickTime = millis();
        totalTime += ledLength;
         
    }
  }
  
  if (millis() > lastClickTime + inputDelay) {
      motionOn = true;
  }

  // Dimming method using hard value
  // NEED TO HAVE PROPORTIONAL VALUE TO TIME PER LED !!!!!!!!!!!!!!!!!!!!
  if (millis() > lastClickTime + dimDelay) {
      if (lastLEDValue >= 0) {
          lastLEDValue -= 0.5;
      } else {
          lastLEDValue = 0;
      }
      leds[clickCount] = CHSV( gHue, 255, lastLEDValue);
//      Serial.println(lastLEDValue);
  }

  // If set time for each LED passes turn off last LED and update values
  unsigned long now = millis();
  if (now - lastClickTime > ledLength) {
    leds[clickCount] = CHSV(gHue,0,0);
    totalTime -= ledLength;
    lastClickTime = millis();
    if (clickCount >= 0) {
          clickCount--;
  //        Serial.println("Countdown:");
    //      Serial.println(clickCount);
          if (clickCount < 0) {
   //         Serial.println("Buzz!");
            tone(buzzerPin, 500, 500);
            delay(2000);
          }
    }
    lastLEDValue = baseLEDBrightness;
//    Serial.println("End of loop");
  }

  if (motionState == HIGH) {
    prevIsHigh = true;
  } else {
    prevIsHigh = false;
  }
  
  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
}
