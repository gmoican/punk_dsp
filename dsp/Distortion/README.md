# Distortion

A set of processors providing various waveshaping/clipping algorithms for audio distortion and saturation effects.

Jump to each processor:
* [Waveshapers](#waveshaper)
* [Tube](#Tube)

## Waveshaper

The `Waveshaper` class implements four different waveshaping algorithms commonly used in audio processing for distortion, saturation, and harmonic enhancement. Each algorithm offers distinct sonic characteristics, from smooth soft clipping to aggressive hard clipping, although the sound depends mainly on how you combine these functions with other processors. Check out the following picture to see the visual representation of each waveshaper function:

![waveshaper functions](docs/waveshapers.png)


### Features

- **Four waveshaping algorithms**: Soft Clipper, Hard Clipper, Tanh Clipper, and ATan Clipper
- **Flexible processing**: Process individual samples or entire audio buffers
- **Configurable parameters**: Adjustable gain, parameter, and bias factors
- **Namespace organization**: Clean `punk_dsp` namespace

### Class Structure

#### Waveshaper

The main class providing waveshaping functionality.

##### Private Members

- `float gainFactor` - Output gain multiplier applied after waveshaping, meant to be used for compensating the volume difference the standard waveshaper can produce
- `float param` - Reserved parameter for future algorithm variations.
- `float bias` - Reserved bias parameter for DC offset or asymmetric shaping.

##### Public Methods

###### Parameter Setters

```cpp
// Sets the input gain factor, meant to change the function shape.
void setInGain(float newGain)

// Sets the output gain factor, meant to compensate the signal level.
void setOutGain(float newGain)

// Sets the parameter factor (reserved for future use).
void setParamFactor(float newParam)

// Sets the bias factor (reserved for future use).
void setBiasFactor(float newBias)
```

## Tube

Work in progress...

