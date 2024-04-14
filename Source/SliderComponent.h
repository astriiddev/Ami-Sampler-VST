/*
  ==============================================================================

    SliderComponent.h
    Created: 23 Dec 2023 11:49:24am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class SliderComponent  : public juce::Component,
                         public juce::Slider::Listener,
                         public juce::AudioProcessorValueTreeState::Listener
{
public:
    SliderComponent(AmiAudioProcessor&);
    ~SliderComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void hideSliders(const bool hide);
    void changeSampleChannel(const int& channel);
    void enableGliss(const bool enable);
    
private:
    void initSliders();
    void initSlider(juce::Slider*, juce::Label*, const juce::String);
    void initSlider(juce::Slider*, juce::Label*, const juce::String&, const juce::String&, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>*);

    void attachSlider(juce::Slider*, juce::Label*, const juce::String&, const juce::String&, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>*);
    void attachSlider(juce::Slider*, const juce::String&, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>*);
    
    void sliderValueChanged (juce::Slider *slider) override;
    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void changePanWidth(const bool, const juce::String&, const juce::String&);

    int currentSample = 0, lastSample = -1;
    bool hideChannelSliders = false;

    juce::Slider masterVolSlider,  masterPanSlider,
                 vibeSpeedSlider, vibeIntstySlider,
                 channelVolSlider, channelPanSlider,
                 downSampleSlider, glissandoSlider, fineTuneSlider,
                 attackSlider, decaySlider, sustainSlider, releaseSlider,
                 sampleRateSlider;

    juce::Label masterVolLabel,  masterPanLabel,
                vibeSpeedLabel, vibeIntstyLabel,
                channelVolLabel, channelPanLabel,
                downSampleLabel, glissandoLabel, fineTuneLabel,
                attackLabel, decayLabel, sustainLabel, releaseLabel,
                sampleRateLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolAttachment,  masterPanAttachment,
                                                                          vibeSpeedAttachment, vibeIntstyAttachment,
                                                                          channelVolAttachment, channelPanAttachment,
                                                                          downSampleAttachment, glissandoAttachment, fineTuneAttachment,
                                                                          attackAttachment, decayAttachment, sustainAttachment, releaseAttachment;

    AmiAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderComponent)
};
