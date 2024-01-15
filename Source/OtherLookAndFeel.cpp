/*
  ==============================================================================

    OtherLookAndFeel.cpp
    Created: 13 May 2023 6:14:26pm
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OtherLookAndFeel.h"

#include "ami_palette.h"
/*
  ==============================================================================


  //// Global custom graphics ////


  ==============================================================================
*/

//==============================================================================
OtherLookAndFeel::OtherLookAndFeel()
{
    /* Custom mouse cursor */
    typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::amidos_ttf, BinaryData::amidos_ttfSize);
    pixelFont = juce::Font(typeface);

    mouseCursor = juce::ImageCache::getFromMemory(BinaryData::amiMouseCursor_png, BinaryData::amiMouseCursor_pngSize);
    pixelCursor = juce::MouseCursor(mouseCursor.rescaled(22,22, juce::Graphics::lowResamplingQuality), 1, 1);
}

OtherLookAndFeel::~OtherLookAndFeel()
{
}

const juce::Font OtherLookAndFeel::getCustomFont()
{
    return pixelFont;
}

juce::Typeface::Ptr OtherLookAndFeel::getTypefaceForFont(const juce::Font&)
{
    return typeface;
}

juce::Font OtherLookAndFeel::getLabelFont(juce::Label& label)
{
    const float height = label.getFont().getHeight();

    pixelFont.setHeight(height);
    return pixelFont;
}

juce::MouseCursor OtherLookAndFeel::getMouseCursorFor(juce::Component&)
{
    return pixelCursor;
}

void OtherLookAndFeel::setMouseCursorSize(const float size)
{
    int mouse_size = size > 0 ? (int) round(size * 22) : 22;

    mouse_size += 22 - (mouse_size % 11);
    pixelCursor = juce::MouseCursor(mouseCursor.rescaled(mouse_size, mouse_size, juce::Graphics::lowResamplingQuality), 0, 0);
}

juce::Font OtherLookAndFeel::getTextButtonFont(juce::TextButton& button, int buttonHeight)
{
    /* Sets text button label to main font with fized size */
    juce::String buttonTxt = button.getButtonText();
    if (buttonTxt.containsChar('\n'))
    {
        int lfCount = 1;
        for (int i = 0; i < buttonTxt.length(); i++)
            if (buttonTxt.toStdString().at(i) == '\n') lfCount++;

        pixelFont.setHeight((float)buttonHeight * (0.7f / (float)lfCount));
    }
    else
        pixelFont.setHeight((float)buttonHeight * 0.7f);
    
    return pixelFont;
}

void OtherLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool shouldDrawButtonAsDown)
{
    juce::Font font(getTextButtonFont(button, button.getHeight()));
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    g.setFont(font);
    g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
        : juce::TextButton::textColourOffId)
        .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

    bool shiftText = !shouldDrawButtonAsDown && !button.getToggleState();

    const int yIndent = shiftText ? 
                        juce::jmin(4, button.proportionOfHeight(0.3f)) : 
                        juce::jmin(4, button.proportionOfHeight(0.3f)) + 2;

    const int cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

    const int fontHeight = juce::roundToInt(font.getHeight() * 0.6f);

    const int leftIndent = shiftText ?
                           juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2)) :
                           juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2)) + 2;

    const int rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;

    if (textWidth > 0)
        g.drawFittedText(button.getButtonText(),
            leftIndent, yIndent, textWidth, 
            shiftText ? button.getHeight() - yIndent * 2 : button.getHeight() - yIndent * 2 + 2,
            juce::Justification::centred, 2);
}

void OtherLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    /* Custom button graphics */
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    juce::Colour baseColour = backgroundColour;

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

        if (!shouldDrawButtonAsDown && !button.getToggleState())
        {
            g.setColour(backgroundColour.brighter(5.0f));
            g.drawLine(bounds.getX(), bounds.getY() + 2, bounds.getWidth(), bounds.getY() + 2, 3.0f);
            g.drawLine(bounds.getX() + 2, bounds.getY(), bounds.getX() + 2, bounds.getHeight(), 3.0f);

            g.setColour(backgroundColour.darker(2.f));
            g.drawLine(bounds.getX(), bounds.getHeight() - 2, bounds.getWidth(), bounds.getHeight() - 2, 3.0f);
            g.drawLine(bounds.getWidth() - 2, bounds.getY(), bounds.getWidth() - 2, bounds.getHeight(), 3.0f);

        }
        else
        {
            g.setColour(backgroundColour.darker(2.f));
            g.drawLine(bounds.getX(), bounds.getY() + 2, bounds.getWidth(), bounds.getY() + 2, 3.0f);
            g.drawLine(bounds.getX() + 2, bounds.getY(), bounds.getX() + 2, bounds.getHeight(), 3.0f);
        }

        g.setColour(juce::Colours::black);
        g.drawRect(bounds, 2.0f);
    }
}

