# Emotion Watch

 based on Mediation Trainer by Collin Cunningham and  
 based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman http://www.pulsesensor.com and  
 based on Arduino and Galvanic Skin Response (GSR) Sensor by hypnotistas

 Uses a Circuit Playground board's built-in LEDs to diplay mood according to sensor input
 
 The [ChangeLog](doc/CHANGELOG.md) provides you with information about important changes.
 
 The [DevEnvironment](doc/HOWTO.md) explains setup of device during development.
 
**Hardware requirements:**
 - Adafruit Circuit Playground https://www.adafruit.com/products/3000
 - Pulse Sensor Amped https://www.adafruit.com/products/1093
 - Grove GSR Sensor https://www.seeedstudio.com/Grove-GSR-sensor-p-1614.html

**Wiring:**
 - Pulse Sensor Black wire to Circuit Playground GND
 - Pulse Sensor Red wire to Circuit Playground VBATT
 - Pulse Sensor Purple wire to Circuit Playground #9
 - GSR Sensor Black wire to Circuit Playground GND
 - GSR Sensor Red wire to Circuit Playground 3.3V
 - GSR Sensor Yellow wire to Circuit Playground #10

**Mood Control:**
 The left PushButton (#4) is used to calculate a personal baseline. Start and Stop of calculation are indicated by a sound.
 Neopixel LEDs 0 and 9 on the Circuit Playground are used to indicate the current mood. Color-Schema:
 - TBD

**Pulse:** 
 Neopixel LEDs 1 and 8 on the Circuit Playground flash with each pulse detected and their color indicates a basic HRV "coherence" rating which roughly translates to the user's level of relaxation:
 - red = low relaxation
 - blue = moderate relaxation
 - green = high relaxation
 