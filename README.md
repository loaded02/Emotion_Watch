# Emotion Watch

 based on Mediation Trainer by Collin Cunningham and  
 based on Pulse Sensor Amped 1.4 by Joel Murphy and Yury Gitman http://www.pulsesensor.com and  
 based on Arduino and Galvanic Skin Response (GSR) Sensor by hypnotistas  
 based on Using Bitmaps to Animate NeoPixels on Circuit Playground https://learn.adafruit.com/circuit-playground-neoanim-using-bitmaps-to-animate-neopixels  
 based on Using notes for a melody on CircuitPlayground https://learn.adafruit.com/circuit-playground-music/the-sound-of-music
 

 Uses a Circuit Playground board's built-in LEDs to diplay mood according to Galvanic Skin Response, Heartrate and Acceleration values.
 
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
 Neopixel LEDs 0 and 9 on the Circuit Playground are used to indicate the current mood. Based on HFR, GSR and Accelleration values.
 Neopixel LEDs 1 to 8 are used to suggest interactions according to the currently detected emotion.
 The left PushButton (#4) is used to calculate a personal baseline. Start and Stop of calculation are indicated by a custom signal.

**Functionality:**
- LED 0 and 9 indicate Emotion
- LED 1 to 8 indicate Interaction
- GsrSignal is a sliding average value over 10 seconds.
- Motion has to be present for a duration of at least 5 seconds in order to be detected.
- HFR value is correction factor for calculated EmotionLevel from GSR and Acceleration.
- Red LED indicates that HFR Signal was not found.
- Switch turns sound on and off ( mute when switch is on - side (right))
- LeftButton (#4) starts baseline calculation. Start and Stop indicated by custom signal. Any PushButton stops Calculation.
- Baseline is saved in EEPROM
 
 
**What it should look like**

![Idea](https://github.com/loaded02/Emotion_Watch/raw/master/doc/screenshot_1.png)
