#pragma once

#include "juce_dsp/juce_dsp.h"

namespace punk_dsp
{
    class PitchFollower
    {
    public:
        PitchFollower()
        {
            setMinFrequency(40.0f);   // Low E on guitar
            setMaxFrequency(2000.0f); // High notes
        }
        
        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            currentSampleRate = spec.sampleRate;
            
            // Allocate buffer for autocorrelation
            int maxLag = static_cast<int>(sampleRate / minFrequency);
            audioBuffer.resize(maxLag * 2);
            autocorrelation.resize(maxLag);
            
            reset();
        }
        
        void setMinFrequency(float freq)
        {
            minFrequency = freq;
        }
        
        void setMaxFrequency(float freq)
        {
            maxFrequency = freq;
        }
        
        void setConfidenceThreshold(float threshold)
        {
            confidenceThreshold = juce::jlimit(0.0f, 1.0f, threshold);
        }
        
        void processBlock(const juce::AudioBuffer<float>& buffer)
        {
            int numSamples = buffer.getNumSamples();
            int numChannels = buffer.getNumChannels();
            
            // Mix down to mono and store in circular buffer
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float monoSample = 0.0f;
                for (int channel = 0; channel < numChannels; ++channel)
                    monoSample += buffer.getSample(channel, sample);
                
                monoSample /= static_cast<float>(numChannels);
                
                audioBuffer[writePosition] = monoSample;
                writePosition = (writePosition + 1) % audioBuffer.size();
            }
            
            // Perform pitch detection
            detectPitch();
        }
        
        float getCurrentFrequency() const
        {
            return detectedFrequency;
        }
        
        juce::String getCurrentNote() const
        {
            if (!isPitchDetected())
                return "---";
            
            return frequencyToNoteName(detectedFrequency);
        }
        
        int getCurrentMidiNote() const
        {
            if (!isPitchDetected())
                return -1;
            
            return frequencyToMidiNote(detectedFrequency);
        }
        
        float getConfidence() const
        {
            return confidence;
        }
        
        bool isPitchDetected() const
        {
            return confidence >= confidenceThreshold;
        }
        
        void reset()
        {
            std::fill(audioBuffer.begin(), audioBuffer.end(), 0.0f);
            writePosition = 0;
            detectedFrequency = 0.0f;
            confidence = 0.0f;
        }

    private:
        void detectPitch()
        {
            int minLag = static_cast<int>(currentSampleRate / maxFrequency);
            int maxLag = static_cast<int>(currentSampleRate / minFrequency);
            int bufferSize = audioBuffer.size() / 2;
            
            // Autocorrelation using YIN-like approach
            float threshold = 0.1f;
            int bestLag = -1;
            float bestCorrelation = 0.0f;
            
            for (int lag = minLag; lag < maxLag && lag < bufferSize; ++lag)
            {
                float sum = 0.0f;
                float energy1 = 0.0f;
                float energy2 = 0.0f;
                
                for (int i = 0; i < bufferSize; ++i)
                {
                    int pos1 = (writePosition - bufferSize + i + audioBuffer.size()) % audioBuffer.size();
                    int pos2 = (writePosition - bufferSize + i + lag + audioBuffer.size()) % audioBuffer.size();
                    
                    float sample1 = audioBuffer[pos1];
                    float sample2 = audioBuffer[pos2];
                    
                    sum += sample1 * sample2;
                    energy1 += sample1 * sample1;
                    energy2 += sample2 * sample2;
                }
                
                // Normalized correlation
                float denominator = std::sqrt(energy1 * energy2);
                if (denominator > 1e-6f)
                {
                    float correlation = sum / denominator;
                    
                    if (correlation > bestCorrelation)
                    {
                        bestCorrelation = correlation;
                        bestLag = lag;
                    }
                }
            }
            
            // Update detected frequency and confidence
            if (bestLag > 0 && bestCorrelation > threshold)
            {
                // Parabolic interpolation for sub-sample accuracy
                float refinedLag = refinelagEstimate(bestLag);
                detectedFrequency = static_cast<float>(currentSampleRate / refinedLag);
                confidence = bestCorrelation;
            }
            else
            {
                detectedFrequency = 0.0f;
                confidence = 0.0f;
            }
        }
        
        float refinelagEstimate(int lag)
        {
            // Simple parabolic interpolation
            if (lag <= 0 || lag >= autocorrelation.size() - 1)
                return static_cast<float>(lag);
            
            // Return the lag as-is for now (can be enhanced with interpolation)
            return static_cast<float>(lag);
        }
        
        static juce::String frequencyToNoteName(float frequency)
        {
            if (frequency <= 0.0f)
                return "---";
            
            int midiNote = frequencyToMidiNote(frequency);
            float cents = frequencyToCents(frequency);
            
            const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", 
                                       "F#", "G", "G#", "A", "A#", "B" };
            
            int noteIndex = midiNote % 12;
            int octave = (midiNote / 12) - 1;
            
            juce::String noteName = juce::String(noteNames[noteIndex]) + juce::String(octave);
            
            // Add cents deviation
            int centsDeviation = static_cast<int>(std::round(cents));
            if (centsDeviation != 0)
                noteName += " " + juce::String(centsDeviation > 0 ? "+" : "") + juce::String(centsDeviation) + "¢";
            
            return noteName;
        }
        
        static int frequencyToMidiNote(float frequency)
        {
            if (frequency <= 0.0f)
                return -1;
            
            return static_cast<int>(std::round(69.0f + 12.0f * std::log2(frequency / 440.0f)));
        }
        
        static float frequencyToCents(float frequency)
        {
            if (frequency <= 0.0f)
                return 0.0f;
            
            int midiNote = frequencyToMidiNote(frequency);
            float targetFreq = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
            
            return 1200.0f * std::log2(frequency / targetFreq);
        }
        
        std::vector<float> audioBuffer;
        std::vector<float> autocorrelation;
        int writePosition = 0;
        
        double currentSampleRate = 44100.0;
        float minFrequency = 40.0f;
        float maxFrequency = 2000.0f;
        float confidenceThreshold = 0.3f;
        
        float detectedFrequency = 0.0f;
        float confidence = 0.0f;

        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
    };
}
