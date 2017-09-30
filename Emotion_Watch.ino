
/*  Emotion Watch 
 Emotion Tracker by Moritz Hilberg
 based on Mediation Trainer by Collin Cunningham and
 based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman http://www.pulsesensor.com and
 based on Arduino and Galvanic Skin Response (GSR) Sensor by hypnotistas
 based on Using Bitmaps to Animate NeoPixels on Circuit Playground https://learn.adafruit.com/circuit-playground-neoanim-using-bitmaps-to-animate-neopixels
 based on Using notes for a melody on CircuitPlayground https://learn.adafruit.com/circuit-playground-music/the-sound-of-music
 

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
  Neopixel LEDs 0 and 9 on the Circuit Playground are used to indicate the current mood. Based on HFR, GSR and Accelleration values.
  Neopixel LEDs 1 to 8 are used to suggest interactions according to the currently detected emotion.
  The left PushButton (#4) is used to calculate a personal baseline. Start and Stop of calculation are indicated by a custom signal.
 
*/

#include <Adafruit_CircuitPlayground.h>
#include <avr/eeprom.h>
#include "emotion_levels.h"

/*define block*/
#define BASELINE_AVERAGE_TIME 10000 // interval for baseline calculation (ms)
#define BASELINE_OFFSET       50    // min, max = basline +/- offset
#define ACCEL_AVERAGE_TIME    5000
#define MOVE_THRESHOLD        0.1   // mess with this number to adjust motiondetection - lower number = more sensitive
#define EEPROM_ADDRESS        100

uint32_t color_emotion;             // currently selected emotion color
int last_intervention_led, intervention_led;

/*Regards Serial OutPut  -- Set This Up to your needs*/
static boolean serialOutput = false;   // Switch Serial Output on and off. Set to 'false' in Production!
static boolean serialVisual = false;   // Set to 'false' for Arduino Serial Plotter.  Set to 'true' to see Arduino Serial Monitor ASCII

/*Pulse Sensor Specific Variables*/
int pulsePin = 9;                   // Pulse Sensor purple wire connected to analog pin 9

boolean alive = false;              // we've detected a hearbeat in the last 1.5sec
uint32_t lastAlive;

/*Volatile Variables, used in the interrupt service routine!*/
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int PulseSignal;           // holds the incoming raw data
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.
volatile int tCoh = 10;             // coherence values total

/*GSR Sensor Specific Variables*/
int gsrPin = 10;
int baseline;

/*Volatile Variables, used in the interrupt service routine!*/
volatile int GsrSignal;         // holds the incoming raw data
volatile int minGsrSignal;
volatile int maxGsrSignal;
volatile int AvGsrSignal;       // sliding average of gsr value

/*Accelerometer Specific Variables*/
boolean inMotion = false;       // InMotion value. Is true, when device is above MOVE_THRESHOLD
int inMotionNoValues = 0;
unsigned int inMotionSum = 0;
boolean AvInMotion = false;     // Average InMotion value. Is true, when device is during ACCEL_AVERAGE_TIME more than 50% above MOVE_THRESHOLD
uint32_t startAvInMotion = 0;


void setup() {
  Serial.begin(115200);             // we agree to talk fast!
  CircuitPlayground.begin();
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS
  // IF YOU ARE POWERING The Pulse Sensor AT VOLTAGE LESS THAN THE BOARD VOLTAGE,
  // UN-COMMENT THE NEXT LINE AND APPLY THAT VOLTAGE TO THE A-REF PIN4
  //   analogReference(EXTERNAL);
  
  CircuitPlayground.clearPixels();
  CircuitPlayground.setBrightness(30);
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_16_G);
  
  delay(50);                // wait until sensor GSR calms down
  eeprom_read_block((void*)&baseline, (void*)EEPROM_ADDRESS, sizeof(baseline));
  if (baseline == -1) {
    baseline = 200;           // seed baseline
  }
  minGsrSignal = baseline - BASELINE_OFFSET;        // seed extrem values
  maxGsrSignal = baseline + BASELINE_OFFSET;       // seed extrem values

  showSignal();
}

/** 
 * Where the Magic Happens 
 */
