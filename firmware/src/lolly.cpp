// #define FASTLED_ALLOW_INTERRUPTS 0

#include <Arduino.h>
#include <Audio.h>
#include <ADC.h>
#include <SoftwareSerial.h>
#include <FastLED.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoTrellis.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
#include <RCSwitch.h>
#include <vector>

float FindE(int bands, int bins);

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define POTENTIOMETER_PIN A17
#define MIC_PIN A2
#define REMOTE_RECEIVER_PIN 24

// analog - digital
ADC *adc = new ADC();

// remote codes
#define REMOTE_BUTTON_A 240191
#define REMOTE_BUTTON_B 4382
#define REMOTE_BUTTON_C 4242
#define REMOTE_BUTTON_D 987654

// remote switch 433 Mhz
RCSwitch remoteSwitch = RCSwitch();

// motion sensor
Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x29, &Wire2);

// button board
Adafruit_NeoTrellis pad(NEO_TRELLIS_ADDR, &Wire1);
#define PAD_CONNECTED false

// active or not active for the pad buttons
uint8_t buttonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t buttonStateful[16] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t buttonDebounce[16] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

// 3W pixels
// SoftwareSerial pixieSerial(-1, 26);
// Adafruit_Pixie pixie = Adafruit_Pixie(1, &Serial1);

#define NUM_LEDS 512

#define MAX_BRIGHTNESS 150
#define TEENSY_LED 13

// 2,14,7,8  ,6,20,21,5
CRGB _leds[NUM_LEDS]; // actual rendered leds - filled by remap
CRGB leds[NUM_LEDS];  // leds to work with while codeing effects

AudioInputAnalog adc1(MIC_PIN);
AudioAnalyzePeak peak1;
AudioConnection patchCord1(adc1, peak1);

#include "segment.h"
#include "maps.h"

typedef void (*Mode[2])(void);

// global state
int8_t currentMode = 0;
int8_t previousMode = 0;
int8_t nextScheduledMode = -1;
int8_t nextScheduledModeDir = 0;
uint8_t currentBrightness = MAX_BRIGHTNESS;
uint16_t currentDelay = 0; // delay between frames
uint8_t shouldClear = 1;   // clear all leds between frames
uint8_t shouldShow = 1;
uint8_t usePixies = 0;                       // pixies on
uint8_t potentiometerControlsBrightness = 1; // use potentiometer to get brightness value for next frame
uint8_t useFibers = 1;
uint8_t scheduleStrobo = 0;
// Debug stuff to count LEDS properly
static int globalP = 0;

int accelTestMV = 0;

elapsedMillis fps0;

class Effect
{
public:
  virtual void setup(void) {};
  virtual void draw(void) {};
};

std::vector<Effect *> effects;

#include "inc/util.cpp"

void remap()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    _leds[i] = leds[ledsOnPCBMap[i]];
  }
}

