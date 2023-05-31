/*
  ==============================================================================

    LoopSliderLnF.cpp
    Created: 17 May 2023 9:52:53pm
    Author:  finle

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LoopSliderLnF.h"

/* 
  ==============================================================================
  

  //// Start and End loop point custom graphics ////
  

  ==============================================================================
*/

//==============================================================================
LoopSliderLnF::LoopSliderLnF(AmiSamplerAudioProcessor& p) : audioProcessor(p)
{
}

LoopSliderLnF::~LoopSliderLnF()
{
}
//
//juce::MouseCursor LoopSliderLnF::getMouseCursorFor(juce::Component& c)
//{/*
//    auto mouseCursor = juce::ImageCache::getFromMemory(BinaryData::amiMouseCursor_png, BinaryData::amiMouseCursor_pngSize);
//    return juce::MouseCursor(mouseCursor.rescaled(mouseCursor.getBounds().getWidth() * (float)c.getTopLevelComponent()->getWidth() / 128,
//        mouseCursor.getBounds().getHeight() * (float)c.getTopLevelComponent()->getHeight() / 96, juce::Graphics::lowResamplingQuality), 0, 0);*/
//    return     juce::MouseCursor::ParentCursor
;
//}

int LoopSliderLnF::getSliderThumbRadius(juce::Slider& slider)
{
    /* Sets slider thumb area */
    return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.011f)
        : static_cast<int> ((float)slider.getWidth() * 0.5f);
}

void LoopSliderLnF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                                                            float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    
    //
}
