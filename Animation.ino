#include "neoAnim.h" //this is the name of the animation derrived from the neoAnim.png bitmap file
#include "neoAnim_beat.h"
//#include "pitches.h"
#include "notification.h"

#define ANIMATION_TIME_LONG   400   // ms for animation duration
#define ANIMATION_TIME_SHORT  300

/*Animation Specific Variables*/
uint16_t *pixelBaseAddr,  // Address of active animation table
         pixelLen,        // Number of pixels in active table
         pixelIdx;        // Index of first pixel of current frame
uint8_t pixelFPS;         // Frames/second for active animation
boolean pixelLoop;        // If true, animation repeats
uint32_t prev = 0;        // Time of last NeoPixel refresh

/*Sound Specific Variables*/
//const int numNotes = 2;                     // number of notes we are playing
//int melody[] = {NOTE_F6, NOTE_A6}; // specific notes in the melody
//int noteDurations[] = {16, 8}; // note durations: 4 = quarter note, 8 = eighth note, etc.:


// Begin playing a NeoPixel animation from a PROGMEM table
void initAnimation(const uint16_t *addr, uint8_t fps, uint16_t bytes, boolean repeat) {
  pixelBaseAddr = addr;
  if(addr) {
    pixelFPS    = fps;
    pixelLen    = bytes / 2;
    pixelLoop   = repeat; //if set to 'repeat' it'll loop, set to 0 to play once only
    pixelIdx    = 0;
  } else {
    CircuitPlayground.strip.clear();
  }
}

void playAnimationLong() {
  uint32_t startTime = millis();
  while (millis() - startTime < ANIMATION_TIME_LONG) {
    uint32_t t;      // Current time in milliseconds
    
    // Until the next animation frame interval has elapsed...
    while(((t = millis()) - prev) < (1000 / pixelFPS));
    // Show LEDs rendered on prior pass.  It's done this way so animation timing
    // is a bit more consistent (frame rendering time may vary slightly).
    CircuitPlayground.strip.show();
    
    prev = t; // Save refresh time for next frame sync
    
    if(pixelBaseAddr) {
      for(uint8_t i=0; i<10; i++) { // For each NeoPixel...
        // Read pixel color from PROGMEM table
        uint16_t rgb = pgm_read_word(&pixelBaseAddr[pixelIdx++]);
        // Expand 16-bit color to 24 bits using gamma tables
        // RRRRRGGGGGGBBBBB -> RRRRRRRR GGGGGGGG BBBBBBBB
        CircuitPlayground.strip.setPixelColor(i,
        pgm_read_byte(&gamma5[ rgb >> 11        ]),
        pgm_read_byte(&gamma6[(rgb >>  5) & 0x3F]),
        pgm_read_byte(&gamma5[ rgb        & 0x1F]));
      }
      if(pixelIdx >= pixelLen) { // End of animation table reached
        if(pixelLoop) { // Repeat animation
          pixelIdx = 0; // Reset index to start of table
        } else {        // else switch off LEDs
          initAnimation(NULL, neoAnimFPS, 0, false);
        }
      }
    }
  }
}

void playAnimationShort() {
  uint32_t startTime = millis();
  while (millis() - startTime < ANIMATION_TIME_SHORT) {
    uint32_t t;      // Current time in milliseconds
    
    // Until the next animation frame interval has elapsed...
    while(((t = millis()) - prev) < (1000 / pixelFPS));
    // Show LEDs rendered on prior pass.  It's done this way so animation timing
    // is a bit more consistent (frame rendering time may vary slightly).
    CircuitPlayground.strip.show();
    
    prev = t; // Save refresh time for next frame sync
    
    if(pixelBaseAddr) {
      for(uint8_t i=0; i<10; i++) { // For each NeoPixel...
        // Read pixel color from PROGMEM table
        uint16_t rgb = pgm_read_word(&pixelBaseAddr[pixelIdx++]);
        // Expand 16-bit color to 24 bits using gamma tables
        // RRRRRGGGGGGBBBBB -> RRRRRRRR GGGGGGGG BBBBBBBB
        CircuitPlayground.strip.setPixelColor(i,
        pgm_read_byte(&gamma7[ rgb >> 11        ]), // renaming gamma 5,6 to 7,8
        pgm_read_byte(&gamma8[(rgb >>  5) & 0x3F]),
        pgm_read_byte(&gamma7[ rgb        & 0x1F]));
      }
      if(pixelIdx >= pixelLen) { // End of animation table reached
        if(pixelLoop) { // Repeat animation
          pixelIdx = 0; // Reset index to start of table
        } else {        // else switch off LEDs
          initAnimation(NULL, neoAnimFPS, 0, false);
        }
      }
    }
  }
}

void playSound() {
//    for (int thisNote = 0; thisNote < numNotes; thisNote++) { // play notes of the melody
//      // to calculate the note duration, take one second divided by the note type.
//      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//      int noteDuration = 1000 / noteDurations[thisNote];
//      CircuitPlayground.playTone(melody[thisNote], noteDuration);
// 
//      // to distinguish the notes, set a minimum time between them.
//      //   the note's duration + 30% seems to work well:
//      int pauseBetweenNotes = noteDuration * 1.30;
//      delay(pauseBetweenNotes);
//    }

  CircuitPlayground.speaker.playSound(notificationAudioData, sizeof(notificationAudioData), notificationSampleRate);
  CircuitPlayground.speaker.end();  
}

/**
 * Custom Signal
 * Used for start and end of Baseline Calculation
 */
void showSignal() {
  /*sound*/
  if (CircuitPlayground.slideSwitch()) { // sound if switch is on + side (left)
    playSound();
  }

  /*neopixels from bitmap*/
  CircuitPlayground.clearPixels();
  initAnimation(neoAnimPixelData, neoAnimFPS, sizeof(neoAnimPixelData), false);
  playAnimationLong();
  CircuitPlayground.clearPixels();
}

/**
 * flash pixels to represent pulse
 */
void flashPixels() {
  /*neopixels from bitmap*/
  CircuitPlayground.clearPixels();
  initAnimation(neoAnim_beatPixelData, neoAnim_beatFPS, sizeof(neoAnim_beatPixelData), false);
  playAnimationShort();
  CircuitPlayground.clearPixels();
}

