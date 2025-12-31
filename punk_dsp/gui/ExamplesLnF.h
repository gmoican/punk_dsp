#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace punk_dsp
{
    namespace UIConstants
    {
        // 5-color palette
        const juce::Colour background    = juce::Colour(0xff1B3C53);
        const juce::Colour primary       = juce::Colour(0xffDFD0B8);
        const juce::Colour secondary     = juce::Colour(0xff456882);
        const juce::Colour text          = juce::Colour(0xffE0D9D9);
        const juce::Colour highlight     = juce::Colour(0xffD3DAD9);

        // Sizing constants
        const int knobSize = 80;
        const int margin = 20;
    }

    class ExamplesLnF : public juce::LookAndFeel_V4
    {
    public:
        ExamplesLnF();
            
        // Rotary slider
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                            juce::Slider& slider) override;
        
        // Linear slider
        void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle style, juce::Slider& slider) override;
        
        // Button
        void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                const juce::Colour& backgroundColour,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown) override;
        
        void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;
    };
}