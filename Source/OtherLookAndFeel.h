/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 13 May 2023 6:14:26pm
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class OtherLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel();
    ~OtherLookAndFeel() override;

    /* Custom global font */

    const juce::Font getCustomFont();
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font& f) override;
    juce::Font getLabelFont(juce::Label&) override;
    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/, bool shouldDrawButtonAsDown) override;

    juce::MouseCursor getMouseCursorFor(juce::Component&) override;
    void setMouseCursorSize(const float w);

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;

    int getSliderThumbRadius(juce::Slider& slider) override;

    /* Custom linear slider graphics */
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
        float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override;

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                      bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    void drawTickBox(juce::Graphics& g, juce::Component& component,
        float x, float y, float w, float h,
        const bool ticked,
        [[maybe_unused]] const bool isEnabled,
        [[maybe_unused]] const bool shouldDrawButtonAsHighlighted,
        [[maybe_unused]] const bool shouldDrawButtonAsDown) override;
private:

    juce::Image mouseCursor;
    juce::MouseCursor pixelCursor;
    juce::Typeface::Ptr typeface;
    juce::Font pixelFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