TrellisCallback readButton(keyEvent evt)
{
  Serial.print("BUTTON: ");
  Serial.print(evt.bit.NUM);

  if (buttonStateful[evt.bit.NUM])
  {
    Serial.print(" OLDSTATE=");
    Serial.print(buttonState[evt.bit.NUM]);
    if (buttonState[evt.bit.NUM] && evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING && millis() > buttonDebounce[evt.bit.NUM] + 1000)
    {
      buttonState[evt.bit.NUM] = 0;
      pad.pixels.setPixelColor(evt.bit.NUM, 0);
      buttonDebounce[evt.bit.NUM] = millis();
    }
    else if (!buttonState[evt.bit.NUM] && evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING && millis() > buttonDebounce[evt.bit.NUM] + 1000)
    {
      buttonState[evt.bit.NUM] = 1;
      CRGB c = CHSV(map(evt.bit.NUM, 0, pad.pixels.numPixels(), 0, 255), 240, 240);
      pad.pixels.setPixelColor(evt.bit.NUM, pad.pixels.Color(c.r, c.g, c.b));
      buttonDebounce[evt.bit.NUM] = millis();
    }
    Serial.print(" NEWSTATE=");
    Serial.print(buttonState[evt.bit.NUM]);
  }
  else
  {
    if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    {
      CRGB c = CHSV(map(evt.bit.NUM, 0, pad.pixels.numPixels(), 0, 255), 240, 240);
      pad.pixels.setPixelColor(evt.bit.NUM, pad.pixels.Color(c.r, c.g, c.b));

      switch (evt.bit.NUM)
      {
        // new FiberAndFishsEffect(), // 0
        // new Fibercrazy(), // 1
        // new Heartbeat(), // 2
        // new AccelTest(), // 3
        // new NewAudio(), // 4
        // new JellyAudio(), // 5
        // new JellyColorEffect(), // 6
        // new RunnersEffect(), // 7
        // new ColorWheelNew(), // 8
        // new ColorWheelWithSparkels(), // 9
        // new Streaming(), // 10
        // new Rings(), // 11
        // new Juggle(), // 12
        // // new DropEffect(),
        // new Bouncy(), // 13
        // new PrideEffect(), // 14
        // new IceSparkEffect(), // 15
        // new SinelonEffect(), // 16
        // new PixelFiringEffect(), // 17
        // // new BlurEffect(),
        // new PaletteTestEffect()}; // 18

      case 0:
        nextScheduledMode = 0;
        break;
      case 1:
        nextScheduledMode = 2;
        break;
      case 2:
        nextScheduledMode = 4;
        break;
      case 4:
        nextScheduledMode = 8;
        break;
      case 5:
        nextScheduledMode = 10;
        break;
      case 6:
        nextScheduledMode = 12;
        break;
      case 8:
        nextScheduledMode = 13;
        break;
      case 9:
        nextScheduledMode = 14;
        break;
      case 10:
        nextScheduledMode = 15;
        break;
      case 12:
        nextScheduledMode = 16;
        break;
      case 13:
        nextScheduledMode = 18;
        break;
      case 14:
        scheduleStrobo = 1;
        break;
      case 11:
        nextScheduledModeDir = -1;
        break;
      case 15:
        nextScheduledModeDir = 1;
        break;
      }
    }
    else if (evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
    {
      pad.pixels.setPixelColor(evt.bit.NUM, 0);
    }
  }
  Serial.println("");
  pad.pixels.show();
  return 0;
}

void checkSerial()
{
  if (Serial.available() > 0)
  {
    char c = Serial.read();
    if (c == 'n')
      nextMode(1);
    else if (c == 'b')
      nextMode(-1);
    else
    {
      globalP++;
      Serial.print("POS:");
      Serial.println(globalP);
    }
  }
}

static uint16_t potentiometer = 0;

void checkPotentiometer()
{
  potentiometer = adc->analogRead(POTENTIOMETER_PIN, ADC_1);
  // Serial.print("POT: ");
  // Serial.println(potentiometer);
  // Serial.println(potentiometer * 3.3 / adc->getMaxValue(ADC_1), 2);
  // Serial.print("  ");
  if (potentiometerControlsBrightness)
  {
    LEDS.setBrightness(map(potentiometer, 0, 65535, 0, 255));
  }
}

void checkButtons()
{
  if (PAD_CONNECTED)
    pad.read();
}

void readRemoteSwitch()
{
  if (remoteSwitch.available())
  {

    int value = remoteSwitch.getReceivedValue();
    if (value != 0)
    {
      // nextMode(1);
      Serial.print("Received ");
      Serial.print(remoteSwitch.getReceivedValue());
      Serial.print(" / delay ");
      Serial.print(remoteSwitch.getReceivedDelay());

      static int codes[4] = {REMOTE_BUTTON_A, REMOTE_BUTTON_B, REMOTE_BUTTON_C, REMOTE_BUTTON_D};
      // for (int i = 0; i < 4; i++)
      // {
      //   if (codes[i] == value && PAD_CONNECTED)
      //   {
      //     pad.pixels.setPixelColor(12 + i, random8(), random8(), random8());
      //     pad.pixels.show();
      //   }
      // }

      // if (value == REMOTE_BUTTON_B)
      //   globalP++;
      if (value == REMOTE_BUTTON_A || value == REMOTE_BUTTON_B)
        nextMode(1);
      if (value == REMOTE_BUTTON_C || value == REMOTE_BUTTON_D)
        scheduleStrobo = 2;
    }
    remoteSwitch.resetAvailable();
  }
}

void runScheduluer()
{
  if (nextScheduledMode != -1)
  {
    setMode(nextScheduledMode);
    nextScheduledMode = -1;
  }
  if (nextScheduledModeDir)
  {
    nextMode(nextScheduledModeDir);
    nextScheduledModeDir = 0;
  }
}

uint16_t fps = 0;

void showFps()
{
  Serial.println(fps);
  fps = 0;
}

// 2d coordiante system effects similar to PixelBlaze, just derive an effect
class Effect2D : public Effect
{
public:
  virtual void setup(void) {};
  void draw(void) override
  {
    this->beforeRender();
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = this->render(i, ledsXYMap[i][0], ledsXYMap[i][0]);
    }
  };
  virtual void beforeRender(void) {};
  virtual CRGB render(int idx, float x, float y) { return CRGB::Black; }
};

