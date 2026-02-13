// This processor is directly influenced by https://thmstudio.com/blog/how-i-made-a-guitar-amp-plugin/

#pragma once

#include "juce_dsp/juce_dsp.h"

namespace punk_dsp
{
    class ParametricWaveshaper
    {
    public:
        ParametricWaveshaper();
        ~ParametricWaveshaper() = default;

        // Drive
        void setDrive_dB(float newDrive_dB);
        void setDrive_lin(float newDrive_lin);
        void setOutGain_dB(float newOutGain_dB);
        void setOutGain_lin(float newOutGain_lin);
        void setParam(float newParam);
        void setBiasPre(float newBiasPre);
        void setBiasPost(float newBiasPost);
        
        // Extras
        void setMix(float newMix);
        
        float processSample(float sample);
        void processBuffer(juce::AudioBuffer<float>& inputBuffer);

    private:
        // Parameters
        float drive     { 1.0f };
        float outGain   { 1.0f };
        float param     { 1.0f };
        float biasPre   { 0.0f };
        float biasPost  { 0.0f };
        
        float mix { 1.0f };
        
        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParametricWaveshaper)
    };
}
