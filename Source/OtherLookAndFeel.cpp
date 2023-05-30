/*
  ==============================================================================

    OtherLookAndFeel.cpp
    Created: 13 May 2023 6:14:26pm
    Author:  finle

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"

/*
  ==============================================================================


  //// Global custom graphics ////


  ==============================================================================
*/

//==============================================================================
OtherLookAndFeel::OtherLookAndFeel(AmiSamplerAudioProcessor &p) : audioProcessor(p)
{
}

OtherLookAndFeel::~OtherLookAndFeel()
{
}

int OtherLookAndFeel::getSliderThumbRadius(juce::Slider& slider)
{
    /* Sets slider thumb area */
    return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.25f)
        : static_cast<int> ((float)slider.getWidth() * 0.5f);
}

void OtherLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, 
                    float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider& slider) 
{
    /* Custom linear slider graphics */

    /* Slider drag was too stiff, this loosens it up a bit :) */
    slider.setMouseDragSensitivity(100);

    /* Slider track drawing */
    float trackWidth = slider.isHorizontal() ? (float)height : (float)width;
    float thumbWidth = getSliderThumbRadius(slider);
    float thumbOffset = thumbWidth * 0.5f;

    juce::Point<float> startPoint(slider.isHorizontal() ? (float)x - thumbOffset : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x + thumbOffset + 1.0f) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    juce::Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(juce::Colour(0xFF00113D));
    g.strokePath(backgroundTrack, { trackWidth, juce::PathStrokeType::mitered });

    /* Graphics for slider "bevels" */
    g.setColour(juce::Colour(0xFFDDDDFF));
    g.drawLine((float)x - thumbOffset, (float)y, (float)width + (float)x + thumbOffset, (float)y + 1, 1.0f);
    g.drawLine((float)x - thumbOffset, (float)y, (float)x - thumbOffset, (float)height, 1.0f);

    g.setColour(juce::Colour(0xFF4444AA));
    g.drawLine((float)x - thumbOffset, (float)height, (float)width + (float)x + thumbOffset, (float)height, 1.0f);
    g.drawLine((float)width + (float)x + thumbOffset, (float)y, (float)width + (float)x + thumbOffset, (float)height, 1.0f);

    juce::Point<float> maxPoint;
    float kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    float ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    /* Slider thumb drawing */
    maxPoint = { kx, ky };

    auto sliderThumb = (juce::Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (trackWidth - 1.5f)).withCentre(maxPoint));

    /* Turn thumb to orange while being dragged (a la OctaMED */
    if (slider.getThumbBeingDragged() == 0)
        g.setColour(juce::Colour(0xfffc8a00));
    else
    g.setColour(juce::Colours::white);
    g.fillRect(sliderThumb);
}

void OtherLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar& scrollbar, int x, int y, int width, int height,
                                     bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown)
{
    /* Custom scrollbar graphics // most of it is just taken from JUCE's own look and feel with just some minor tweaks */

    g.fillAll(scrollbar.findColour(juce::ScrollBar::backgroundColourId));

    juce::Path slotPath, thumbPath;

    const float slotIndent = juce::jmin(width, height) > 15 ? 1.0f : 0.0f;
    const float slotIndentx2 = slotIndent * 2.0f;
    const float thumbIndent = slotIndent + 1.0f;
    const float thumbIndentx2 = thumbIndent * 2.0f;

    float gx1 = 0.0f, gy1 = 0.0f, gx2 = 0.0f, gy2 = 0.0f;
    auto thumbBorder = scrollbar.getWidth() * 0.0075f;

    /* Draws scrollbar as rectangle with respect to scrollbar size */
    if (isScrollbarVertical)
    {
        slotPath.addRectangle((float)x + slotIndent,
            (float)y + slotIndent,
            (float)width - slotIndentx2,
            (float)height - slotIndentx2);

        if (thumbSize > 0)
            thumbPath.addRectangle((float)x + thumbBorder + thumbIndent,
                (float)thumbStartPosition + thumbIndent,
                (float)width - (thumbBorder * 1.5f) - thumbIndentx2,
                (float)thumbSize - thumbIndentx2);
        gx1 = (float)x;
        gx2 = (float)x + (float)width * 0.7f;
    }
    else
    {
        slotPath.addRectangle((float)x + thumbBorder + slotIndent,
            (float)y + slotIndent,
            (float)width - thumbBorder - slotIndentx2,
            (float)height - slotIndentx2);

        if (thumbSize > 0)
            thumbPath.addRectangle((float)thumbStartPosition + thumbBorder,
                (float)y + thumbIndent,
                (float)thumbSize - thumbIndentx2 - (thumbBorder * 1.5),
                (float)height - thumbIndentx2);
        gy1 = (float)y;
        gy2 = (float)y + (float)height * 0.7f;
    }

    const juce::Colour thumbColour(scrollbar.findColour(juce::ScrollBar::thumbColourId));
    juce::Colour trackColour1, trackColour2;

    if (scrollbar.isColourSpecified(juce::ScrollBar::trackColourId)
        || isColourSpecified(juce::ScrollBar::trackColourId))
    {
        trackColour1 = trackColour2 = scrollbar.findColour(juce::ScrollBar::trackColourId);
    }
    else
    {
        trackColour1 = thumbColour.overlaidWith(juce::Colour(0x44000000));
        trackColour2 = thumbColour.overlaidWith(juce::Colour(0x19000000));
    }

    g.setGradientFill(juce::ColourGradient(trackColour1, gx1, gy1,
        trackColour2, gx2, gy2, false));
    g.fillPath(slotPath);

    if (isScrollbarVertical)
    {
        gx1 = (float)x + (float)width * 0.6f;
        gx2 = (float)x + (float)width;
    }
    else
    {
        gy1 = (float)y + (float)height * 0.6f;
        gy2 = (float)y + (float)height;
    }

    g.setGradientFill(juce::ColourGradient(juce::Colours::transparentBlack, gx1, gy1,
        juce::Colour(0x19000000), gx2, gy2, false));
    g.fillPath(slotPath);

    /* Changes scrollbar thumb to black when clicked and waveform is zoomed in (a la Workbench 1.3 scrollbar graphics) */
    if (isMouseDown && thumbSize != scrollbar.getWidth())
    {
        g.setColour(juce::Colours::black);
    }
    else
    g.setColour(thumbColour);
    g.fillPath(thumbPath);

    g.setGradientFill(juce::ColourGradient(juce::Colour(0x10000000), gx1, gy1,
        juce::Colours::transparentBlack, gx2, gy2, false));

    {
        juce::Graphics::ScopedSaveState ss(g);

        if (isScrollbarVertical)
            g.reduceClipRegion(x + width / 2, y, width, height);
        else
            g.reduceClipRegion(x, y + height / 2, width, height);

        g.fillPath(thumbPath);
    }

    g.setColour(juce::Colour(0x4c000000));
    g.strokePath(thumbPath, juce::PathStrokeType(0.4f));
}