// instead of 2d cartesian this is a polar coordinate system
class EffectPolar : public Effect
{
public:
  virtual void setup(void) {};
  void draw(void) override
  {
    this->beforeRender();
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = this->render(i, ledsPolarMap[i][0], ledsPolarMap[i][0]);
    }
  };
  virtual void beforeRender(void) {};
  virtual CRGB render(int idx, float r, float deg) { return CRGB::Black; }
};

// Simple test for the polar coordiante rendering
class PolarTest : public EffectPolar
{
  float r = 0;
  uint8_t hue = 0;
  void beforeRender() override
  {
    this->r = beatsin8(20, 0, 255) / 255.0;
    this->hue++;
  }
  CRGB render(int idx, float r, float deg) override
  {
    float delta = abs(r - this->r);
    if (delta < 0.2)
      return CHSV(this->hue, 200 - (0.2 - delta) * 600, 200);
    return CRGB::Black;
  }
};

// This reproduces the very first test with changing the number of arms every 2 seconds
// the first test was here: https://editor.p5js.org/dynamix/sketches/gNS2GeqVQ
class VariableArmsDemo : public Effect
{
  void draw(void) override
  {
    static uint8_t hueOffset = 0;
    static uint8_t numArms = 1;
    EVERY_N_SECONDS(2) { numArms++; }
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint16_t armIndex = i % numArms;
      uint8_t hue = (map(armIndex, 0, numArms, 0, 255) + hueOffset) % 255;
      leds[ledsAsSpiralMap[i]] = CHSV(hue, 255, 255);
    }
  }
};

// class Kaleidoscope : public Effect
// {
//   elapsedMillis t;
//   void draw(void) override
//   {

//     uint16_t a = t / 8;

//     for (int j = 0; j < NUM_ROWS_CILINDR; j++)
//     {
//       for (int i = 0; i < NUM_COLS_CILINDR; i++)
//       {
//         uint16_t index = XY_CILINDR(i, j);
//         if (index == lastSafeIndex)
//           continue;

//         byte valueR = (sin8(i * 16 + a) + cos8(j * 16 + a / 2)) / 2;
//         byte valueG = sin8(j * 16 + a / 2 + sin8(leds[index].r + a) / 16);
//         byte valueB = cos8(i * 16 + j * 16 - a / 2 + leds[index].g);

//         leds[index].setRGB(valueR, valueG, valueB);
//       }
//     }
//   }

static uint8_t hue = 0;
hue++;
for (int i = 0; i < NUM_LEDS; i++)
{
  leds[i] = CHSV(hue + i * 2, 255, 255);
}
}
}
;

class FTest : public Effect
{
public:
  void draw()
  {
    static uint8_t x = 0;
    x++;
    fill_rainbow(leds, NUM_LEDS, x, 1);
  }
};

class ColorWheelWithSparkels : public Effect
{
public:
  void setup()
  {
    currentDelay = 0;
    shouldClear = false;
  }
  void draw()
  {
    static elapsedMillis timer;
    if (timer < 20)
      return;
    timer = 0;

    static uint8_t hue = 0;
    hue++;

    hue++;
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CHSV(5 * i + hue, 240, 150);
    }
    for (int j = 0; j < 3; j++)
    {
      int x = random(0, NUM_LEDS);
      leds[x] = CRGB::White;
    }
  }
};

