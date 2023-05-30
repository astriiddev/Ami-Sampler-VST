/*
  ==============================================================================

    OtherLookAndFeel.h
    Created: 13 May 2023 6:14:26pm
    Author:  finle

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

    /* Sets slider thumb area */
    int getSliderThumbRadius(juce::Slider& slider) override;

    /* Custom linear slider graphics */
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
        float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override;

    /* Custom scrollbar graphics */
    void drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                        bool isScrollbarVertical, int thumbStartPosition,int thumbSize, bool isMouseOver, bool isMouseDown) override;

private:

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OtherLookAndFeel)
};
