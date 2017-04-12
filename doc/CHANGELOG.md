0.0.2
=====

### Features
- NeoPixel 0 and 9 show Emotion based on Gsr Sensor
- NeoPixel 1 to 8 show interactions based on emotion level. A new interaction is indicated by tone.
- LeftButton (#4) starts baseline calculation. Start and Stop indicated by custom signal.
- Switch turns sound on and off ( mute when switch is on - side (right))

### BREAKING CHANGES
- RightButton (#19) without function
- no debug prints by default (commented in code)
- userfeedback during baseline calculation by neopixel animation
- motion detection in place but not used yet
- HFR detection in place but not used

### Bug Fixes

0.0.1
=====

### Features
- LeftButton (#4) starts baseline calculation. Start and Stop indicated by tone.
- RightButton (#19) shows all colors used for indicating emotions.
- NeoPixel 0 and 9 show Emotion based on Gsr Sensor
- NeoPixel 1 and 8 show Pulsefrequency based on Pulse Sensor
- Debug print via SerialMonitor if switch is on + side (left)