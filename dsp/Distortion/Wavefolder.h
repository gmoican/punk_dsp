
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * Wavefolder processor (header-only).
 *
 * Features:
 *  - Drive (dB) with multiplicative smoothing
 *  - Threshold (fold limit, linear amplitude)
 *  - Symmetry (-1 .. +1): adjusts positive vs. negative fold limits
 *  - Bias (pre-fold DC offset)
 *  - Stages: cascaded folders (1..N)
 *  - Dry/Wet mix
 *  - Output gain (dB)
 *
 * Usage:
 *    Wavefolder wf;
 *    wf.prepare ({ sampleRate, (uint32) blockSize, (uint32) numChannels });
 *    wf.setDriveDecibels (18.0f);
 *    wf.setThreshold (0.5f);
 *    wf.setSymmetry (0.2f);
 *    wf.setBias (0.0f);
 *    wf.setStages (4);
 *    wf.setMix (0.8f);
 *    wf.setOutputGainDecibels (0.0f);
 *    wf.process (juce::dsp::AudioBlock<float> (buffer));
 */
namespace punk_dsp
{
    class Wavefolder
    {
    public:
        Wavefolder();
        ~Wavefolder() = default;

        void setDrive (float newDrive);
        void setOutGain (float newOutGain);
        void setThreshold (float newThreshold);
        void setSymmetry (float newSymmetry);
        void setBias (float newBias);
        void setMix (float newMix);
        
        // Sample processing
        float foldToRangeSample(float sample);
        float foldSinSample(float sample);
        
        // Buffer processing
        void foldToRangeBuffer(juce::AudioBuffer<float>& inputBuffer);
        void foldSinBuffer(juce::AudioBuffer<float>& inputBuffer);
       
    private:
        // Parameters
        float drive     { 1.0f };   // linear
        float outGain   { 1.0f };   // linear
        float threshold { 0.6f };   // fold limit
        float symmetry  { 0.0f };   // [-1..1]
        float bias      { 0.0f };   // pre-fold offset
        float mix       { 1.0f };   // wet
    };
}