void loop() {
  if (serialOutput) {
    serialOutputData();
  }
  
  /*Handle PushButton*/
  if (CircuitPlayground.leftButton()){
    calcBaseLine();
  }

  /*Pulse detection*/
  if (QS == true) {    // A Heartbeat Was Found
    alive = true;
    CircuitPlayground.redLED(false);
    lastAlive = millis();
    // BPM and IBI have been Determined
    // Quantified Self "QS" true when arduino finds a heartbeat
    if (serialOutput) {
      serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.
    }
    QS = false;                      // reset the Quantified Self flag for next time
  }
  
  uint32_t now = millis();
  if (now - lastAlive > 5000) {
    alive = false;
    CircuitPlayground.redLED(true);
    //reset coherence value
    tCoh = 10;
  }

  /*Acceleration detection*/
  inMotion = moving();
  if (now - startAvInMotion < ACCEL_AVERAGE_TIME) {
    inMotionSum += inMotion;
    inMotionNoValues++;
  } else {
    if (float(inMotionSum) / inMotionNoValues > 0.5) { // if you were moving more than 50% of the time during last interval
      if (!AvInMotion) {
        calcBaseLine(); // re init baseline when moving state changes
      }
      AvInMotion = true;
    } else {
      if (AvInMotion) {
        calcBaseLine(); // re init baseline when moving state changes
      }
      AvInMotion = false;
    }
//    Serial.print("inMotionSum: ");
//    Serial.println(inMotionSum);
//    Serial.print("calc: ");
//    Serial.println(float(inMotionSum) / inMotionNoValues);
    inMotionSum = 0;
    inMotionNoValues = 0;
    startAvInMotion = now;
  }
  if (AvInMotion) {
    if (serialOutput) {
      serialOutputWhenMoving();
    }
  }

  refreshLeds();
  
  delay(20); //  take a break
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
  float x = CircuitPlayground.motionX() / 9.8; // m/s2 to G
  float y = CircuitPlayground.motionY() / 9.8;
  float z = CircuitPlayground.motionZ() / 9.8;
    
  //Serial.print("Accel X: "); Serial.print(x); Serial.print(" ");
  //Serial.print("Y: "); Serial.print(y);       Serial.print(" ");
  //Serial.print("Z: "); Serial.print(z);     Serial.print(" ");
  
  // Get the magnitude (length) of the 3 axis vector
  // http://en.wikipedia.org/wiki/Euclidean_vector#Length
  float d = sqrt(x * x + y * y + z * z);
  d = fabs(d - 1.0); // Neutral is 1G; d is relative acceleration now
  //Serial.print("d: "); Serial.println(d);
  
  // are we moving 
  if (d >= MOVE_THRESHOLD) {
    //Serial.println("Twinkle!");
    return true;
  }
  return false;
}

/**
 * Adjust baseline, min-, maxGsrSignal to current state of user
 * Needed if Moving or not. Or if person changes.
 */
void calcBaseLine() {
  showSignal();
  unsigned long sum = 0;
  uint32_t startTime = millis();
  int index = 0;
  int fadeRate = 255;
  
  while (millis() - startTime < BASELINE_AVERAGE_TIME) {
    if (CircuitPlayground.leftButton() || CircuitPlayground.rightButton()){
      showSignal();
      return;
    }
    
    sum += GsrSignal;
    index++;

    if (index % 70 == 0) {
      flashPixels();
    }
    
    delay(20);
  }
  
  baseline = sum / index;
  minGsrSignal = baseline - BASELINE_OFFSET;       // seed extrem values
  maxGsrSignal = baseline + BASELINE_OFFSET;       // seed extrem values
  eeprom_write_block((const void*)&baseline, (void*)EEPROM_ADDRESS, sizeof(baseline));
  showSignal();
  //Serial.print("Baseline: ");
  //Serial.println(baseline);
}

/**
 * NeoPixels 1 to 8 show Interventions
 */
void ledFadeToInterventions() {

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
      playSound();
    }
  }
  last_intervention_led = intervention_led;
}

/**
 * NeoPixels 0 and 9 show Emotion
 */
