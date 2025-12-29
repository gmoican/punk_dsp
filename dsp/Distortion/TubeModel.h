#pragma once

/**
 * @class TubeModel
 * @brief Implements several types of waveshaping function
 *
 * Contains some waveshaping functions
 * On its own, it works as a clipper
 * In combination with filters, adequate input gain and bias, it works as the core of distortion processors
 */
namespace punk_dsp
{
    class TubeModel
    {
    public:
        TubeModel();
        ~TubeModel() = default;

        float processSample(float sample);
        void processBuffer(juce::AudioBuffer<float>& inputBuffer);

        // Parameter Updates
        void setDrive(float newDrive);
        void setOutGain(float newOutGain);
        void setBiasPre(float newBiasPre);
        void setBiasPost(float newBiasPost);
        void setCoeffPos(float newCoeffPos);
        void setCoeffNeg(float newCoeffNeg);
        void setHarmonicGain(float newHarmGain);
        void setHarmonicBalance(float newBalance);
        void setHarmonicSidechain(bool usePostDrive);

    private:
        float drive { 1.0f };
        float outGain { 1.0f };

        float biasPre { 0.0f };
        float biasPost { 0.0f };

        float coeffPos { 1.0f };
        float coeffNeg { 1.2f };
        
        float harmonicGain { 0.1f };
        float harmonicBalance { 0.5f };
        bool harmonicSidechain { true };

        // Extra processing
        float addHarmonics(float inputSignal);

        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeModel)
    };
}