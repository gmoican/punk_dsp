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
        const int comboboxHeight = 30;
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
        
        // ComboBox
        void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                          int buttonX, int buttonY, int buttonW, int buttonH,
                          juce::ComboBox&) override;

        void positionComboBoxText(juce::ComboBox&, juce::Label&) override;

        juce::Font getComboBoxFont(juce::ComboBox&) override;

        void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                               bool isSeparator, bool isActive, bool isHighlighted,
                               bool isTicked, bool hasSubMenu, const juce::String& text,
                               const juce::String& shortcutKeyText,
                               const juce::Drawable* icon, const juce::Colour* textColourToUse) override;

        void drawPopupMenuBackground(juce::Graphics&, int width, int height) override;
    };
}
