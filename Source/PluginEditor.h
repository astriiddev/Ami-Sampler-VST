/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PixelBuffer.h"
#include "OtherLookAndFeel.h"
#include "TrackerKeyboardComponent.h"
#include "GuiComponent.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AmiAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                 public juce::AudioProcessorValueTreeState::Listener,
                                 public juce::FileDragAndDropTarget,
                                 public juce::Timer,
                                 public juce::Button::Listener
{
public:
    AmiAudioProcessorEditor (AmiAudioProcessor&);
    ~AmiAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void paintOverChildren(juce::Graphics&) override;
    void resized() override;
    void visibilityChanged() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void loadWaves();

    void drawWaveMenu();

private:
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void handleQuitKeyShortcut(const juce::String, const juce::String);
    bool cycleWaveforms(const int key);
    bool switchWaveforms(const juce::KeyPress&);
    void checkLoops();

    void initWaveforms(PixelBuffer*);
    void initButton(juce::Button*);
    void initImgButton(juce::ImageButton*, const juce::Image, const juce::Image);

    const int numWaveforms = 12;
    int currentSample = 0;
    int lastLowKey = 24, lastHighkey = 124;
    int baseOctave = 5, asciiNote = 60, currMidiChannel = 1;
    uint32_t scrollcolor = UINT32_MAX;

    juce::ImageComponent logoImage;
    juce::ImageButton clearSampleButton, resetScale, upScale, downScale;

    juce::TextButton loadButton{ "LOAD" }, saveButton{ "SAVE" };

    const juce::Rectangle<int> scrollBack{ 32, 290, 747, 28 };
    const juce::Rectangle<int> waveBox{ 0, 0, 810, 320 };

    bool onScrollBar = false;

    OtherLookAndFeel lookNFeel;

    PixelBuffer waveMenu;
    PixelBuffer waveform[12];

    const uint8_t keyPress2Note[50] =
    {
        0x0D, 0x00, 0x0F, 0x11, 0x1C, 0x00, 0x0E, 0x10, 0x00, 0x13,
        0x15, 0x17, 0x00, 0x1A, 0x00, 0x10, 0x00, 0x1F, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x05, 0x04, 0x11, 0x00, 0x07, 0x09, 0x19,
        0x0B, 0x00, 0x0E, 0x0C, 0x0A, 0x1B, 0x1D, 0x0D, 0x12, 0x02,
        0x14, 0x18, 0x06, 0x0F, 0x03, 0x16, 0x01, 0x1E, 0x22, 0x20
    };
    bool keyPressed(const juce::KeyPress&) override;
    bool keyStateChanged(bool isKeyDown) override;
    void handleExtraNoteOn(const int note);
    void handleExtraNoteOff(const int note);

    GuiComponent handleGui;

    TrackerKeyboardComponent keyboardComponent;

    AmiAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiAudioProcessorEditor)
};