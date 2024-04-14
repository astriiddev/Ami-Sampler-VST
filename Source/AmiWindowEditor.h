/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PixelBuffer.h"
#include "TrackerKeyboardComponent.h"
#include "GuiComponent.h"

//==============================================================================
/**
*/
class AmiWindowEditor  :  public juce::Component,
                          public juce::AudioProcessorValueTreeState::Listener,
                          public juce::Timer,
                          public juce::Button::Listener
{
public:
    AmiWindowEditor(AmiAudioProcessor&);
    ~AmiWindowEditor() override;

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

    void handleLoad(const juce::File&);
    void loadWaves();

    void drawWaveMenu();
    void setFont(const juce::Font& font) { pixelFont = font; }
    
    juce::String getAlertWinMessage() { return showAlertWin ? alertWinMesage : ""; }
    juce::String getAlertWinTitle()   { return showAlertWin ? alertWinTitle : ""; }

    bool& alertWinShowing() { return showAlertWin; }
    void alertWindowCleared() { showAlertWin = false; alertWinMesage = alertWinTitle = ""; }

private:
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    bool cycleWaveforms(const int key);
    bool switchWaveforms(const juce::KeyPress&);
    void checkLoops();

    void initWaveforms(PixelBuffer*);
    void initButton(juce::Button*);
    void initImgButton(juce::ImageButton*, const juce::Image, const juce::Image);

    juce::String getHostName();

    const int numWaveforms = 12;
    int currentSample = 0;
    int lastLowKey = 24, lastHighkey = 124;
    int baseOctave = 5, asciiNote = 60, currMidiChannel = 1;
    int lastPos = 0, lastPosLine = 0;

    juce::Font pixelFont;

    uint32_t scrollcolor = UINT32_MAX;

    juce::ImageComponent logoImage;
    juce::ImageButton clearSampleButton;

    juce::TextButton loadButton{ "LOAD" }, saveButton{ "SAVE" }, 
                     moreOptions{ "MORE SETTINGS" }, resampleButton{ "RESAMPLE"} ;

    const juce::Rectangle<int> scrollBack{ 32, 290, 747, 28 };
    const juce::Rectangle<int> waveBox{ 0, 0, 810, 320 };

    bool onScrollBar = false, showAlertWin = false, showExtendedOptions = false;

    std::unique_ptr<PixelBuffer> waveMenu;
    std::unique_ptr<PixelBuffer> waveform[12];

    juce::String alertWinMesage = "", alertWinTitle = "";

    const uint8_t keyPress2Note[50] =
    {
        0x0D, 0x00, 0x0F, 0x11, 0x1C, 0x00, 0x0E, 0x10, 0x00, 0x13,
        0x15, 0x17, 0x00, 0x1A, 0x00, 0x10, 0x00, 0x1F, 0x00, 0x00,
        0x00, 0x00, 0x08, 0x05, 0x04, 0x11, 0x00, 0x07, 0x09, 0x19,
        0x0B, 0x00, 0x0E, 0x0C, 0x0A, 0x1B, 0x1D, 0x0D, 0x12, 0x02,
        0x14, 0x18, 0x06, 0x0F, 0x03, 0x16, 0x01, 0x1E, 0x22, 0x20
    };

    uint8_t keysPressed[50]{};

    bool keyPressed(const juce::KeyPress&) override;
    bool keyStateChanged(bool isKeyDown) override;
    void handleExtraNoteOn(const int note);
    void handleExtraNoteOff(const int note);

    GuiComponent handleGui;

    TrackerKeyboardComponent keyboardComponent;

    AmiAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiWindowEditor)
};
