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
        envelope = 0.0f;
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

    float EnvelopeFollower::process(float input_lin)
    {
        float target = std::abs(input_lin);
        
        float alpha = (target > envelope) ? attackCoeff : releaseCoeff;
        envelope = alpha * (envelope - target) + target;
        
        return envelope;
    }
    
    float EnvelopeFollower::getEnvelope()
    {
        return envelope;
    }
}
