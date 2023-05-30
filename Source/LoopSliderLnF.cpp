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

int LoopSliderLnF::getSliderThumbRadius(juce::Slider& slider)
{
    /* Sets slider thumb area */
    return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.011f)
        : static_cast<int> ((float)slider.getWidth() * 0.5f);
}

void LoopSliderLnF::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
                                                            float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    /* Custom linear slider graphics */

    auto loopColour = juce::Colour::Colour(0xfffc8a00);

    /* Slider track drawing */
    float trackWidth = slider.isHorizontal() ? (float)(height * 0.01f) : (float)width;

    juce::Point<float> startPoint(slider.isHorizontal() ? x * width : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    juce::Point<float> endPoint(slider.isHorizontal() ? (float)(slider.getMaximum() + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(loopColour);

    /* Min and Max slider positions */
    juce::Path valueTrack;
    juce::Point<float> minPoint, maxPoint;
    
    minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                         slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

    maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                 slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };

    valueTrack.startNewSubPath(minPoint);
    valueTrack.lineTo( maxPoint);
    g.setColour(loopColour);

    /* Draws slider min and max thumbs */
    drawPointer(g, minSliderPos,
        0.0f, (float)height, loopColour, trackWidth);

    drawPointer(g, maxSliderPos,
        0.0f, (float)height, loopColour, trackWidth);
    
}

void LoopSliderLnF::drawPointer(juce::Graphics& g, const float x, const float y, const float diameter,
                                const juce::Colour& colour, float width) noexcept
{
    /* Slider thumb drawing */

    /* Makes slider thumbs straight line running from top to bottom of slider drawing */
    //???? Possible TODO: add square or triangle "flags" (a la Protracker or Fasttracker) to tops of thumb graphics ????//
    juce::Path p;
    p.startNewSubPath(x, y);
    p.lineTo(x, y + diameter);
    p.closeSubPath();

    g.setColour(colour);
    g.strokePath(p, juce::PathStrokeType(width * 1.5f, juce::PathStrokeType::mitered,
        juce::PathStrokeType::butt));

}