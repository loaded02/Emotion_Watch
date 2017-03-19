
/*  Emotion Watch 
 Emotion Tracker by Moritz Hilberg
 based on Mediation Trainer by Collin Cunningham and
 based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman http://www.pulsesensor.com and
 based on Arduino and Galvanic Skin Response (GSR) Sensor by hypnotistas

 Uses a Circuit Playground board's built-in LEDs to diplay mood according to sensor input

 Hardware requirements:
 - Adafruit Circuit Playground https://www.adafruit.com/products/3000
 - Pulse Sensor Amped https://www.adafruit.com/products/1093
 - Grove GSR Sensor https://www.seeedstudio.com/Grove-GSR-sensor-p-1614.html

 Wiring:
 - Pulse Sensor Black wire to Circuit Playground GND
 - Pulse Sensor Red wire to Circuit Playground VBATT
 - Pulse Sensor Purple wire to Circuit Playground #9
 - GSR Sensor Black wire to Circuit Playground GND
 - GSR Sensor Red wire to Circuit Playground 3.3V
 - GSR Sensor Yellow wire to Circuit Playground #10

 Mood Control:
 The left PushButton (#4) is used to calculate a personal baseline. Start and Stop of calculation are indicated by a sound.
 Neopixel LEDs 0 and 9 on the Circuit Playground are used to indicate the current mood. Color-Schema:
 - TBD

 Pulse: 
 Neopixel LEDs 1 and 8 on the Circuit Playground flash with each pulse detected and their color indicates a basic HRV "coherence" rating which roughly translates to the user's level of relaxation:
 - red = low relaxation
 - blue = moderate relaxation
 - green = high relaxation
 
*/

#include <Adafruit_CircuitPlayground.h>

/*define block*/
#define BASELINE_AVERAGE_TIME 60000 // interval for baseline calculation

#define EMO_POS_4             90  // signals that are higher than baseline. Value is Percent of interval max - baseline
#define EMO_POS_3             75
#define EMO_POS_2             50
#define EMO_POS_1             25
#define EMO_NEG_2             15  // signals that are lower than baseline.
#define EMO_NEG_1             5

#define EMO_POS_4_C           0x780A00 // color for emotion level 4
#define EMO_POS_3_C           0xCC6600
#define EMO_POS_2_C           0xFFA919
#define EMO_POS_1_C           0x800080
#define EMO_NEUTRAL_C         0x006400
#define EMO_NEG_2_C           0x2F4F4F
#define EMO_NEG_1_C           0x00008B

/*Pulse Sensor Specific Variables*/
int pulsePin = 9;                 // Pulse Sensor purple wire connected to analog pin 9

int fadeRatePulse = 0;                 // used to fade LED on with PWM on fadePin
boolean alive = false;     // we've detected a hearbeat in the last 1.5sec
unsigned long lastAlive;

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int PulseSignal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int tCoh = 10;             // coherence values total
volatile int rCoh = 50;             // coherence rating

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse


/*GSR Sensor Specific Variables*/
int gsrPin = 10;

int baseline = 220;             // seeded baseline
int gsrFadeRate = 255;          // LEDs color multiplicator

// Volatile Variables, used in the interrupt service routine!
volatile int GsrSignal;         // holds the incoming raw data
volatile int minGsrSignal;
volatile int maxGsrSignal;



void setup() {
  Serial.begin(115200);             // we agree to talk fast!
  CircuitPlayground.begin();
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN4
  //   analogReference(EXTERNAL);
  
  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(30);
  
  delay(50);                // wait until sensor GSR calms down
  minGsrSignal = 200;       // seed extrem values
  maxGsrSignal = 250;       // seed extrem values
}


