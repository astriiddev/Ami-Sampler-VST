/*
  ==============================================================================

    TrackerKeyboardComponent.h
    Created: 31 Dec 2023 2:56:06am
    Author:  finle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class TrackerKeyboardComponent  : public juce::MidiKeyboardComponent
{
public:
    TrackerKeyboardComponent(juce::MidiKeyboardState& state, Orientation orientation) :MidiKeyboardComponent(state, orientation)
    {
        setRepaintsOnMouseActivity(false);
        pixelkey_black = juce::ImageCache::getFromMemory(BinaryData::pixelkey_black_png, BinaryData::pixelkey_black_pngSize);
    }

    ~TrackerKeyboardComponent() override
    {
    }

    void setFont(const juce::Font font) { pixelFont = font; }

    void drawBlackNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
                       bool isDown, bool /*isOver*/, juce::Colour /*noteFillColour*/) override
    {
        auto c = juce::Colours::transparentWhite;

        g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
        g.drawImageWithin(pixelkey_black, (int)area.getX(), (int)area.getY(), 
                          (int)area.getWidth(), (int)area.getHeight(),
                          juce::RectanglePlacement::stretchToFit, false);

        const float pixelSize = area.getWidth() / 8;

        if (isDown)
        {
            c = c.overlaidWith(findColour(keyDownOverlayColourId));
            g.setColour(c.withAlpha(0.5f));
            g.fillRect(area.withX(area.getX() - pixelSize).withWidth(area.getWidth() + pixelSize));
            g.setColour(c.withAlpha(1.f));
        }

        const auto currentOrientation = getOrientation();
        juce::String text = juce::String::toHexString(midiNoteNumber).paddedLeft('0', 2).toUpperCase();

        if (text.isNotEmpty())
        {
            auto fontHeight = juce::jmin(12.0f, area.getWidth() * 0.9f);

            g.setColour(juce::Colours::white);
            g.setFont(pixelFont.withHeight(fontHeight).withHorizontalScale(0.8f));

            switch (currentOrientation)
            {
            case horizontalKeyboard:
                if (isDown)
                    g.drawText(text, area.withY(area.getY() + pixelSize), juce::Justification::centred, false);
                else
                    g.drawText(text, area, juce::Justification::centred, false);
                break;
            case verticalKeyboardFacingLeft:    g.drawText(text, area.reduced(2.0f), juce::Justification::centredLeft, false); break;
            case verticalKeyboardFacingRight:   g.drawText(text, area.reduced(2.0f), juce::Justification::centredRight, false); break;
            default: break;
            }
        }
    }

    void drawWhiteNote(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area,
        bool isDown, bool /*isOver*/, juce::Colour lineColour, juce::Colour textColour)override
    {
        auto c = juce::Colours::white;

        g.setColour(c);
        g.fillRect(area);

        const float pixelSize = area.getWidth() / 12;
        juce::Rectangle<float> keyCorner = { area.getX() + 2, area.getHeight() - pixelSize, pixelSize, pixelSize};

        g.setColour(c.darker(2.0f));
        g.fillRect(keyCorner);

        keyCorner.setX(area.getRight() - pixelSize);
        g.fillRect(keyCorner);

        if (lineColour.isTransparent()) g.setColour(juce::Colours::grey);
        else g.setColour(c.darker(0.5f));

        if(isDown)
        {
            const float pixelshadow1 = (float)proportionOfHeight(0.34f);
            const float pixelshadow2 = (float)proportionOfHeight(0.66f);

            g.fillRect(area.getX() + 2, pixelshadow1, pixelSize, pixelshadow1);
            g.fillRect(area.getRight() - pixelSize, pixelshadow1, pixelSize, pixelshadow1);

            g.fillRect(area.getX() + pixelSize + 2, area.getHeight() - (pixelSize * 2), pixelSize, pixelSize);
            g.fillRect(area.getRight() - (pixelSize * 2), area.getBottom() - (pixelSize * 2), pixelSize, pixelSize);

            if (lineColour.isTransparent()) g.setColour(juce::Colours::black);
            else g.setColour(lineColour);

            g.fillRect(area.getX() + 2, pixelshadow2, pixelSize, pixelshadow1);
            g.fillRect(area.getRight() - pixelSize, pixelshadow2, pixelSize, pixelshadow1);
            g.fillRect(area.getX(), area.getBottom() - pixelSize, area.getWidth(), pixelSize);
        }
        else
        {
            g.fillRect(area.getX() + 2, area.getHeight() - pixelSize, pixelSize, pixelSize);
            g.fillRect(area.getRight() - pixelSize, area.getBottom() - pixelSize, pixelSize, pixelSize);
        }

        const auto currentOrientation = getOrientation();

        juce::String text = juce::String::toHexString(midiNoteNumber).paddedLeft('0', 2).toUpperCase();

        if (text.isNotEmpty())
        {
            auto fontHeight = juce::jmin(12.0f, getKeyWidth() * 0.9f);

            g.setColour(textColour);
            g.setFont(pixelFont.withHeight(fontHeight).withHorizontalScale(0.8f));

            switch (currentOrientation)
            {
            case horizontalKeyboard:            
                if(isDown)
                    g.drawText(text, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f).withY(area.getY() - pixelSize), juce::Justification::centredBottom, false);
                else
                    g.drawText(text, area.withTrimmedLeft(1.0f).withTrimmedBottom(2.0f), juce::Justification::centredBottom, false); 
                break;
            case verticalKeyboardFacingLeft:    g.drawText(text, area.reduced(2.0f), juce::Justification::centredLeft, false); break;
            case verticalKeyboardFacingRight:   g.drawText(text, area.reduced(2.0f), juce::Justification::centredRight, false); break;
            default: break;
            }
        }

        if (!lineColour.isTransparent())
        {
            g.setColour(lineColour);

            switch (currentOrientation)
            {
            case horizontalKeyboard:            g.fillRect(area.withWidth(2.0f)); break;
            case verticalKeyboardFacingLeft:    g.fillRect(area.withHeight(1.0f)); break;
            case verticalKeyboardFacingRight:   g.fillRect(area.removeFromBottom(1.0f)); break;
            default: break;
            }

            if (midiNoteNumber == getRangeEnd())
            {
                switch (currentOrientation)
                {
                case horizontalKeyboard:            g.fillRect(area.expanded(1.0f, 0).removeFromRight(1.0f)); break;
                case verticalKeyboardFacingLeft:    g.fillRect(area.expanded(0, 1.0f).removeFromBottom(1.0f)); break;
                case verticalKeyboardFacingRight:   g.fillRect(area.expanded(0, 1.0f).removeFromTop(1.0f)); break;
                default: break;
                }
            }
        }

    }


private:
    juce::Font pixelFont;

    juce::Image pixelkey_black;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerKeyboardComponent)
};
