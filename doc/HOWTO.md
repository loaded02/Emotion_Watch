# Production Environment

Make sure 'serialOutput' in 'Emotion_Watch.ino' is set to false.

# Development Environment

## Wiring:
 - Pulse Sensor Black wire to Circuit Playground GND
 - Pulse Sensor Red wire to Circuit Playground VBATT
 - Pulse Sensor Purple wire to Circuit Playground #9
 - GSR Sensor Black wire to Circuit Playground GND
 - GSR Sensor Red wire to Circuit Playground 3.3V
 - GSR Sensor Yellow wire to Circuit Playground #10

## CircuitPlayground Closeup

![Closeup](https://github.com/loaded02/Emotion_Watch/raw/master/doc/image1.JPG)

## CircuitPlayground with GsrSensor and PulseSensor

![Overview](https://github.com/loaded02/Emotion_Watch/raw/master/doc/image2.JPG)

## Animation from Bitmap
[Using Bitmaps to Animate NeoPixels on Circuit Playground](https://learn.adafruit.com/circuit-playground-neoanim-using-bitmaps-to-animate-neopixels/coding-the-circuit-playground)

Generate Header file from Bitmap:
```
python convert.py neoAnim.png > neoAnim.h
```