//  Where the Magic Happens
void loop() {
  /*Serial Communication*/
  //serialOutputPulse();
  //serialOutputGsr();
  
  /*Handle PushButton*/
  if (CircuitPlayground.leftButton()){
    calcBaseLine();
  }
  if (CircuitPlayground.rightButton()){ // for color testing
    CircuitPlayground.strip.setPixelColor(0, EMO_POS_4_C);
    CircuitPlayground.strip.setPixelColor(1, EMO_POS_3_C);
    CircuitPlayground.strip.setPixelColor(2, EMO_POS_2_C);
    CircuitPlayground.strip.setPixelColor(3, EMO_POS_1_C);
    CircuitPlayground.strip.setPixelColor(4, EMO_NEUTRAL_C);
    CircuitPlayground.strip.setPixelColor(5, EMO_NEG_2_C);
    CircuitPlayground.strip.setPixelColor(6, EMO_NEG_1_C);
    CircuitPlayground.strip.setPixelColor(7, 0xFFFFFF);
    CircuitPlayground.strip.setPixelColor(8, 0xFFFFFF);
    CircuitPlayground.strip.setPixelColor(9, 0xFFFFFF);
    CircuitPlayground.strip.show();
    delay(10000);
    CircuitPlayground.clearPixels();
  }

  /*Pulse detection*/
  if (QS == true) {    // A Heartbeat Was Found
    alive = true;
    lastAlive = millis();
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRatePulse = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
  }
  
  unsigned long now = millis();
  if (now - lastAlive > 5000) {
    alive = false;
    //reset coherence values
    tCoh = 10;
    rCoh = 10;
  }

  refreshLeds();
  
  delay(20);                             //  take a break
}

void refreshLeds() {
  ledFadeToBeat();                      // Makes the LED Fade Effect Happen
  ledFadeToGsr();
  CircuitPlayground.strip.show();
}

void calcBaseLine() {
  CircuitPlayground.playTone(440, 500);
  unsigned long sum = 0;
  unsigned int startTime = millis();
  unsigned int now = 0;
  int index = 0;
  while (now <= BASELINE_AVERAGE_TIME) {
    sum += GsrSignal;
    index++;
    delay(20);
    now = millis() - startTime;
  }
  baseline = sum / index;
  CircuitPlayground.playTone(440, 500);
  Serial.print("Baseline: ");
  Serial.println(baseline);
}

void ledFadeToGsr() {
  float diff = GsrSignal - baseline;
  float diffPercent;
  float span;
  uint32_t color;
  if (diff >= 0) {
    span = maxGsrSignal - baseline;
    diffPercent = (diff / span) * 100;
    if (diffPercent >= EMO_POS_4) {
      color = EMO_POS_4_C;
    } else if (diffPercent >= EMO_POS_3) {
      color = EMO_POS_3_C;
    } else if (diffPercent >= EMO_POS_2) {
      color = EMO_POS_2_C;
    } else if (diffPercent >= EMO_POS_1) {
      color = EMO_POS_1_C;
    } else {
      color = EMO_NEUTRAL_C;
    }
  } else {
      span = baseline - minGsrSignal;
      diffPercent = (abs(diff) / span) * 100;
      if (diffPercent >= EMO_NEG_2) {
        color = EMO_NEG_2_C;
      } else if (diffPercent >= EMO_NEG_1) {
        color = EMO_NEG_1_C;
      }
  }

  if (CircuitPlayground.slideSwitch()) { // debug print if switch is on + side (left)
    Serial.print("Baseline: ");
    Serial.println(baseline);
    Serial.print("maxGsrSignal: ");
    Serial.println(maxGsrSignal);
    Serial.print("minGsrSignal: ");
    Serial.println(minGsrSignal);
    Serial.print("GsrSignal: ");
    Serial.println(GsrSignal);
    Serial.print("diff: ");
    Serial.println(diff);
    Serial.print("diffPercent: ");
    Serial.println(diffPercent);
    Serial.print("color: ");
    Serial.println(color, HEX);
  }
  
  CircuitPlayground.strip.setPixelColor(0, color);
  CircuitPlayground.strip.setPixelColor(9, color);
}


void ledFadeToBeat() {
  fadeRatePulse -= 15;                         //  set LED fade value
  fadeRatePulse = constrain(fadeRatePulse, 0, 255); //  keep LED fade value from going into negative numbers!

  //find color mods
  int r, g, b;
  if (tCoh >= 6) {
    r = 1; g = 0; b = 0;
  }
  else if (tCoh >= 3) {
    r = 0; g = 0; b = 1;
  }
  else {
    r = 0; g = 1; b = 0;
  }

  //fade first and last pixels to represent pulse
  CircuitPlayground.strip.setPixelColor(1, CircuitPlayground.strip.Color(fadeRatePulse * r, fadeRatePulse * g, fadeRatePulse * b));
  CircuitPlayground.strip.setPixelColor(8, CircuitPlayground.strip.Color(fadeRatePulse * r, fadeRatePulse * g, fadeRatePulse * b));
}




