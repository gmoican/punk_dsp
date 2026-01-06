#include "ExamplesLnF.h"

namespace punk_dsp
{
    ExamplesLnF::ExamplesLnF()
    {
        // Set default colors for components
        setColour(juce::Slider::thumbColourId, UIConstants::primary);
        setColour(juce::Slider::trackColourId, UIConstants::secondary);
        setColour(juce::Slider::backgroundColourId, UIConstants::background);

        setColour(juce::TextButton::buttonColourId, UIConstants::secondary);
        setColour(juce::TextButton::buttonOnColourId, UIConstants::primary);
        setColour(juce::TextButton::textColourOffId, UIConstants::text);
        setColour(juce::TextButton::textColourOnId, UIConstants::background);

        setColour(juce::ComboBox::backgroundColourId, UIConstants::secondary);
        setColour(juce::ComboBox::textColourId, UIConstants::text);
    }

    void ExamplesLnF::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                            juce::Slider& slider)
    {
        // juce::ignoreUnused(slider);
        
        auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // // Define the central area for text
        // auto textBounds = juce::Rectangle<int>(x, y, width, height).reduced(width/4, height/4).toFloat();

        // Draw background circle
        g.setColour(UIConstants::secondary.withAlpha(0.3f));
        g.fillEllipse(rx, ry, rw, rw);

        // Draw track (background arc)
        g.setColour(UIConstants::secondary);
        juce::Path trackArc;
        trackArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                            rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath(trackArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Draw filled arc (value)
        g.setColour(UIConstants::primary);
        juce::Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius, 0.0f,
                            rotaryStartAngle, angle, true);
        g.strokePath(valueArc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // // Draw thumb (indicator)
        // juce::Path p;
        // auto pointerLength = radius * 0.5f;
        // auto pointerThickness = 2.0f;
        // p.addRectangle (-pointerThickness * 0.5f, -radius * 0.7f, pointerThickness, pointerLength);
        // p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        // g.setColour (UIColors::primary);
        // g.fillPath (p);
        
        // 5. Draw Dynamic Text Label/Value
        // Define the central area for text
        auto textBounds = juce::Rectangle<int>(x, y, width, height).reduced(width/4, height/4).toFloat();
        
        juce::String textToDisplay;
        float fontSize = juce::jmin(radius * 0.7f, 24.0f);
        
        // Check if the user is dragging or hovering
        if (slider.isMouseButtonDown() || slider.isMouseOver())
        {
            // Display the current value
            textToDisplay = slider.getTextFromValue(slider.getValue());
        }
        else
        {
            textToDisplay = slider.getName();
        }
        
        g.setColour(UIConstants::text);
        g.setFont(juce::FontOptions(fontSize));
        
        // Draw the text in the center of the knob
        g.drawFittedText(textToDisplay,
                        textBounds.toNearestInt(),
                        juce::Justification::centred,
                        1);
    }

    void ExamplesLnF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                            float sliderPos, float minSliderPos, float maxSliderPos,
                                            const juce::Slider::SliderStyle style, juce::Slider& slider)
    {
        // Use the default JUCE drawing for linear sliders
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }

    void ExamplesLnF::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                                const juce::Colour& backgroundColour,
                                                bool shouldDrawButtonAsHighlighted,
                                                bool shouldDrawButtonAsDown)
    {
        juce::ignoreUnused(shouldDrawButtonAsDown);
            
        auto buttonArea = button.getLocalBounds().toFloat();
        g.setColour (shouldDrawButtonAsHighlighted ? UIConstants::primary : backgroundColour);
        g.fillRoundedRectangle(buttonArea, 7.0f);
    }

    void ExamplesLnF::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                        bool shouldDrawButtonAsHighlighted,
                                        bool shouldDrawButtonAsDown)
    {
        juce::ignoreUnused(shouldDrawButtonAsHighlighted);
        
        auto font = getTextButtonFont (button, button.getHeight());
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f)
                    );
        auto yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
        auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;
        auto fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
        auto leftIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        auto rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        auto textWidth = button.getWidth() - leftIndent - rightIndent;
        auto edge = 4;
        auto offset = shouldDrawButtonAsDown ? edge / 2 : 0;
        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                            leftIndent + offset,
                            yIndent + offset,
                            textWidth,
                            button.getHeight() - yIndent * 2 - edge,
                            juce::Justification::centred,
                            2);
    }

    // COMBO-BOX ========================================
    void ExamplesLnF::drawComboBox(juce::Graphics& g, int width, int height,
                                    bool isButtonDown,
                                    int buttonX, int buttonY,
                                    int buttonW, int buttonH,
                                    juce::ComboBox& box)
    {
        auto cornerSize = 7.0f;
        auto boxBounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
        
        // Draw background
        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(boxBounds, cornerSize);
        
        // Draw outline
        g.setColour(box.hasKeyboardFocus(true) ? UIConstants::primary : UIConstants::secondary.darker());
        g.drawRoundedRectangle(boxBounds.reduced(0.5f, 0.5f), cornerSize, 1.0f);
        
        // Draw arrow button area
        auto arrowZone = juce::Rectangle<int>(buttonX, buttonY, buttonW, buttonH).toFloat();
        
        // Draw arrow
        juce::Path path;
        auto arrowW = 8.0f;
        auto arrowH = 5.0f;
        auto arrowX = arrowZone.getCentreX() - arrowW * 0.5f;
        auto arrowY = arrowZone.getCentreY() - arrowH * 0.5f;
        
        path.addTriangle(arrowX, arrowY,
                         arrowX + arrowW, arrowY,
                         arrowX + arrowW * 0.5f, arrowY + arrowH);
        
        g.setColour(box.findColour(juce::ComboBox::arrowColourId)
                    .withAlpha(box.isEnabled() ? 1.0f : 0.3f));
        g.fillPath(path);
    }

    void ExamplesLnF::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
    {
        label.setBounds(8, 0, box.getWidth() - 30, box.getHeight());
        label.setFont(getComboBoxFont(box));
        label.setJustificationType(juce::Justification::centredLeft);
    }

    juce::Font ExamplesLnF::getComboBoxFont(juce::ComboBox& box)
    {
        return juce::Font(juce::jmin(16.0f, (float)box.getHeight() * 0.85f));
    }

    void ExamplesLnF::drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                                         bool isSeparator, bool isActive, bool isHighlighted,
                                         bool isTicked, bool hasSubMenu,
                                         const juce::String& text,
                                         const juce::String& shortcutKeyText,
                                         const juce::Drawable* icon,
                                         const juce::Colour* textColourToUse)
    {
        juce::ignoreUnused(shortcutKeyText, icon);
        
        if (isSeparator)
        {
            auto r = area.reduced(5, 0);
            r.removeFromTop(juce::roundToInt((float)r.getHeight() * 0.5f - 0.5f));
            g.setColour(UIConstants::secondary.withAlpha(0.3f));
            g.fillRect(r.removeFromTop(1));
        }
        else
        {
            auto textColour = textColourToUse != nullptr ? *textColourToUse : UIConstants::text;
            
            if (isHighlighted && isActive)
            {
                g.setColour(UIConstants::primary);
                g.fillRect(area);
                textColour = UIConstants::background;
            }
            
            auto r = area.reduced(1);
            
            if (isTicked)
            {
                auto tickBounds = r.removeFromLeft(r.getHeight()).toFloat().reduced(4.0f);
                g.setColour(textColour);
                
                juce::Path tick;
                tick.addTriangle(0.0f, 0.5f, 0.4f, 1.0f, 1.0f, 0.0f);
                g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds, true));
            }
            
            if (hasSubMenu)
            {
                auto arrowH = 0.6f * getPopupMenuFont().getHeight();
                auto x = (float)r.removeFromRight((int)arrowH).getX();
                auto halfH = (float)r.getCentreY();
                
                juce::Path path;
                path.addTriangle(x, halfH - arrowH * 0.5f,
                               x, halfH + arrowH * 0.5f,
                               x + arrowH * 0.6f, halfH);
                
                g.setColour(textColour);
                g.fillPath(path);
            }
            
            r.removeFromLeft(10);
            
            g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
            g.setFont(getPopupMenuFont());
            g.drawFittedText(text, r, juce::Justification::centredLeft, 1);
        }
    }

    void ExamplesLnF::drawPopupMenuBackground(juce::Graphics& g, int width, int height)
    {
        g.fillAll(UIConstants::background);
        g.setColour(UIConstants::secondary);
        g.drawRect(0, 0, width, height, 1);
    }
}
