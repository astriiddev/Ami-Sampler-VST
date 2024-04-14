/*
  ==============================================================================

    AmiAlertWindow.h
    Created: 30 Mar 2024 10:58:10am
    Author:  astriid

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PixelBuffer.h"

//==============================================================================
/*
*/
class AmiAlertWindow  : public juce::Component
{
public:
    AmiAlertWindow(const int width, const int height, const juce::String &title, const juce::String& message);
    ~AmiAlertWindow() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    bool wasOkPressed() { return okPressed; }
private:
    Rect_t okRect = { 0, 0, 0 ,0, 0 };
    bool okPressed = false;

    std::unique_ptr<PixelBuffer> pixelBuffer = nullptr;

    bool hitbox(const Rect_t *b, const int x, const int y);
    void drawButton(const bool clicked, const Rect_t *b, const std::string txt);
    void drawOkButton(const bool clicked) { drawButton(clicked, &okRect, std::string("OK")); }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiAlertWindow)
};
