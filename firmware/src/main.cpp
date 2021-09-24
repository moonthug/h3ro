#include "FastLED.h"
#include "Adafruit_FreeTouch.h"

#define DATA_PIN 2
#define TOUCH_PIN A6
#define LED_COUNT 8
#define COLOUR_COUNT 8
#define FRAME_MS 25
#define ANIMATION_OFF_MS 10000
#define MIN_TOUCH_VALUE 600

Adafruit_FreeTouch touch = Adafruit_FreeTouch(A6, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

enum state {
  HERO,
  PAUSE,
  TOUCH
};
state currentState;

CRGB leds[LED_COUNT];
static uint8_t lightId;
static long lastMillis;
static long currentMillis;
static long inc;
static uint8_t noise;

static uint8_t colour;
CRGB colours[COLOUR_COUNT] = {
    CRGB(255, 20, 20),  // red (corrected)
    CRGB(0x267DDB),     // blue
    CRGB(0xFCB09C),     // st2
    CRGB(0, 255, 50),   // st3 (corrected)
    CRGB(0xFCA642),     // st4
    CRGB(0xFACCF5),     // st6
    CRGB(0, 200, 100),  // st7 (corrected)
    CRGB::Black
};

void setup() {
  currentState = HERO;

  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, LED_COUNT);
  FastLED.setCorrection(TypicalPixelString);
  //FastLED.setBrightness(50);

  if (!touch.begin()) {
    Serial.println("Failed to begin qt");
  }

  Serial.println("Hi, I'm Alex. Thank you for taking the time to plug me into your serial port. I was once a HERO, but now I am not. Please don't hate my code :D");
}

void reset() {
  inc = 0;
}

void drawHero() {
  if (currentMillis - lastMillis > FRAME_MS) {
    colour = (inc / LED_COUNT) % COLOUR_COUNT;
    lightId = inc % LED_COUNT;

    leds[lightId] = colours[colour];
    inc++;

    lastMillis = currentMillis;
  }

  if (inc == LED_COUNT * COLOUR_COUNT) {
    inc = 0;
    currentState = PAUSE;
  }
}

void drawTouch() {
  for(int i = 0; i < LED_COUNT; i++) {
    noise = inoise16((currentMillis - i * 100) / 2);
    leds[i] = CHSV(noise, 255, 255);
  }
}

void drawPause() {
  fill_solid(leds, LED_COUNT, CRGB::Black);

  if (currentMillis - lastMillis > ANIMATION_OFF_MS) {
    currentState = HERO;
    reset();
  }
}

void loop() {
  // For animation
  currentMillis = millis();

  // Handle qtouch
  int touchValue = touch.measure();
  if (touchValue > MIN_TOUCH_VALUE) {
    currentState = TOUCH;
  } else {
    if (currentState == TOUCH) {
      currentState = PAUSE;
      reset();
    }
  }

  // State machine
  if (currentState == HERO) {
    drawHero();
  } else if(currentState == TOUCH) {
    drawTouch();
  } else {
    drawPause();
  }

  FastLED.show();
}