// the setup routine runs once when you press reset:
void setup()
{

  // add all effects
  effects = {
      new FTest(),
      new ColorWheelWithSparkels(),
  };

  while (!Serial && (millis() <= 2000))
    ; // Wait for Serial interface
  Serial.begin(115200);

  Serial.println("boot lolly");

  // init 3w leds serial connection
  // Serial.println("3W LED init");
  // Serial1.setTX(26);
  // Serial1.begin(115200);
  // FastLED.addLeds<PIXIE, 26>(powerLeds, 2);

  Serial.println("LED12345 init");

  // 2,14,7,8,6, 20,21,5 (last three are unused now)
  // FastLED.addLeds<APA102>(leds, 1);
  FastLED.addLeds<APA102, 2, 6>(_leds, 128);
  FastLED.addLeds<APA102, 14, 6>(_leds, 128, 128);
  FastLED.addLeds<APA102, 7, 6>(_leds, 128 + 128, 127);
  FastLED.addLeds<APA102, 8, 6>(_leds, 128 + 128 + 127, 129);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
  FastLED.setBrightness(MAX_BRIGHTNESS);

  Serial.println("Radio init");
  // init radio!
  pinMode(REMOTE_RECEIVER_PIN, INPUT);
  remoteSwitch.enableReceive(REMOTE_RECEIVER_PIN);

  if (PAD_CONNECTED)
  {
    Serial.println("pad init");
    // init button pads and flash them once
    if (!pad.begin())
    {
      Serial.println("pad init failed!");
    }
    else
    {
      for (uint16_t i = 0; i < pad.pixels.numPixels(); i++)
      {

        pad.activateKey(i, SEESAW_KEYPAD_EDGE_RISING);
        pad.activateKey(i, SEESAW_KEYPAD_EDGE_FALLING);
        pad.registerCallback(i, readButton);
      }
      for (uint16_t i = 0; i < pad.pixels.numPixels(); i++)
      {
        CRGB c = CHSV(i * 20, 240, 240);
        pad.pixels.setPixelColor(i, c.r, c.g, c.b);
        pad.pixels.show();
        delay(25);
      }
      for (uint16_t i = 0; i < pad.pixels.numPixels(); i++)
      {
        pad.pixels.setPixelColor(i, 0x000000);
        pad.pixels.show();
        delay(25);
      }
    }
  }
  else
  {
    Serial.println("pad disabled!");
  }

  Serial.println("motion init");

  // Initialise the motion sensor
  if (!bno.begin())
  {
    Serial.print("motion sensor error");
  }
  else
  {
    Serial.println("motion sensor OK");
    delay(200);
    int8_t temp = bno.getTemp();
    Serial.print("temperature: ");
    Serial.print(temp);
    Serial.println(" C");
    bno.setExtCrystalUse(true);
  }

  AudioMemory(8);

  pinMode(TEENSY_LED, OUTPUT);
  pinMode(POTENTIOMETER_PIN, INPUT);

  // adc->setAveraging(16, ADC_1);
  // adc->setResolution(16, ADC_1);
  // adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED, ADC_1);
  // adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1);
  // // adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0);
  // adc->setReference(ADC_REFERENCE::REF_3V3, ADC_1); // doesn't work but we hacked the audio library and core to still  work
  // adc->adc1->recalibrate();

  delay(10);

  // start with mode number 0
  // modes[0][1]();
  effects[0]->setup();
}

void loop()
{
  runScheduluer();

  if (shouldClear)
    clear();

  effects[currentMode]->draw();

  if (shouldShow)
  {
    remap();
    FastLED.show();
  }

  fps++;

  // if (PAD_CONNECTED)
  // {
  //   EVERY_N_MILLISECONDS(100) { checkButtons(); }
  // }
  // EVERY_N_MILLISECONDS(250) { checkPotentiometer(); }
  // EVERY_N_MILLISECONDS(50) { readRemoteSwitch(); }

  EVERY_N_MILLISECONDS(1000) { testled(); }
  EVERY_N_MILLISECONDS(100) { checkSerial(); }
  EVERY_N_MILLISECONDS(1000) { showFps(); }

  if (currentDelay > 0)
    delay(currentDelay);
}
