/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 22 Jan 2023 7:35:43pm
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRComponent.h"

/*
  ==============================================================================


  //// ADSR and volume/pan controls ////


  ==============================================================================
*/

//==============================================================================
ADSRComponent::ADSRComponent(AmiSamplerAudioProcessor&p) : audioProcessor(p)
{
    /* Slider initialization // most of this implemented from The Audio Programmer's sampler tutorial */

    //Volume Slider
    mVolSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mVolSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(mVolSlider);
    
    mVolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "VOLUME", mVolSlider);

    mVolLabel.setText("V", juce::NotificationType::dontSendNotification);
    mVolLabel.setJustificationType(juce::Justification::centredLeft);
    mVolLabel.attachToComponent(&mVolSlider, true);

    //Pan Slider
    mPanSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mPanSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mPanSlider.snapValue(1.0f, juce::Slider::absoluteDrag);
    addAndMakeVisible(mPanSlider);

    mPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "PANNER", mPanSlider);

    mPanLabel.setText("P", juce::NotificationType::dontSendNotification);
    mPanLabel.setJustificationType(juce::Justification::centredLeft);
    mPanLabel.attachToComponent(&mPanSlider, true);

    //Attack Slider
    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(mAttackSlider);

    mAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "ATTACK", mAttackSlider);

    mAttackLabel.setText("A", juce::NotificationType::dontSendNotification);
    mAttackLabel.setJustificationType(juce::Justification::centredLeft);
    mAttackLabel.attachToComponent(&mAttackSlider, true);

    //Decay Slider
    mDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mDecaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(mDecaySlider);

    mDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "DECAY", mDecaySlider);

    mDecayLabel.setText("D", juce::NotificationType::dontSendNotification);
    mDecayLabel.setJustificationType(juce::Justification::centredRight);
    mDecayLabel.attachToComponent(&mDecaySlider, true);

    //Sustain Slider
    mSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mSustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mSustainSlider.setRange(0.0f, 1.0f, 0.33f);
    mSustainSlider.setValue(1.0f);
    addAndMakeVisible(mSustainSlider);

    mSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "SUSTAIN", mSustainSlider);

    mSustainLabel.setText("S", juce::NotificationType::dontSendNotification);
    mSustainLabel.setJustificationType(juce::Justification::centredLeft);
    mSustainLabel.attachToComponent(&mSustainSlider, true);

    //Release Slider
    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mReleaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(mReleaseSlider);

    mReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "RELEASE", mReleaseSlider);

    mReleaseLabel.setText("R", juce::NotificationType::dontSendNotification);
    mReleaseLabel.setJustificationType(juce::Justification::centredRight);
    mReleaseLabel.attachToComponent(&mReleaseSlider, true);

}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint (juce::Graphics& g)
{
    /* Slider label font */
    auto theFont = g.getCurrentFont();
    theFont.setHeight( (float)getWidth() / 26.0f);
    mVolLabel.setFont(theFont);
    mPanLabel.setFont(theFont);
    mAttackLabel.setFont(theFont);
    mDecayLabel.setFont(theFont);
    mSustainLabel.setFont(theFont);
    mReleaseLabel.setFont(theFont);

}

void ADSRComponent::resized()
{
    /* Slider positioning and sizing */

    const auto startX = 0.05f, startY = 0.0175f, sliderWidth = 0.167f, sliderHeight = 0.067f;

    mVolSlider.setBoundsRelative(startX, startY, sliderWidth, sliderHeight);
    mPanSlider.setBoundsRelative((startX + (sliderWidth * 1.3)), startY, sliderWidth, sliderHeight);
    mAttackSlider.setBoundsRelative(startX, (startY + (sliderHeight)), sliderWidth, sliderHeight);
    mDecaySlider.setBoundsRelative((startX + (sliderWidth * 1.3)), (startY + (sliderHeight * 1)), sliderWidth, sliderHeight);
    mSustainSlider.setBoundsRelative(startX, (startY + (sliderHeight * 2)), sliderWidth, sliderHeight);
    mReleaseSlider.setBoundsRelative((startX + (sliderWidth * 1.3)), (startY + (sliderHeight * 2)), sliderWidth, sliderHeight);
}