int OtherLookAndFeel::getSliderThumbRadius(juce::Slider& slider)
{
    return slider.isHorizontal() ? static_cast<int> ((float)slider.getHeight() * 0.25f)
                                 : static_cast<int> ((float)slider.getWidth() * 0.5f);
}

void OtherLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/, const juce::Slider::SliderStyle, juce::Slider& slider)
{
    slider.setMouseDragSensitivity(100);
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    /* Slider track drawing */
    float trackWidth = slider.isHorizontal() ? (float)height : (float)width;
    float thumbWidth = (float)getSliderThumbRadius(slider);
    float thumbOffset = thumbWidth * 0.5f;

    juce::Point<float> startPoint(slider.isHorizontal() ? (float)x - thumbOffset : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x + thumbOffset + 1.0f) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    juce::Rectangle<float> sliderBack = slider.isHorizontal() ? juce::Rectangle<float>(startPoint.x, (float)y, endPoint.x - startPoint.x - 1, trackWidth - 1) :
                                                                juce::Rectangle<float>(startPoint.y, (float)x, trackWidth - 1, endPoint.y - startPoint.y - 1);
    
    g.fillCheckerBoard(sliderBack, trackWidth / 8, trackWidth / 8, JPAL(0xFF00449A), JPAL(0xFF00113D));

    /* Graphics for slider "bevels" */
    g.setColour(JPAL(AMI_BLL));
    g.fillRect(x - (int)thumbOffset, y + height - 1, width + (int)(thumbOffset * 2), 1);
    g.fillRect(x + width + (int)thumbOffset, y, 1, height);

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(x - (int)thumbOffset, y, width + (int)(thumbOffset * 2), 1);
    g.fillRect(x - (int)thumbOffset, y, 1, height);

    juce::Point<float> maxPoint;
    float kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    float ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    /* Slider thumb drawing */
    maxPoint = { kx, ky };

    auto sliderThumb = (juce::Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (trackWidth - 1.5f)).withCentre(maxPoint));

    /* Turn thumb to orange while being dragged (a la OctaMED */

    juce::Colour thumbColour = slider.findColour(slider.thumbColourId).getARGB() == 0xFF42A2C8 ? 
                               juce::Colours::white : slider.findColour(slider.thumbColourId);

    if (slider.getThumbBeingDragged() == 0)
        g.setColour(JPAL(AMI_ORG));
    else
        g.setColour(thumbColour);
    g.fillRect(sliderThumb);
}
void OtherLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                  bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize = juce::jmin (15.0f, (float) button.getHeight() * 0.75f);
    auto tickWidth = fontSize * 1.1f;

    drawTickBox (g, button, 4.0f, ((float) button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour (button.findColour (juce::ToggleButton::textColourId));
    g.setFont (pixelFont.withHeight(fontSize));

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds().withTrimmedLeft (juce::roundToInt (tickWidth) + 10)
                                             .withTrimmedRight (2),
                      juce::Justification::centredLeft, 10);
}

void OtherLookAndFeel::drawTickBox(juce::Graphics& g, juce::Component& component, float x, float y, float w, float h, const bool ticked,
                                    const bool /*isEnabled*/, const bool /*shouldDrawButtonAsHighlighted*/, const bool /*shouldDrawButtonAsDown*/)
{
    juce::Rectangle<float> tickBounds(x, y, w, h);

    g.setColour(component.findColour(juce::ToggleButton::tickDisabledColourId));
    g.drawRect(tickBounds, 1.0f);

    if (ticked)
    {
        g.setColour(component.findColour(juce::ToggleButton::tickColourId));
        auto tick = getTickShape(0.75f);
        g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
    }
}