void ledFadeToEmotions() {
  
  float span = maxGsrSignal - minGsrSignal;
  int gsrPercent = ((AvGsrSignal - minGsrSignal) / span) * 100;
  
  if (!AvInMotion) {
    emoNoMovement(gsrPercent);
  }
  else {
    emoMovement(gsrPercent);
  }
  
  //  Serial.print("Baseline: ");
  //  Serial.println(baseline);
  //  Serial.print("maxGsrSignal: ");
  //  Serial.println(maxGsrSignal);
  //  Serial.print("minGsrSignal: ");
  //  Serial.println(minGsrSignal);
  //  Serial.print("AvGsrSignal: ");
  //  Serial.println(AvGsrSignal);
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
 * Emotion calcultion when not moving
 */
void emoNoMovement(int gsrPercent) {
  if (gsrPercent <= EMOTION_NOM_LEVEL_0) {
    color_emotion = EMOTION_COLOR_DEPRESSION;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_1) {
    color_emotion = EMOTION_COLOR_PESSIMISTIC;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_2) {
    color_emotion = EMOTION_COLOR_FRAGILE;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_3) {
    color_emotion = EMOTION_COLOR_ISOLATED;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_4) {
    color_emotion = EMOTION_COLOR_BORED;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_5) {
    color_emotion = EMOTION_COLOR_SAFE;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_6) {
    color_emotion = EMOTION_COLOR_HOPEFUL;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_7) {
    color_emotion = EMOTION_COLOR_CONTENT;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_8) {
    color_emotion = EMOTION_COLOR_REFRESHED;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_9) {
    color_emotion = EMOTION_COLOR_FRIENDLY;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_10) {
    color_emotion = EMOTION_COLOR_CREATIVE;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_11) {
    color_emotion = EMOTION_COLOR_ROMANTIC;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_12) {
    color_emotion = EMOTION_COLOR_HAPPY;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_13) {
    color_emotion = EMOTION_COLOR_ANNOYED;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_14) {
    color_emotion = EMOTION_COLOR_NERVOUS;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_15) {
    color_emotion = EMOTION_COLOR_RAGE;
  }
  else if (gsrPercent <= EMOTION_NOM_LEVEL_16) {
    color_emotion = EMOTION_COLOR_FEAR;
    adjustToHeartRate();
  }
  else {
    color_emotion = COLOR_ERROR;
  }
}

/**
 * Emotion calcultion when moving
 */
void emoMovement(int gsrPercent) {
  if (gsrPercent < EMOTION_MOVE_LOWER_LIMIT) { // no values underneath this value
    color_emotion = COLOR_ERROR;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_0) {
    color_emotion = EMOTION_COLOR_DEPRESSION;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_1) {
    color_emotion = EMOTION_COLOR_PESSIMISTIC;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_2) {
    color_emotion = EMOTION_COLOR_FRAGILE;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_3) {
    color_emotion = EMOTION_COLOR_ISOLATED;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_4) {
    color_emotion = EMOTION_COLOR_BORED;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_5) {
    color_emotion = EMOTION_COLOR_SAFE;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_6) {
    color_emotion = EMOTION_COLOR_HOPEFUL;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_7) {
    color_emotion = EMOTION_COLOR_CONTENT;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_8) {
    color_emotion = EMOTION_COLOR_REFRESHED;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_9) {
    color_emotion = EMOTION_COLOR_FRIENDLY;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_10) {
    color_emotion = EMOTION_COLOR_CREATIVE;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_11) {
    color_emotion = EMOTION_COLOR_ROMANTIC;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_12) {
    color_emotion = EMOTION_COLOR_HAPPY;
    adjustToHeartRate();
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_13) {
    color_emotion = EMOTION_COLOR_ANNOYED;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_14) {
    color_emotion = EMOTION_COLOR_NERVOUS;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_15) {
    color_emotion = EMOTION_COLOR_RAGE;
  }
  else if (gsrPercent <= EMOTION_MOVE_LEVEL_16) {
    color_emotion = EMOTION_COLOR_FEAR;
    adjustToHeartRate();
  }
  else {
    color_emotion = COLOR_ERROR;
  }
}

/**
 * HFR - Correction of Gsr Emotion
 */
void adjustToHeartRate() {
  /*adjust emotion pessimistic if HRV available*/
  if (color_emotion == EMOTION_COLOR_PESSIMISTIC) {
    if (alive && tCoh >= 6) {
    // high Heartrate -> confirm pessimistic
    }
    else if (alive && tCoh >= 3) {
      // middle Heartrate
      color_emotion = EMOTION_COLOR_ISOLATED;
    }
    else if (alive) {
      // regular Heartrate
      color_emotion = EMOTION_COLOR_BORED;
    }
  }
  /*adjust emotion content if HRV available*/
  else if (color_emotion == EMOTION_COLOR_CONTENT) {
    if (alive && tCoh >= 6) {
    // high Heartrate
    color_emotion = EMOTION_COLOR_SAFE;
    }
    else if (alive && tCoh >= 3) {
      // middle Heartrate -> confirm content
    }
    else if (alive) {
      // regular Heartrate
      color_emotion = EMOTION_COLOR_FRIENDLY;
    }
  }
  /*adjust emotion happy if HRV available*/
  else if (color_emotion == EMOTION_COLOR_HAPPY) {
    if (alive && tCoh >= 6) {
    // high Heartrate
    color_emotion = EMOTION_COLOR_CREATIVE;
    }
    else if (alive && tCoh >= 3) {
      // middle Heartrate -> confirm happy
    }
    else if (alive) {
      // regular Heartrate
      color_emotion = EMOTION_COLOR_ANNOYED;
    }
  }
  /*adjust emotion fear if HRV available*/
  else if (color_emotion == EMOTION_COLOR_FEAR) {
    if (alive && tCoh >= 6) {
    // high Heartrate
    color_emotion = EMOTION_COLOR_ANNOYED;
    }
    else if (alive && tCoh >= 3) {
      // middle Heartrate
      color_emotion = EMOTION_COLOR_NERVOUS;
    }
    else if (alive) {
      // regular Heartrate -> confirm fear
    }
  }
}
