#include "EnvelopeFollower.h"

namespace punk_dsp
{
    EnvelopeFollower::EnvelopeFollower()
    {
    }

    void EnvelopeFollower::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        
        // Recalculate time coefficients based on current sample rate
        attackCoeff = calculateTimeCoeff (attackTime);
        releaseCoeff = calculateTimeCoeff (releaseTime);
    }

    void EnvelopeFollower::reset()
    {
        envelope_lin = 0.0f;
    }

    float EnvelopeFollower::calculateTimeCoeff(float time_ms)
    {
        return std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.f / time_ms / sampleRate);
    }

    void EnvelopeFollower::setAttack(float newAttMs)
    {
        attackTime = std::max(0.1f, newAttMs);
        attackCoeff = calculateTimeCoeff(attackTime);
    }

    void EnvelopeFollower::setRelease(float newRelMs)
    {
        releaseTime = std::max(0.1f, newRelMs);
        releaseCoeff = calculateTimeCoeff(releaseTime);
    }

    void EnvelopeFollower::setPeakDetection(bool newUsePeakDetector)
    {
        usePeakDetector = newUsePeakDetector;
    }
    
    float EnvelopeFollower::peakDetection(float input_lin)
    {
        float target = std::abs(input_lin);
        
        float alpha = (target > envelope_lin) ? attackCoeff : releaseCoeff;
        
        return (alpha * envelope_lin) + ((1.0f - alpha) * target);
    }

    float EnvelopeFollower::rmsDetection(float input_lin)
    {
        float target = input_lin * input_lin;

        float alpha = (target > meanSquare) ? attackCoeff : releaseCoeff;

        meanSquare = (alpha * meanSquare) + ((1.0f - alpha) * target);

        return std::sqrt(meanSquare);
    }

    void EnvelopeFollower::updateEnvelope(float input_lin)
    {
        juce::FloatVectorOperations::disableDenormalisedNumberSupport();
        
        if (usePeakDetector)
            envelope_lin = peakDetection(input_lin);
        else
            envelope_lin = rmsDetection(input_lin);
    }
    
    float EnvelopeFollower::getEnvelope()
    {
        return envelope_lin;
    }

    float EnvelopeFollower::updateAndReturnEnvelope(float input_lin)
    {
        updateEnvelope(input_lin);
        return getEnvelope();
    }
}
