/*
  ==============================================================================

    LoopSliderLnF.h
    Created: 20 May 2023 12:10:31am
    Author:  finle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
  ==============================================================================


  //// Start and End loop point custom graphics ////


  ==============================================================================
*/

/*???? Possible TODO: loop point graphics //could// be put into OtherLookAndFeel and //
  shared with drawLinearSlider function to need fewer files, but giving loop point sliders //
  their own drawLinearSlider makes it easier to edit their graphics on the fly quicker ????*/

//=============================================================================
class LoopSliderLnF : public juce::LookAndFeel_V4
{
public:
    LoopSliderLnF(AmiSamplerAudioProcessor& p);
    ~LoopSliderLnF() override;

    //juce::MouseCursor getMouseCursorFor(juce::Component&) override;

    /* Sets slider thumb area */
    int getSliderThumbRadius(juce::Slider& slider) override;

    /* Custom linear slider graphics */
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
        float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) override;

    /* Slider thumb drawing */
    void drawPointer(juce::Graphics& g, const float x, const float y, const float diameter,
        const juce::Colour& colour, float width) noexcept;

private:

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopSliderLnF)
};
