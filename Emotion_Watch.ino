
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
 The left PushButton (#4) is used to calculate a personal baseline. Start and Stop of calculation are indicated by a custom signal.
  Neopixel LEDs 0 and 9 on the Circuit Playground are used to indicate the current mood. Color-Schema:
  - TBD
 
*/

#include <Adafruit_CircuitPlayground.h>

/*define block*/
#define BASELINE_AVERAGE_TIME 60000 // interval for baseline calculation (ms)
#define MOVE_THRESHOLD 5            // mess with this number to adjust motiondetection - lower number = more sensitive

#define EMOTION_LEVEL_0       10  // Depression (Value is Percent of interval max - min)
#define EMOTION_LEVEL_1       20  // Pessimistic
#define EMOTION_LEVEL_2       25  // Fragile
#define EMOTION_LEVEL_3       30  // Isolated
#define EMOTION_LEVEL_4       35  // Bored
#define EMOTION_LEVEL_5       40  // Safe
#define EMOTION_LEVEL_6       45  // Hopeful
#define EMOTION_LEVEL_7       50  // Content
#define EMOTION_LEVEL_8       55  // Refreshed
#define EMOTION_LEVEL_9       60  // Engaged - Friendly
#define EMOTION_LEVEL_10      65  // Creative
#define EMOTION_LEVEL_11      70  // Romantic
#define EMOTION_LEVEL_12      75  // Happy
#define EMOTION_LEVEL_13      80  // Annoyed
#define EMOTION_LEVEL_14      85  // Nervous
#define EMOTION_LEVEL_15      90  // Rage
#define EMOTION_LEVEL_16      100 // Fear 

#define COLOR_ERROR           0xFFFFFF
#define COLOR_NON             0x000000

#define EMOTION_COLOR_FEAR            0x0D0D0D
#define EMOTION_COLOR_DEPRESSION      0x2F4F4F
#define EMOTION_COLOR_PESSIMISTIC     0x0000FF
#define EMOTION_COLOR_SAFE            0x00FFFF
#define EMOTION_COLOR_HOPEFUL         0x3399FF
#define EMOTION_COLOR_CONTENT         0x00FF00
#define EMOTION_COLOR_REFRESHED       0x7CFC00
#define EMOTION_COLOR_BORED           0x33FF33
#define EMOTION_COLOR_RAGE            0x190000
#define EMOTION_COLOR_ANNOYED         0xFF0000
#define EMOTION_COLOR_ROMANTIC        0xFF3333
#define EMOTION_COLOR_NERVOUS         0x780A00
#define EMOTION_COLOR_FRIENDLY        0xFF4500
#define EMOTION_COLOR_HAPPY           0xFFEA00
#define EMOTION_COLOR_ISOLATED        0xCC6600
#define EMOTION_COLOR_FRAGILE         0x3C0082
#define EMOTION_COLOR_CREATIVE        0xFF007F

#define INTERVENTION_COLOR_WORK             0x0000FF
#define INTERVENTION_COLOR_DONOTDRIVE      0xFF0000
#define INTERVENTION_COLOR_MEDITATE         0x00FF00
#define INTERVENTION_COLOR_SHOPPING         0x3399FF
#define INTERVENTION_COLOR_BUYFLOWERS       0xFF007F
#define INTERVENTION_COLOR_MAKEDECISSION    0x7CFC00
#define INTERVENTION_COLOR_SOLVEPROBLEM     0xFF4500
#define INTERVENTION_COLOR_EXCERCISE        0x00FFFF

uint32_t color_emotion;             // currently selected emotion color
int last_intervention_led;

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

// Volatile Variables, used in the interrupt service routine!
volatile int GsrSignal;         // holds the incoming raw data
volatile int minGsrSignal;
volatile int maxGsrSignal;

/*Accelerometer Specific Variables*/
float x, y, z;
boolean inMotion;



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

  inMotion = false;

  showSignal();
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

  /*Pulse detection*/
  if (QS == true) {    // A Heartbeat Was Found
    alive = true;
    lastAlive = millis();
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    fadeRatePulse = 255;         // Makes the LED Fade Effect Happen
    // Set 'fadeRate' Variable to 255 to fade LED with pulse
    //serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    QS = false;                      // reset the Quantified Self flag for next time
  }
  
  unsigned long now = millis();
  if (now - lastAlive > 5000) {
    alive = false;
    //reset coherence values
    tCoh = 10;
    rCoh = 10;
  }

  inMotion = moving();

  refreshLeds();
  
  delay(20);                             //  take a break
}

void refreshLeds() {
  ledFadeToEmotions();
  ledFadeToInterventions();
  CircuitPlayground.strip.show();
}

