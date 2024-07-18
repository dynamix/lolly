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
// #include <Adafruit_SH110X.h>
#include <math.h>
#include <RCSwitch.h>
#include <vector>

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

// Display
#define SCREEN_I2C_ADDRESS 0x3C // I2C address of the Amazon OLED display, even though it says 0x78 on the back
#define SCREEN_WIDTH 128        // OLED display width, in pixels
#define SCREEN_HEIGHT 64        // OLED display height, in pixels
#define OLED_RESET -1           //   QT-PY / XIAO
// Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

// button board
Adafruit_NeoTrellis pad(NEO_TRELLIS_ADDR, &Wire1);
#define PAD_CONNECTED false

// active or not active for the pad buttons
uint8_t buttonState[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t buttonStateful[16] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t buttonDebounce[16] = {0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

#define NUM_LEDS 512
#define MAX_BRIGHTNESS 25
// #define MAX_BRIGHTNESS 150
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
// uint8_t currentBrightness = MAX_BRIGHTNESS;
uint8_t currentBrightness = 25;
uint16_t currentDelay = 0; // delay between frames
uint8_t shouldClear = 1;   // clear all leds between frames
uint8_t shouldShow = 1;
uint8_t potentiometerControlsBrightness = 1; // use potentiometer to get brightness value for next frame
uint8_t scheduleStrobo = 0;

// Debug stuff to count LEDS properly
static int globalP = 0;

int accelTestMV = 0;

elapsedMillis fps0;

// base class to create some effects
class Effect
{
public:
  virtual void setup(void) {};
  virtual void draw(void) {};
};

std::vector<Effect *> effects;

#include "inc/util.cpp"

// map from linear leds to the actual physical led order
// _leds is the array that fastled uses for physical signals
void remap()
{
  // return;
  for (int i = 0; i < NUM_LEDS; i++)
  {
    _leds[ledsOnPCBMap[i]] = leds[i];
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

uint16_t BNO055_SAMPLERATE_DELAY_MS = 10;
// velocity = accel*dt (dt in seconds)
// position = 0.5*accel*dt^2
double ACCEL_VEL_TRANSITION = (double)(BNO055_SAMPLERATE_DELAY_MS) / 1000.0;
double ACCEL_POS_TRANSITION = 0.5 * ACCEL_VEL_TRANSITION * ACCEL_VEL_TRANSITION;
double DEG_2_RAD = 0.01745329251; // trig functions require radians, BNO055 outputs degrees

// trait to add accelerometer data to an effect
class AccelTrait
{
private:
  elapsedMillis t;

public:
  double xPos = 0;
  double yPos = 0;
  double headingVel = 0;
  sensors_event_t orientationData, linearAccelData, accelData;

  void updateSenorData()
  {
    // - VECTOR_ACCELEROMETER - m/s^2
    // - VECTOR_MAGNETOMETER  - uT
    // - VECTOR_GYROSCOPE     - rad/s
    // - VECTOR_EULER         - degrees
    // - VECTOR_LINEARACCEL   - m/s^2
    // - VECTOR_GRAVITY       - m/s^2
    if (this->t < BNO055_SAMPLERATE_DELAY_MS)
      return;
    this->t = 0;

    bno.getEvent(&this->orientationData, Adafruit_BNO055::VECTOR_EULER);
    //  bno.getEvent(&angVelData, Adafruit_BNO055::VECTOR_GYROSCOPE);
    bno.getEvent(&this->linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
    bno.getEvent(&this->accelData, Adafruit_BNO055::VECTOR_ACCELEROMETER);

    this->xPos = this->xPos + ACCEL_POS_TRANSITION * this->linearAccelData.acceleration.x;
    this->yPos = this->yPos + ACCEL_POS_TRANSITION * this->linearAccelData.acceleration.y;

    // velocity of sensor in the direction it's facing
    this->headingVel = ACCEL_VEL_TRANSITION * this->linearAccelData.acceleration.x / cos(DEG_2_RAD * this->orientationData.orientation.x);
  }
};

void printBNOCalibrationStatus()
{
  uint8_t sensorStatus[4] = {0, 0, 0, 0};
  bno.getCalibration(&sensorStatus[0], &sensorStatus[1], &sensorStatus[2], &sensorStatus[3]);
  if (!sensorStatus[0])
  {
    Serial.print("SYSTEM CALIBRATION NOT READY");
  }
  Serial.print("Sys:");
  Serial.print(sensorStatus[0]);
  Serial.print(" G:");
  Serial.print(sensorStatus[1]);
  Serial.print(" A:");
  Serial.print(sensorStatus[2]);
  Serial.print(" M:");
  Serial.println(sensorStatus[3]);
}

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

class VariableArmsDemo : public Effect
{
  void draw(void) override
  {
    static uint8_t hueOffset = 0;
    static uint8_t numArms = 1;
    EVERY_N_SECONDS(1) { numArms++; }
    for (int i = 0; i < NUM_LEDS; i++)
    {
      uint16_t armIndex = i % numArms;
      uint8_t hue = (map(armIndex, 0, numArms, 0, 255) + hueOffset) % 255;
      leds[ledsAsSpiralMap[i]] = CHSV(hue, 255, 255);
    }
  }
};

// uses the planar map instead of x and y coordinates of every pixel
class PlanarDemo : public Effect
{
  void draw(void) override
  {
    for (int i = 0; i < PLANAR_COLS; i++)
    {
      for (int j = 0; j < PLANAR_ROWS; j++)
      {
        int idx = planarMap[i * PLANAR_COLS + j];
        if (idx == 1000)
          continue;
        leds[idx] = CHSV(i * 20, 255, 255);
      }
    }
  }
};

// uses the polarMap instead of distance and angle of every pixel
class PolarDemo : public Effect
{
  void draw(void) override
  {
    uint16_t a = millis() / 7;
    static int n = 0;

    for (int j = 0; j < POLAR_ROWS; j++)
    {
      for (int i = 0; i < POLAR_COLS; i++)
      {
        uint16_t idx = polarMap[POLAR_COLS * j + i];
        if (idx == 1000)
          continue;
        leds[idx].setHue(i * 54 + (a >> 2) + (sin8(j * 16 + a)) >> 1);
      }
    }
  }
};

class PolarCaleidoscope : public Effect
{
  void draw(void) override
  {
    uint16_t a = millis() / 7;
    static int n = 0;
    for (int j = 0; j < POLAR_ROWS; j++)
    {
      for (int i = 0; i < POLAR_COLS; i++)
      {
        uint16_t idx = polarMap[POLAR_COLS * j + i];

        if (idx == 1000)
          continue;

        byte r = (sin8(i * 16 + a) + cos8(j * 16 + a / 2)) / 2;
        byte g = sin8(j * 16 + a / 2 + sin8(leds[idx].r + a) / 16);
        byte b = cos8(i * 16 + j * 16 - a / 2 + leds[idx].g);

        leds[idx].setRGB(r, g, b);
      }
    }
  }
};

class PolarFireFly : public Effect
{
  void draw(void) override
  {
    uint16_t a = millis() / 3;
    static int n = 0;
    // EVERY_N_MILLIS(259) { n++; n = n%POLAR_COLS;}

    for (int j = 0; j < POLAR_ROWS; j++)
    {
      for (int i = 0; i < POLAR_COLS; i++)
      {
        uint16_t idx = polarMap[POLAR_COLS * j + i];

        int jj = POLAR_ROWS - j;

        if (idx == 1000)
          continue;
        leds[idx] = HeatColor(qsub8(inoise8(i * 60 + a, jj * 5 + a, a / 3), abs8(jj - (POLAR_ROWS - 1)) * 255 / (POLAR_ROWS + 2)));

        // leds[idx] = CHSV(i*4,200,200);
      }
    }
  }
};
class FTest : public Effect
{
public:
  void draw()
  {
    static uint8_t x = 0;
    x++;
    // fill_rainbow(leds, NUM_LEDS, x, 1);
    fill_solid(_leds, 128, CRGB::Green);
    fill_solid(&_leds[128], 128, CRGB::Red);
    fill_solid(&_leds[128 + 128], 127, CRGB::Blue);
    fill_solid(&_leds[128 + 128 + 127], 129, CRGB::Yellow);
  }
};
class MapTest : public Effect
{
public:
  void draw()
  {
    static uint16_t x = 0;

    leds[x] = CRGB::Red;
    x++;
    x = x % NUM_LEDS;
    delay(500);
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

class AccelTest : public Effect, public AccelTrait
{
public:
  void setup()
  {
    shouldClear = false;
  }

  void draw()
  {
    // fadeToBlackBy(leds, NUM_LEDS, 13);
    this->updateSenorData();
    EVERY_N_MILLISECONDS(250)
    {
      Serial.print("Heading: ");
      Serial.println(this->orientationData.orientation.x);
      // Serial.print("Position: ");
      // Serial.print(xPos);
      // Serial.print(" , ");
      // Serial.println(yPos);
      // Serial.print("Speed: ");
      // Serial.println(headingVel);
      Serial.print("linearAccel");
      Serial.print(" x:");
      Serial.print(this->linearAccelData.acceleration.x);
      Serial.print(" y:");
      Serial.print(this->linearAccelData.acceleration.y);
      Serial.print(" z:");
      Serial.println(this->linearAccelData.acceleration.z);
      // Serial.println("accel");
      // this->printEvent(&accelData);
      // Serial.println("-------");
    }
  }
};

class AudioTest : public Effect
{

  elapsedMillis fps;
  void draw() override
  {
    if (this->fps > 24)
    {
      if (peak1.available())
      {
        this->fps = 0;
        int monoPeak = peak1.read() * 30.0;
        Serial.print("|");
        for (int cnt = 0; cnt < monoPeak; cnt++)
        {
          Serial.print(">");
        }
        Serial.println();
      }
    }
  }
};
// class GifTest : public Effect
// {
//   void draw(void) override
//   {

//     CRGB *pal = (CRGB *)status->palette;
//     uint8_t *pixels = status->hsd.buffers + HEATSHRINK_STATIC_INPUT_BUFFER_SIZE;
//     // plot each sprite pixel modulo the size of the matrix, i.e. wrap the image
//     for (int i = 0; i < PLANAR_COLS; i++)
//     {
//       for (int j = 0; j < PLANAR_ROWS; j++)
//       {
//         int idx = planarMap[i * PLANAR_COLS + j];
//         if (idx == 1000)
//           continue;

//         uint8_t palette_index = *pixels++;
//         if (palette_index > 0 && cfg.effect != 1)
//         {
//           // non-0 palette entry;  copy the palette entry to the LED
//           leds[idx] = pal[palette_index];
//         }
//         else
//         {
//           // palette entry 0 is the mask;  fade this LED to the mask colour (black usually)
//           fadeTowardColour(leds[idx], pal[palette_index], fade_speed);
//         }
//       }
//     }
//   }
// };

// the setup routine runs once when you press reset:
void setup()
{
  // add all effects
  effects = {
      new PolarFireFly(),
      new MapTest(),
      new PolarTest(),
      new FTest(),
      new PolarDemo(),
      new AudioTest(),
      new AccelTest(),
      new ColorWheelWithSparkels(),
  };

  while (!Serial && (millis() <= 2000))
    ; // Wait for Serial interface
  Serial.begin(115200);

  Serial.println("boot lolly");

  Serial.println("LED init");

  // 7 no, 6 no, 5 no

  // 2,14,7,8,6, 20,21,5 (last three are unused now)
  // 7,8,6, 5 (last three are unused now)
  // no GRB, RBG, GBR,
  FastLED.addLeds<APA102, 2, 14, BGR>(_leds, 128);       // go
  FastLED.addLeds<APA102, 20, 14, BGR>(_leds, 128, 128); // go
  FastLED.addLeds<APA102, 21, 14, BGR>(_leds, 128 + 128, 127);
  FastLED.addLeds<APA102, 26, 14, BGR>(_leds, 128 + 128 + 127, 129);

  //  FastLED.addLeds<APA102, 8, 14>(_leds, 128 + 128, 127);
  //  FastLED.addLeds<APA102, 20, 14>(_leds, 128 + 128 + 127, 129);

  // FastLED.addLeds<APA102, 5, 14>(_leds, 128 + 128, 127);

  // FastLED.addLeds<APA102, 7, 8>(_leds, 128, 128);
  // FastLED.addLeds<APA102, 6, 20>(_leds, 128 + 128, 127);
  // FastLED.addLeds<APA102, 21, 5>(_leds, 128 + 128 + 127, 129);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
  // FastLED.addLeds<APA102, 2, 6>(_leds, 128);
  // FastLED.addLeds<APA102, 14, 6>(_leds, 128, 128);
  // FastLED.addLeds<APA102, 7, 6>(_leds, 128 + 128, 127);
  // FastLED.addLeds<APA102, 8, 6>(_leds, 128 + 128 + 127, 129);
  // FastLED.setMaxPowerInVoltsAndMilliamps(5, 5000);
  FastLED.setBrightness(MAX_BRIGHTNESS);

  // init radio!
  Serial.println("Radio init");
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

    printBNOCalibrationStatus();
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

  // digitalWrite(2, HIGH);
  // digitalWrite(26, HIGH); // was 5
  // digitalWrite(6, HIGH);
  // digitalWrite(7, HIGH);
  // digitalWrite(8, HIGH);
  // digitalWrite(14, HIGH);
  // digitalWrite(20, HIGH);
  // digitalWrite(21, HIGH);

  // return;

  // EVERY_N_MILLIS(2000) { printBNOCalibrationStatus(); }

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
