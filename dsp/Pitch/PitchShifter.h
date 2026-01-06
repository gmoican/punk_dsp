
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

/**
 * PitchShifter: Time-domain granular pitch shifter (dual-head Hann OLA).
 *
 * Usage:
 *  - Call prepare(sampleRate, maxBlockSize, numChannels)
 *  - Update pitch via setSemitones(...) or setPitchRatio(...)
 *  - Call process(audioBuffer) each block
 *  - Optionally, reset() and read getLatencySamples()
 *
 * Notes:
 *  - For best results, use window sizes ~20–60 ms depending on material.
 *  - Bypasses internally when ratio is ~1.0 to avoid coloration.
 * Potential enhancements
 *  - Pitch-detection guided PSOLA for vocal formants (more complex)
 *  - Phase vocoder (FFT-based) for higher fidelity at large ratios; add phase locking to reduce “phasiness”
 *  - Formant preservation by shifting spectral envelope separately
 */
class PitchShifter
{
public:
    PitchShifter();

    void prepare(double sampleRate, int maxBlockSize, int numChannels);
    void reset();

    void setSemitones(float semitones);      // [-24, +24] default range
    void setPitchRatio(double ratio);        // direct ratio (e.g. 2.0 = +12 st)

    // Tunables (call before prepare or between blocks)
    void setWindowSizeMs(double ms);         // default 40 ms
                                             // Shorter (20–30 ms): snappier, less smearing; may warble more on complex material
                                             // Longer (50–60 ms): smoother, more stable; adds latency and can smear transients
    void setOverlap(double fractional);      // default 0.5 (50%)
    void setRetriggerDelayMs(double ms);     // default = window size
    void setSmoothingTimeMs(double ms);      // default 50 ms
    void setMix(float newMix);

    void process(juce::AudioBuffer<float>& buffer);

    int getLatencySamples() const noexcept { return latencySamples; }

private:
    struct Grain
    {
        double pos = 0.0;       // position inside window [0, windowSize)
        int startIndex = 0;     // index in circular buffer where grain begins
    };

    // Per-channel state
    struct ChannelState
    {
        juce::AudioBuffer<float> delayBuffer; // [1 x bufferSize] storage
        int writeIndex = 0;

        // Dry delay (for latency compensation)
        juce::AudioBuffer<float> dryDelayBuffer;
        int dryWriteIndex = 0;

        Grain g1, g2;           // dual grains
    };

    // Internal helpers
    void allocateBuffers();
    void updateDerived();
    void retriggerGrain(ChannelState& ch, Grain& g, int delaySamples);
    inline float readInterpolated(const ChannelState& ch, int channel, double readIndex) const noexcept;
    inline float hannAt(double x01) const noexcept; // x in [0,1]

    // Configuration
    double sr = 44100.0;
    int maxBlock = 512;
    int numCh = 0;

    int windowSize = 0;         // samples
    int hopSize = 0;            // samples (overlap 0.5 -> hop = window/2)
    int retriggerDelay = 0;     // samples (default equals windowSize)
    int delayBufferSize = 0;    // samples per channel

    double windowSizeMs = 40.0;
    double overlap = 0.5;
    double retriggerDelayMs = -1.0; // -1 => use window size
    int latencySamples = 0;

    // Smoothed pitch ratio and mix
    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> pitchRatioSmoothed;
    double targetRatio = 1.0;

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> mixSmoothed;
    double targetMix = 1.0;

    // Hann table
    juce::HeapBlock<float> hannTable;
    int hannTableSize = 0;

    std::vector<ChannelState> channels;

    // Constants
    static constexpr double epsilonBypass = 1e-3; // threshold for bypassing near 1.0
};
