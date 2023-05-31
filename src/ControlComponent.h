/*
  ==============================================================================

    ControlComponent.h
    Created: 13 May 2023 5:43:35pm
    Author:  finle

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ADSRComponent.h"

/*
  ==============================================================================


  //// GUI button and slider controls ////


  ==============================================================================
*/

/*???? Possible TODO : move all buttons to own component and then 
  inheret them into this component like the ADSR sliders ????*/

//==============================================================================
class ControlComponent  : public juce::Component,
                          public juce::Button::Listener
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

    /* Button commands */
    void buttonClicked(juce::Button* button) override;

    /* MIDI Note On and Note Off event generator for extended ASCII MIDI map*/
    void handleExtraNoteOn(int note);
    void handleExtraNoteOff(int note);

private:

    /* Start and End sample loop point text */
    int mLoopStart{ 0 }, mLoopEnd{ 0 };
    juce::Label startLoopText{ "000000" }, endLoopText{ "000000" };

    /* Image generate */
    juce::ImageComponent mLogoImage;
    juce::Image mTrashOff, mTrashClicked;

    /* Inheret ADSR sliders for window placement */
    ADSRComponent mADSR;

    /* ASCII keypress and MIDI Keyboard */
    juce::Component keyListen;
    juce::MidiKeyboardComponent keyboardComponent;
    int numPadVal{ 5 };

    /* Is extended ASCII MIDI key currently pressed? */
    bool mCommaPressed{ false }, mLCharPressed{ false }, mDotChPressed{ false },
        mColonPressed{ false }, mSlashPressed{ false }, mEqualPressed{ false },
        mBraktPressed{ false }, mBSlhPressed{ false };

    juce::TextButton mLoadSample{ "LOAD" }, mEnableLoop{ "LOOP" }, mSaveSample{ "SAVE" },
                     mSampleBacking{ " " }, mStartBacking{ " " }, mEndBacking{ " " };

    /* Image buttons */
    //???? Possible TODO: change loop enable, load sample, and save sample into drawable graphics instead of images ????//
    juce::ImageButton mClearSample/*, mEnableLoop, mLoadSample, mSaveSample*/;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlComponent)
};