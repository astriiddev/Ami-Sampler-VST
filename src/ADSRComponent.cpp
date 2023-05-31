/*
  ==============================================================================

    ADSRComponent.cpp
    Created: 22 Jan 2023 7:35:43pm
    Author:  finle

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ADSRComponent.h"

/*
  ==============================================================================


  //// ADSR and volume controls ////


  ==============================================================================
*/

//==============================================================================
ADSRComponent::ADSRComponent(AmiSamplerAudioProcessor&p) : audioProcessor(p)
{
    /* Slider initialization // most of this implemented from The Audio Programmer's sampler tutorial */

    //Volume Slider
    mVolSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mVolSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mVolSlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mVolSlider);
    
    mVolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "VOLUME", mVolSlider);

    mVolLabel.setText("V", juce::NotificationType::dontSendNotification);
    mVolLabel.setJustificationType(juce::Justification::centredLeft);
    mVolLabel.attachToComponent(&mVolSlider, true);
    mVolLabel.setMouseCursor(getMouseCursor().ParentCursor);

    //!!!!!!!! UNUSABLE RIGHT NOW!!!! NOT IMPLEMENTED !!!!!!!!//
    //!!!! TODO: literally the entire panning function !!!!//
    //???? Possible TODO: "stepping" panner option -- every other sampler voice will be L or R to simulate Amiga panning ????//
    //Pan Slider
    mPanSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mPanSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mPanSlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mPanSlider);

    mPanSlider.setRange(0.0f, 1.0f); mPanSlider.setValue(0.5f);
    mPanSlider.setEnabled(false);

    mPanLabel.setText("P", juce::NotificationType::dontSendNotification);
    mPanLabel.setJustificationType(juce::Justification::centredLeft);
    mPanLabel.attachToComponent(&mPanSlider, true);
    mPanLabel.setMouseCursor(getMouseCursor().ParentCursor);

    //Attack Slider
    mAttackSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mAttackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mAttackSlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mAttackSlider);

    mAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "ATTACK", mAttackSlider);

    mAttackLabel.setText("A", juce::NotificationType::dontSendNotification);
    mAttackLabel.setJustificationType(juce::Justification::centredLeft);
    mAttackLabel.attachToComponent(&mAttackSlider, true);
    mAttackLabel.setMouseCursor(getMouseCursor().ParentCursor);

    //Decay Slider
    mDecaySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mDecaySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mDecaySlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mDecaySlider);

    mDecayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "DECAY", mDecaySlider);

    mDecayLabel.setText("D", juce::NotificationType::dontSendNotification);
    mDecayLabel.setJustificationType(juce::Justification::centredRight);
    mDecayLabel.attachToComponent(&mDecaySlider, true);
    mDecayLabel.setMouseCursor(getMouseCursor().ParentCursor);

    //Sustain Slider
    mSustainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mSustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mSustainSlider.setRange(0.0f, 1.0f, 0.33f);
    mSustainSlider.setValue(1.0f);
    mSustainSlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mSustainSlider);

    mSustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "SUSTAIN", mSustainSlider);

    mSustainLabel.setText("S", juce::NotificationType::dontSendNotification);
    mSustainLabel.setJustificationType(juce::Justification::centredLeft);
    mSustainLabel.attachToComponent(&mSustainSlider, true);
    mSustainLabel.setMouseCursor(getMouseCursor().ParentCursor);

    //Release Slider
    mReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    mReleaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mReleaseSlider.setMouseCursor(getMouseCursor().ParentCursor);
    addAndMakeVisible(mReleaseSlider);

    mReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "RELEASE", mReleaseSlider);

    mReleaseLabel.setText("R", juce::NotificationType::dontSendNotification);
    mReleaseLabel.setJustificationType(juce::Justification::centredRight);
    mReleaseLabel.attachToComponent(&mReleaseSlider, true);
    mReleaseLabel.setMouseCursor(getMouseCursor().ParentCursor);

}

ADSRComponent::~ADSRComponent()
{
}

void ADSRComponent::paint (juce::Graphics& g)
{
    /* Slider label font */
    auto theFont = g.getCurrentFont();
    theFont.setHeight( getWidth() / 13.0f);
    mVolLabel.setFont(theFont);
    mPanLabel.setFont(theFont);
    mAttackLabel.setFont(theFont);
    mDecayLabel.setFont(theFont);
    mSustainLabel.setFont(theFont);
    mReleaseLabel.setFont(theFont);

    /* Basic windowing, will be changed to Workbench-styled window later */
    g.setColour(juce::Colour(0xFF00113D));
    g.drawLine(0, 0, getWidth(), 0, 4.0f * getDesktopScaleFactor());
    g.drawLine(getWidth(), 0, getWidth(), getHeight(), 4.0f * getDesktopScaleFactor());
    g.drawLine(0, proportionOfHeight(0.115f), getWidth(), proportionOfHeight(0.115f), 2.0f * getDesktopScaleFactor());
    g.drawLine(proportionOfWidth(0.475f), 0, proportionOfWidth(0.475f), getHeight(), 2.0f * getDesktopScaleFactor());

}

void ADSRComponent::paintOverChildren(juce::Graphics& g)
{
    /* Greys out pan slider, will be deleted when panning is implemented */
    g.setColour(juce::Colour(0xC0000000));
    g.fillRect(mPanSlider.getBounds());
}
void ADSRComponent::resized()
{
    /* Slider positioning and sizing */

    const auto startX = 0.1f, startY = 0.025f, sliderWidth = 0.33f, sliderHeight = 0.06f ,leftAdj = (startX * 6.0f);

    mVolSlider.setBoundsRelative(startX, startY, sliderWidth, sliderHeight);
    mPanSlider.setBoundsRelative((startX + (sliderWidth * 1.5)), startY, sliderWidth, sliderHeight);
    mAttackSlider.setBoundsRelative(startX, (startY + (sliderHeight * 2)), sliderWidth, sliderHeight);
    mDecaySlider.setBoundsRelative((startX + (sliderWidth * 1.5)), (startY + (sliderHeight * 2)), sliderWidth, sliderHeight);
    mSustainSlider.setBoundsRelative(startX, (startY + (sliderHeight * 3)), sliderWidth, sliderHeight);
    mReleaseSlider.setBoundsRelative((startX + (sliderWidth * 1.5)), (startY + (sliderHeight * 3)), sliderWidth, sliderHeight);

}
