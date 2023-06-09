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

/*
  ==============================================================================


  //// Global custom graphics ////


  ==============================================================================
*/

//???? Possible TODO: Custom button graphic drawing ????//

//==============================================================================
class OtherLookAndFeel  : public juce::LookAndFeel_V4
{
public:
    OtherLookAndFeel(AmiSamplerAudioProcessor& p);
    ~OtherLookAndFeel() override;

    /* Custom global font */
    const juce::Font getCustomFont();
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font& f) override;
    juce::Font getLabelFont(juce::Label&) override;

    juce::MouseCursor getMouseCursorFor(juce::Component&) override;

    /* Sets slider thumb area */
    int getSliderThumbRadius(juce::Slider& slider) override;

    /* Custom linear slider graphics */
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
        float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override;
    
    void drawPointer(juce::Graphics& g, const float x, const float y, const float diameter,
        const juce::Colour& colour, float width, static const bool mirror) noexcept;

    /* Custom scrollbar graphics */
    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                        bool isScrollbarVertical, int thumbStartPosition,int thumbSize, bool isMouseOver, bool isMouseDown) override;

    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;

   // juce::Path& getFlagPath() { return flagPath; }

private:

   // juce::Path flagPath;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
