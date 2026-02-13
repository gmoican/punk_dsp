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
        attackCoeff = calculateTimeCoeff (10.0f);
        releaseCoeff = calculateTimeCoeff (100.0f);
    }

    float EnvelopeFollower::calculateTimeCoeff(float time_ms)
    {
        return std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.f / time_ms / sampleRate);
    }

    void EnvelopeFollower::setAttack(float newAttMs)
    {
        attackCoeff = calculateTimeCoeff(newAttMs);
    }

    void EnvelopeFollower::setRelease(float newRelMs)
    {
        releaseCoeff = calculateTimeCoeff(newRelMs);
    }

    
    void EnvelopeFollower::updateEnvelope(float input_lin)
    {
        float target = std::abs(input_lin);
        
        float alpha = (target > envelope_lin) ? attackCoeff : releaseCoeff;
        
        // Exponential smoothing: y[n] = a * y[n-1] + (1-a) * x[n]
        envelope_lin = (alpha * envelope_lin) + ((1.0f - alpha) * target);
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