/**
 * detect motion
 */
 boolean moving() {
  // Take a reading of accellerometer data
  x = CircuitPlayground.motionX();
  y = CircuitPlayground.motionY();
  z = CircuitPlayground.motionZ();
    
  //Serial.print("Accel X: "); Serial.print(x); Serial.print(" ");
  //Serial.print("Y: "); Serial.print(y);       Serial.print(" ");
  //Serial.print("Z: "); Serial.print(z);     Serial.print(" ");
 
  // Get the magnitude (length) of the 3 axis vector
  // http://en.wikipedia.org/wiki/Euclidean_vector#Length
  double storedVector = x*x;
  storedVector += y*y;
  storedVector += z*z;
  storedVector = sqrt(storedVector);
  //Serial.print("Len: "); Serial.println(storedVector);
  
  // wait a bit
  delay(100);
  
  // get new data!
  x = CircuitPlayground.motionX();
  y = CircuitPlayground.motionY();
  z = CircuitPlayground.motionZ();
  double newVector = x*x;
  newVector += y*y;
  newVector += z*z;
  newVector = sqrt(newVector);
  //Serial.print("New Len: "); Serial.println(newVector);
  
  // are we moving 
  if (abs(newVector - storedVector) > MOVE_THRESHOLD) {
    //Serial.println("Twinkle!");
    return true;
  }
  return false;
 }

/**
 * signal if something changes
 */
void showSignal() {
  if (CircuitPlayground.slideSwitch()) { // sound if switch is on + side (left)
    CircuitPlayground.playTone(500, 100);
  }

  CircuitPlayground.clearPixels();

  for(int ii = 9; ii > 0 ; ii--) {
    int prevBreathLED = ii;
    int breathLED = --ii;
    
    for (int i = 1; i <= 8; i++) {
      if (i == breathLED) { //leading breath LED
        CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(200, 200, 200));
      }
      else if (i == prevBreathLED) {  //chaser/fade LED
        CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(16, 16, 16));
      }
      else CircuitPlayground.strip.setPixelColor(i, CircuitPlayground.strip.Color(0, 0, 0));
    }
    CircuitPlayground.strip.show();
    delay(150);
  }

  CircuitPlayground.clearPixels();
}

void calcBaseLine() {
  showSignal();
  unsigned long sum = 0;
  unsigned int startTime = millis();
  unsigned int now = 0;
  int index = 0;
  int fadeRate = 255;
  
  while (now <= BASELINE_AVERAGE_TIME) {
    sum += GsrSignal;
    index++;

    if (index % 70 == 0) {
      //flash first and last pixels to represent pulse
      fadeRate = 180;
      CircuitPlayground.strip.setPixelColor(0, CircuitPlayground.strip.Color(fadeRate * 200, fadeRate * 200, fadeRate * 200));
      CircuitPlayground.strip.setPixelColor(9, CircuitPlayground.strip.Color(fadeRate * 200, fadeRate * 200, fadeRate * 200));
      CircuitPlayground.strip.show();
    }
    else {
      CircuitPlayground.clearPixels();
    }
    
    delay(20);
    now = millis() - startTime;
  }
  
  baseline = sum / index;
  minGsrSignal = baseline - 50;       // seed extrem values
  maxGsrSignal = baseline + 50;       // seed extrem values
  showSignal();
  //Serial.print("Baseline: ");
  //Serial.println(baseline);
}

