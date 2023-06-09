/*
  ==============================================================================

    ControlComponent.h
    Created: 13 May 2023 5:43:35pm
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ADSRComponent.h"
#include "ButtonComponent.h"

/*
  ==============================================================================


  //// GUI button and slider controls ////


  ==============================================================================
*/

//==============================================================================
class ControlComponent  : public juce::Component
{
public:
    ControlComponent(AmiSamplerAudioProcessor& p);
    ~ControlComponent() override;
    void paint (juce::Graphics&) override;
    void paintOverChildren(juce::Graphics&) override;
    void resized() override;
    
    /* Key press commands and key release */
    bool keyPressed(const juce::KeyPress&) override;
    bool keyStateChanged(bool isKeyDown) override;

    /* MIDI Note On and Note Off event generator for extended ASCII MIDI map*/
    void handleExtraNoteOn(int note);
    void handleExtraNoteOff(int note);

private:

    /* Start and End sample loop point text */
    int mLoopStart{ 0 }, mLoopEnd{ 0 }, mLoopReplen{ 0 };
    juce::Label startLoopText{ "000000" }, endLoopText{ "000000" }, replenLoopText{ "000000" };

    /* Image generate */
    juce::ImageComponent mLogoImage;
    /* Inheret ADSR sliders for window placement */
    ADSRComponent mADSR;
    ButtonComponent mButtons;

    /* ASCII keypress and MIDI Keyboard */
    juce::Component keyListen;
    juce::MidiKeyboardComponent keyboardComponent;
    int numPadVal{ 5 }, asciiNote{ 0 };

    /* Note value array to convert ASCII keypresses to MIDI note values */
    const uint8_t keyPress2Note[50] =
    {
        0x0D, 0x00, 0x0F, 0x11, 0x1C, 0x00, 0x0E, 0x10, 0x00, 0x13,
        0x15, 0x17, 0x00, 0x1A, 0x00, 0x10, 0x00, 0x1F, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x05, 0x04, 0x11, 0x00, 0x07, 0x09, 0x19,
        0x0B, 0x00, 0x0E, 0x0C, 0x0A, 0x1B, 0x1D, 0x0D, 0x12, 0x02,
        0x14, 0x18, 0x06, 0x0F, 0x03, 0x16, 0x01, 0x1E, 0x22, 0x20
    };

    /* Is extended ASCII MIDI key currently pressed? */
    bool mCommaPressed{ false }, mLCharPressed{ false }, mDotChPressed{ false },
         mColonPressed{ false }, mSlashPressed{ false }, mEqualPressed{ false },
         mBraktPressed{ false }, mBSlhPressed { false };

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlComponent)
};