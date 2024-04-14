/*
  ==============================================================================

    AmiAudioProcessorEditor.h
    Created: 30 Mar 2024 1:31:35pm
    Author:  astriid

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "AmiWindowEditor.h"
#include "AmiLookAndFeel.h"
#include "AmiAlertWindow.h"

//==============================================================================
/*
*/
class AmiAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                 public juce::FileDragAndDropTarget,
                                 public juce::Button::Listener,
                                 public juce::Timer
{
public:
    AmiAudioProcessorEditor(AmiAudioProcessor&);
    ~AmiAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int, int) override;

private:
    juce::ImageButton resetScale, upScale, downScale;
    std::unique_ptr<AmiWindowEditor> amiWindow = nullptr;

    AmiLookAndFeel lookNFeel;

    AmiAudioProcessor& audioProcessor;
    
    bool keyPressed(const juce::KeyPress&) override;
    void initImgButton(juce::ImageButton*, const juce::Image&, const juce::Image&);
    void buttonClicked(juce::Button* button) override;
    void visibilityChanged() override;
    void timerCallback() override;

    void createAlertWin(const juce::String &title, const juce::String& message);
    std::unique_ptr<AmiAlertWindow> alertWin = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiAudioProcessorEditor)
};