void ledFadeToInterventions() {
  int intervention_led;

  /*reset neopixels*/
  CircuitPlayground.strip.setPixelColor(1, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(2, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(3, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(4, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(5, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(6, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(7, COLOR_NON);
  CircuitPlayground.strip.setPixelColor(8, COLOR_NON);

  /*set neopixels*/
  if (color_emotion == EMOTION_COLOR_ROMANTIC) {
    CircuitPlayground.strip.setPixelColor(1, INTERVENTION_COLOR_BUYFLOWERS);
    intervention_led = 1;
  }
  if (color_emotion == EMOTION_COLOR_CREATIVE) {
    CircuitPlayground.strip.setPixelColor(2, INTERVENTION_COLOR_SOLVEPROBLEM);
    intervention_led = 2;
  }
  if (color_emotion == EMOTION_COLOR_SAFE) {
    CircuitPlayground.strip.setPixelColor(3, INTERVENTION_COLOR_MAKEDECISSION);
    intervention_led = 3;
  }
  if (color_emotion == EMOTION_COLOR_RAGE ||
       color_emotion == EMOTION_COLOR_ANNOYED) {
    CircuitPlayground.strip.setPixelColor(4, INTERVENTION_COLOR_DONOTDRIVE);
    intervention_led = 4;
  }   
  if (color_emotion == EMOTION_COLOR_PESSIMISTIC ||
       color_emotion == EMOTION_COLOR_NERVOUS) {
    CircuitPlayground.strip.setPixelColor(5, INTERVENTION_COLOR_SHOPPING);
    intervention_led = 5;
  }   
  if (color_emotion == EMOTION_COLOR_ANNOYED ||
       color_emotion == EMOTION_COLOR_FRAGILE) {
    CircuitPlayground.strip.setPixelColor(6, INTERVENTION_COLOR_MEDITATE);
    intervention_led = 6;
  }   
  if (color_emotion == EMOTION_COLOR_FEAR) {
    CircuitPlayground.strip.setPixelColor(7, INTERVENTION_COLOR_EXCERCISE);
    intervention_led = 7;
  }
  if (color_emotion == EMOTION_COLOR_REFRESHED ||
       color_emotion == EMOTION_COLOR_CREATIVE ||
       color_emotion == EMOTION_COLOR_HOPEFUL ||
       color_emotion == EMOTION_COLOR_FRIENDLY) {
    CircuitPlayground.strip.setPixelColor(8, INTERVENTION_COLOR_WORK);
    intervention_led = 8;
  }

  /*signal new intervention*/
  if (last_intervention_led != intervention_led) {
    if (CircuitPlayground.slideSwitch()) { // sound if switch is on + side (left)
      CircuitPlayground.playTone(500, 100);
    }
  }
  last_intervention_led = intervention_led;

}

/**
 * NeoPixels 0 and 9 show Emotion
 */
void ledFadeToEmotions() {
  
  float span = maxGsrSignal - minGsrSignal;
  int percent = ((GsrSignal - minGsrSignal) / span) * 100;

  // TODO: inMotion ?? - HFR ??

  if (percent <= EMOTION_LEVEL_0) {
    color_emotion = EMOTION_COLOR_DEPRESSION;
  }
  else if (percent <= EMOTION_LEVEL_1) {
    color_emotion = EMOTION_COLOR_PESSIMISTIC;
  }
  else if (percent <= EMOTION_LEVEL_2) {
    color_emotion = EMOTION_COLOR_FRAGILE;
  }
  else if (percent <= EMOTION_LEVEL_3) {
    color_emotion = EMOTION_COLOR_ISOLATED;
  }
  else if (percent <= EMOTION_LEVEL_4) {
    color_emotion = EMOTION_COLOR_BORED;
  }
  else if (percent <= EMOTION_LEVEL_5) {
    color_emotion = EMOTION_COLOR_SAFE;
  }
  else if (percent <= EMOTION_LEVEL_6) {
    color_emotion = EMOTION_COLOR_HOPEFUL;
  }
  else if (percent <= EMOTION_LEVEL_7) {
    color_emotion = EMOTION_COLOR_CONTENT;
  }
  else if (percent <= EMOTION_LEVEL_8) {
    color_emotion = EMOTION_COLOR_REFRESHED;
  }
  else if (percent <= EMOTION_LEVEL_9) {
    color_emotion = EMOTION_COLOR_FRIENDLY;
  }
  else if (percent <= EMOTION_LEVEL_10) {
    color_emotion = EMOTION_COLOR_CREATIVE;
  }
  else if (percent <= EMOTION_LEVEL_11) {
    color_emotion = EMOTION_COLOR_ROMANTIC;
  }
  else if (percent <= EMOTION_LEVEL_12) {
    color_emotion = EMOTION_COLOR_HAPPY;
  }
  else if (percent <= EMOTION_LEVEL_13) {
    color_emotion = EMOTION_COLOR_ANNOYED;
  }
  else if (percent <= EMOTION_LEVEL_14) {
    color_emotion = EMOTION_COLOR_NERVOUS;
  }
  else if (percent <= EMOTION_LEVEL_15) {
    color_emotion = EMOTION_COLOR_RAGE;
  }
  else if (percent <= EMOTION_LEVEL_16) {
    color_emotion = EMOTION_COLOR_FEAR;
  }
  else {
    color_emotion = COLOR_ERROR;
  }

//  Serial.print("Baseline: ");
//  Serial.println(baseline);
//  Serial.print("maxGsrSignal: ");
//  Serial.println(maxGsrSignal);
//  Serial.print("minGsrSignal: ");
//  Serial.println(minGsrSignal);
//  Serial.print("GsrSignal: ");
//  Serial.println(GsrSignal);
//  Serial.print("span: ");
//  Serial.println(span);
//  Serial.print("percent: ");
//  Serial.println(percent);
//  Serial.print("color_emotion: ");
//  Serial.println(color_emotion, HEX);
  
  CircuitPlayground.strip.setPixelColor(0, color_emotion);
  CircuitPlayground.strip.setPixelColor(9, color_emotion);
}

/**
 * NOT USED
 */
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




