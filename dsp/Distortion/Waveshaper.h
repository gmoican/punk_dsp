#pragma once

/**
 * @class Waveshaper
 * @brief Implements several types of waveshaping function
 *
 * Contains some waveshaping functions
 * On its own, it works as a clipper
 * In combination with filters, adequate input gain and bias, it works as the core of distortion processors
 */
namespace punk_dsp
{
    class Waveshaper
    {
    public:
        Waveshaper();
        ~Waveshaper() = default;

        // Sample processing
        float applySoftClipper(float sample);
        float applyHardClipper(float sample);
        float applyTanhClipper(float sample);
        float applyATanClipper(float sample);
        
        // Buffer processing
        void applySoftClipper(juce::AudioBuffer<float>& inputBuffer);
        void applyHardClipper(juce::AudioBuffer<float>& inputBuffer);
        void applyTanhClipper(juce::AudioBuffer<float>& inputBuffer);
        void applyATanClipper(juce::AudioBuffer<float>& inputBuffer);

        // Parameter Updates
        void setInGain(float newInGain);
        void setOutGain(float newOutGain);
        void setBiasPre(float newBiasPre);
        void setBiasPost(float newBiasPost);

    private:
        float inGain { 1.0f };
        float outGain { 1.0f };
        float biasPre { 0.0f };
        float biasPost { 0.0f };

        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Waveshaper)
    };
}