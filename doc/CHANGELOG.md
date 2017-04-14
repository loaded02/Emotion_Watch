### BREAKING CHANGES
- Arduino Serial Monitor AND Serial Plotter can be selected via flags. 
The order of signals in Serial Plotter is HFR Coherence Value, AvGsrSignal, AvInMotionDetection
- baseline stored in EEPROM. So it is present after restart.

### Bug Fixes
- Another Sound is selected. Sound can be customized via Melody (notes) or small wav file.
- GsrSignal is now a sliding average value over 10 seconds.
- Motion has to be for a duration of at least 5 seconds in order to be detected.

0.0.2
=====

### Features
- NeoPixel 0 and 9 show Emotion based on Gsr Sensor
- NeoPixel 1 to 8 show interactions based on emotion level. A new interaction is indicated by tone.
- LeftButton (#4) starts baseline calculation. Start and Stop indicated by custom signal. Any pushbutton stops Calculation.
- Switch turns sound on and off ( mute when switch is on - side (right))

### BREAKING CHANGES
- RightButton (#19) without function
- no debug prints by default (commented in code)
- userfeedback during baseline calculation by neopixel animation
- motion detection in place but not used yet
- HFR detection in place but not used

0.0.1
=====

### Features
- LeftButton (#4) starts baseline calculation. Start and Stop indicated by tone.
- RightButton (#19) shows all colors used for indicating emotions.
- NeoPixel 0 and 9 show Emotion based on Gsr Sensor
- NeoPixel 1 and 8 show Pulsefrequency based on Pulse Sensor
- Debug print via SerialMonitor if switch is on + side (left)