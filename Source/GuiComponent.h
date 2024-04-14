/*
  ==============================================================================

    GuiComponent.h
    Created: 7 Jun 2023 10:32:26am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SliderComponent.h"

//==============================================================================
/**
*/

class GuiComponent  : public juce::Component,
                      public juce::Button::Listener,
                      public juce::AudioProcessorValueTreeState::Listener
{
public:
    GuiComponent(AmiAudioProcessor&);
    ~GuiComponent() override;

    void paint(juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent&) override;
    void mouseUp(const juce::MouseEvent&) override;

    void showMoreOptions(const bool& show);
    void changeSampleChannel(const int& channel);

private:
    void visibilityChanged() override;
    void buttonClicked(juce::Button* button) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    void initLabel(juce::Label*, const bool);
    void initButton(juce::Button*, const bool);
    void initCheckBox(juce::Button*, const juce::String&, const int);

    void changeLoopText(juce::Label*, const int);
    void changeRateText(juce::Label*);
    void changeMidiNoteText(juce::Label*, const int);

    void initAllLabels();
    void initAllCheckboxes();

    void automateLabelText(juce::Label*, const int);

    void hideMainOptions();
    void hideMoreOptions();

    juce::Rectangle<int> ledRectangle;

    juce::Label startLoopText, endLoopText, replenLoopText, sampleRateText, resampleRateText,
                sampleMidiChannel, midiRootNote, midiLowNote, midiHiNote;

    juce::ToggleButton muteBox, soloBox, modelType, ledFilter, forwardLoop, pingpongLoop,
                       monoBox, ptpolyBox, octapolyBox, hexText, decText, enableAscii2Note;

    /* Text Buttons */
    juce::TextButton enableLoop{ "LOOP" }, paulaStereo{ "PAULA" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ledAttachment,  modelAttachment,
                                                                          loopAttachment, paulaAttachment;

    SliderComponent handleSliders;

    juce::String notes[128] = { "-C2", "-C#2", "-D2", "-D#2", "-E2", "-F2", "-F#2", "-G2",
                                "-G#2", "-A2", "-A#2", "-B2", "-C1", "-C#1", "-D1", "-D#1",
                                "-E1", "-F1", "-F#1", "-G1", "-G#1", "-A1", "-A#1", "-B1",
                                 "C0",  "C#0",  "D0",  "D#0",  "E0",  "F0",  "F#0",  "G0",
                                 "G#0",  "A0",  "A#0",  "B0",  "C1",  "C#1",  "D1",  "D#1",
                                 "E1",  "F1",  "F#1",  "G1",  "G#1",  "A1",  "A#1",  "B1",
                                 "C2",  "C#2",  "D2",  "D#2",  "E2",  "F2",  "F#2",  "G2",
                                 "G#2",  "A2",  "A#2",  "B2",  "C3",  "C#3",  "D3",  "D#3",
                                 "E3",  "F3",  "F#3",  "G3",  "G#3",  "A3",  "A#3",  "B3",
                                 "C4",  "C#4",  "D4",  "D#4",  "E4",  "F4",  "F#4",  "G4",
                                 "G#4",  "A4",  "A#4",  "B4",  "C5",  "C#5",  "D5",  "D#5",
                                 "E5",  "F5",  "F#5",  "G5",  "G#5",  "A5",  "A#5",  "B5",
                                 "C6",  "C#6",  "D6",  "D#6",  "E6",  "F6",  "F#6",  "G6",
                                 "G#6",  "A6",  "A#6",  "B6",  "C7",  "C#7",  "D7",  "D#7",
                                 "E7",  "F7",  "F#7",  "G7",  "G#7",  "A7",  "A#7",  "B7",
                                 "C8",  "C#8",  "D8",  "D#8",  "E8",  "F8",  "F#8",  "G8" };

    int numVoiceState{ 0 }, currentSample = 0, lastSample = -1;
    bool showExtendedOptions = false, textInHex = true;

    const juce::Rectangle<int> waveBox{ 0, 0, 810, 320 };
    const juce::Rectangle<int> windowRect[21]
    {
      { waveBox.getX() + 31, waveBox.getBottom() / 2 - 1, waveBox.getWidth() - 58, 2},

      { waveBox.getX(), waveBox.getY(), waveBox.getWidth(), 34 },
      { waveBox.getX(), waveBox.getY(), 32, waveBox.getHeight() },
      { waveBox.getX(), waveBox.getBottom() - 33, waveBox.getWidth(), 33 },
      { waveBox.getRight() - 31, waveBox.getY(), 31, waveBox.getHeight() },

      { waveBox.getX() + 252, waveBox.getY() +  7, 466, 6 },
      { waveBox.getX() + 252, waveBox.getY() + 20, 466, 6 },

      { waveBox.getRight() - 12, waveBox.getY() + 34, 6, waveBox.getHeight() - 67 },
      { waveBox.getRight() - 25, waveBox.getY() + 34, 6, waveBox.getHeight() - 67 },

      { waveBox.getX() + 4, waveBox.getY() + 55, 24, waveBox.getHeight() - 110 },

      { waveBox.getX() + 4, waveBox.getY() + 34, 24, 30 },
      { waveBox.getX() + 4, waveBox.getBottom() - 57, 24, 24 },

      { waveBox.getX() + 10, waveBox.getY() + 38, 10, 3 },
      { waveBox.getX() + 10, waveBox.getY() + 38, 3, 16 },
      { waveBox.getX() + 10, waveBox.getY() + 44, 7, 3 },

      { waveBox.getX() + 10, waveBox.getBottom() - 53, 10, 3 },
      { waveBox.getX() + 10, waveBox.getBottom() - 53, 3, 16 },
      { waveBox.getX() + 10, waveBox.getBottom() - 47, 7, 3 },
      { waveBox.getX() + 10, waveBox.getBottom() - 40, 10, 3 },

      { waveBox.getRight() - 28, waveBox.getBottom() - 30, 12, 12 },
      { waveBox.getRight() - 20, waveBox.getBottom() - 22, 17, 19 }
    };

    AmiAudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiComponent)
};
