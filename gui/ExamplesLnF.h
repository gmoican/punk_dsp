#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace punk_dsp
{
    namespace UIConstants
    {
        // 5-color palette
        const juce::Colour background    = juce::Colour(0xff070707);
        const juce::Colour primary       = juce::Colour(0xffB6771D);
        const juce::Colour secondary     = juce::Colour(0xff854836);
        const juce::Colour text          = juce::Colour(0xffF7F7F7);
        const juce::Colour highlight     = juce::Colour(0xffFFB22C);

        // Sizing constants
        const int headerHeight = 30;
        const int knobSize = 90;
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
