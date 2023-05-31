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

const juce::Font OtherLookAndFeel::getCustomFont()
{
    /* Importing binary of custom font */
    static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::amidos_ttf, BinaryData::amidos_ttfSize);
    return juce::Font(typeface);
}

juce::Typeface::Ptr OtherLookAndFeel::getTypefaceForFont(const juce::Font& f)
{
    /* Returning font typeface */
    return getCustomFont().getTypeface();
}

juce::Font OtherLookAndFeel::getLabelFont(juce::Label& label)
{
    /* Sets text box label to main font with ability to resize */
    const float height = label.getFont().getHeight();
    static auto theFont = getCustomFont();
    theFont.setHeight(height);
    return theFont;
}

juce::MouseCursor OtherLookAndFeel::getMouseCursorFor(juce::Component& c)
{
    /* Custom mouse cursor */

    auto mouseCursor = juce::ImageCache::getFromMemory(BinaryData::amiMouseCursor_png, BinaryData::amiMouseCursor_pngSize);

    /* Resizing pixel art mouse cursor -- lowResamplingQuality stops rescaling from interpolating and retains pixel art graphics  */
    return juce::MouseCursor(mouseCursor.rescaled(mouseCursor.getBounds().getWidth() * (float)c.getTopLevelComponent()->getWidth() / 128,
            mouseCursor.getBounds().getHeight() * (float)c.getTopLevelComponent()->getHeight() / 96 , juce::Graphics::lowResamplingQuality), 0, 0);
}

juce::Font OtherLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    /* Sets text button label to main font with fized size */
    auto theFont = getCustomFont();
    theFont.setHeight((float)buttonHeight * 0.7f);

    return theFont;
}

int OtherLookAndFeel::getSliderThumbRadius(juce::Slider& slider)
{
    /* Sets slider thumb area */
    if(slider.getSliderStyle() == juce::Slider::SliderStyle::TwoValueHorizontal ||
       slider.getSliderStyle() == juce::Slider::SliderStyle::TwoValueVertical)
        return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.011f)
            : static_cast<int> ((float)slider.getWidth() * 0.5f);

    else
        return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.25f)
            : static_cast<int> ((float)slider.getWidth() * 0.5f);
}

void OtherLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, 
                    float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider& slider) 
{
    /* Custom linear slider graphics */

    /* One value control slider graphics and two value loop point graphics had too much 
        variance between graphics, so just kept everything in separate bools */

    auto isLoopSlider = (style == juce::Slider::SliderStyle::TwoValueHorizontal || style == juce::Slider::SliderStyle::TwoValueVertical);

    if(!isLoopSlider)
    {
        /* Standard one value control slider*/

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

    else if (isLoopSlider)
    {
        /* Two value loop point slider */

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
        valueTrack.lineTo(maxPoint);
        g.setColour(loopColour);

        /* Draws slider min and max thumbs */
        drawPointer(g, minSliderPos,
            0.0f, (float)height, loopColour, trackWidth);

        drawPointer(g, maxSliderPos,
            0.0f, (float)height, loopColour, trackWidth);
    }
}

void OtherLookAndFeel::drawPointer(juce::Graphics& g, const float x, const float y, const float diameter,
    const juce::Colour& colour, float width) noexcept
{
    /* Loop slider thumb drawing */

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

void OtherLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    /* Custom button graphics */
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = backgroundColour;

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.darker(0.1f);

    g.setColour(baseColour);

    auto flatOnLeft = button.isConnectedOnLeft();
    auto flatOnRight = button.isConnectedOnRight();
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        juce::Path path;
        path.addRectangle(bounds.getX(), bounds.getY(),
            bounds.getWidth(), bounds.getHeight());

        g.fillPath(path);

        g.setColour(button.findColour(juce::ComboBox::outlineColourId));
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
    else
    {
        g.fillRect(bounds);

        /* When button is clicked down, remove bevel giving a "pushed down" effect */
        if(!shouldDrawButtonAsDown && !button.getToggleState())
        {
            /* Button beveling, inspired by 8bitbubsy's Fasstracker II beveling */
            g.setColour(backgroundColour.brighter(5.0f));
            g.drawLine(bounds.getX(), bounds.getY() + 1, bounds.getWidth(), bounds.getY() + 1, 4.0f);
            g.drawLine(bounds.getX() + 1, bounds.getY(), bounds.getX() + 1, bounds.getHeight(), 4.0f);

            g.setColour(backgroundColour.darker(2.0f));
            g.drawLine(bounds.getX(), bounds.getHeight() - 1, bounds.getWidth(), bounds.getHeight() - 1, 3.0f);
            g.drawLine(bounds.getWidth() - 1, bounds.getY(), bounds.getWidth() - 1, bounds.getHeight(), 3.0f);
        }

        g.setColour(juce::Colours::black);
        g.drawRect(bounds, 2.0f);
    }
}