/*
  ==============================================================================

    SliderComponent.cpp
    Created: 23 Dec 2023 11:49:24am
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "SliderComponent.h"

//==============================================================================
SliderComponent::SliderComponent(AmiAudioProcessor& p) : audioProcessor(p)
{
    setRepaintsOnMouseActivity(false);
    setInterceptsMouseClicks(false, true);

    initSliders();

    for (int i = 0; i < 12; i++)
    {
<<<<<<< HEAD
        audioProcessor.getAPVTS().addParameterListener("PAULA STEREO" + juce::String(i), this);
        audioProcessor.getAPVTS().addParameterListener("MONO POLY" + juce::String(i), this);
=======
        const juce::String plaAtch = "PAULA STEREO" + juce::String(i);
        audioProcessor.getAPVTS().addParameterListener(plaAtch, this);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    }
}

SliderComponent::~SliderComponent()
{
    for (int i = 0; i < 12; i++)
    {
<<<<<<< HEAD
        audioProcessor.getAPVTS().removeParameterListener("PAULA STEREO" + juce::String(i), this);
        audioProcessor.getAPVTS().removeParameterListener("MONO POLY" + juce::String(i), this);
    }

    sampleRateSlider.removeListener(this);
=======
        const juce::String plaAtch = "PAULA STEREO" + juce::String(i);
        audioProcessor.getAPVTS().removeParameterListener(plaAtch, this);
    }
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void SliderComponent::paint (juce::Graphics& g)
{
<<<<<<< HEAD
    if(hideChannelSliders) 
    {
        if(sampleRateSlider.getThumbBeingDragged() < 0)
            sampleRateSlider.setValue(audioProcessor.getSourceSampleRate(currentSample), juce::NotificationType::dontSendNotification);
    }
    else
    {
        g.setFont(getLookAndFeel().getLabelFont(downSampleLabel));
        g.setColour(juce::Colours::white);
        g.drawText("Fine Tune", juce::Rectangle(proportionOfWidth(0.72f), proportionOfHeight(0.684f), 
            proportionOfWidth(0.15f), proportionOfHeight(0.05f)), juce::Justification::centredTop, false);
    }
=======
    currentSample = audioProcessor.getCurrentSample();
    const juce::String currentSampleAtch = juce::String(currentSample);
    const juce::String currentSampleLbl = juce::String(currentSample + 1).paddedLeft('0', 2);

    g.setFont(getLookAndFeel().getLabelFont(downSampleLabel));
    
    if(currentSample != lastSample)
    {
        const bool stereoOn = audioProcessor.paulaStereoOn(currentSample);
        const juce::String chanPanLbl = !stereoOn ? "Chan " + currentSampleLbl + "\nPanning" :
            "Chan " + currentSampleLbl + "\nWidth";

        const juce::String chanPanAtch = !stereoOn ? "CHANNEL PAN" + currentSampleAtch :
            "CHANNEL WIDTH" + currentSampleAtch;

        channelPanAttachment.reset();
        channelPanLabel.setText(chanPanLbl, juce::NotificationType::dontSendNotification);
        channelPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.getAPVTS(), chanPanAtch, channelPanSlider);

        lastSample = currentSample;
    }

    const juce::String chanVolLbl = "Chan " + currentSampleLbl + "\nVolume";
    const juce::String chanVolAtch = "CHANNEL VOLUME" + currentSampleAtch;

    const juce::String snhAtch = "SAMP N HOLD" + currentSampleAtch;
    const juce::String glsAtch = "CHANNEL GLISS" + currentSampleAtch;
    const juce::String tuneAtch = "FINE TUNE" + currentSampleAtch;

    const juce::String attackAtch = "ATTACK" + currentSampleAtch;
    const juce::String decayAtch = "DECAY" + currentSampleAtch;
    const juce::String sustainAtch = "SUSTAIN" + currentSampleAtch;
    const juce::String releaseAtch = "RELEASE" + currentSampleAtch;

    channelVolAttachment.reset();

    downSampleAttachment.reset();
    glissandoAttachment.reset();

    fineTuneAttachment.reset();

    attackAttachment.reset();
    decayAttachment.reset();
    sustainAttachment.reset();
    releaseAttachment.reset();

    if (glissandoSlider.isEnabled() && audioProcessor.getSampler(currentSample).getNumVoices() > 1)
    {
        glissandoSlider.setEnabled(false);
        glissandoLabel.setColour(juce::Label::textColourId, juce::Colour(0xA0DDDDDD));
        glissandoSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightslategrey);
    }
    else if (!glissandoSlider.isEnabled() && audioProcessor.getSampler(currentSample).getNumVoices() == 1)
    {
        glissandoSlider.setEnabled(true);
        glissandoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        glissandoSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    }

    channelVolLabel.setText(chanVolLbl, juce::NotificationType::dontSendNotification);
    channelVolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), chanVolAtch, channelVolSlider);

    downSampleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), snhAtch, downSampleSlider);

    glissandoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), glsAtch, glissandoSlider);

    fineTuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), tuneAtch, fineTuneSlider);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), attackAtch, attackSlider);
    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), decayAtch, decaySlider);
    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), sustainAtch, sustainSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), releaseAtch, releaseSlider);

    g.setColour(juce::Colours::white);
    g.drawText("Fine Tune", juce::Rectangle(proportionOfWidth(0.72f), proportionOfHeight(0.684f), 
        proportionOfWidth(0.15f), proportionOfHeight(0.05f)), juce::Justification::centredTop, false);

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void SliderComponent::resized()
{
    masterVolSlider.setBoundsRelative(0.23f, 0.735f, 0.22f, 0.05f);
    masterPanSlider.setBoundsRelative(0.23f, 0.785f, 0.22f, 0.05f);

    vibeSpeedSlider.setBoundsRelative(0.51f, 0.735f, 0.19f, 0.05f);
    vibeIntstySlider.setBoundsRelative(0.51f, 0.785f, 0.19f, 0.05f);

    channelVolSlider.setBoundsRelative(0.06f, 0.513f, 0.15f, 0.05f);
    channelPanSlider.setBoundsRelative(0.06f, 0.563f, 0.15f, 0.05f);

    attackSlider.setBoundsRelative(0.3f, 0.513f, 0.15f, 0.05f);
    decaySlider.setBoundsRelative(0.3f, 0.563f, 0.15f, 0.05f);
    sustainSlider.setBoundsRelative(0.3f, 0.613f, 0.15f, 0.05f);
    releaseSlider.setBoundsRelative(0.3f, 0.663f, 0.15f, 0.05f);

    downSampleSlider.setBoundsRelative(0.52f, 0.513f, 0.15f, 0.05f);
    glissandoSlider.setBoundsRelative(0.52f, 0.563f, 0.15f, 0.05f);
<<<<<<< HEAD
    sampleRateSlider.setBoundsRelative(0.47f, 0.563f, 0.2f, 0.05f);
=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

    fineTuneSlider.setBoundsRelative(0.72f, 0.632f, 0.15f, 0.05f);
}

<<<<<<< HEAD
void SliderComponent::hideSliders(const bool hide)
{
    const bool show = !hide;

    hideChannelSliders = hide;

    channelVolSlider.setVisible(show);
    channelPanSlider.setVisible(show);
    
    attackSlider.setVisible(show);
    decaySlider.setVisible(show);
    sustainSlider.setVisible(show);
    releaseSlider.setVisible(show);

    downSampleSlider.setVisible(show);
    glissandoSlider.setVisible(show);

    fineTuneSlider.setVisible(show);

    sampleRateSlider.setVisible(hide);
    sampleRateSlider.setValue(audioProcessor.getSourceSampleRate(currentSample), juce::NotificationType::dontSendNotification);
}

void SliderComponent::changeSampleChannel(const int &channel)
{
    const juce::String currentSampleAtch = juce::String((currentSample = channel));
    const juce::String currentSampleLbl = juce::String(currentSample + 1).paddedLeft('0', 2);

    enableGliss(audioProcessor.getSampler(currentSample).getNumVoices() == 1);

    changePanWidth(audioProcessor.paulaStereoOn(currentSample), currentSampleAtch, currentSampleLbl);
    attachSlider(&channelVolSlider, &channelVolLabel, "Chan " + currentSampleLbl + "\nVolume", 
                "CHANNEL VOLUME" + currentSampleAtch, &channelVolAttachment);

    attachSlider(&downSampleSlider, "SAMP N HOLD" + currentSampleAtch, &downSampleAttachment);
    attachSlider(&glissandoSlider, "CHANNEL GLISS" + currentSampleAtch, &glissandoAttachment);
    attachSlider(&fineTuneSlider, "FINE TUNE" + currentSampleAtch, &fineTuneAttachment);
    
    attachSlider(&attackSlider, "ATTACK" + currentSampleAtch, &attackAttachment);
    attachSlider(&decaySlider, "DECAY" + currentSampleAtch, &decayAttachment);
    attachSlider(&sustainSlider, "SUSTAIN" + currentSampleAtch, &sustainAttachment);
    attachSlider(&releaseSlider, "RELEASE" + currentSampleAtch, &releaseAttachment);
}

void SliderComponent::enableGliss(const bool enable)
{
    glissandoSlider.setEnabled(enable);
    
    if (!glissandoSlider.isEnabled())
    {
        glissandoLabel.setColour(juce::Label::textColourId, juce::Colour(0xA0DDDDDD));
        glissandoSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightslategrey);
    }
    else
    {
        glissandoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
        glissandoSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    }
}

void SliderComponent::initSliders()
{
    initSlider(&masterVolSlider, &masterVolLabel, "Master\nVolume", "MASTER VOLUME", &masterVolAttachment);
    initSlider(&masterPanSlider, &masterPanLabel, "Master\nPanning", "MASTER PAN", &masterPanAttachment);
    initSlider(&vibeSpeedSlider, &vibeSpeedLabel, "Vibe\nSpeed", "VIBRATO SPEED", &vibeSpeedAttachment);
    initSlider(&vibeIntstySlider, &vibeIntstyLabel, "Vibe\nAmount", "VIBRATO INTENSITY", &vibeIntstyAttachment);
=======
void SliderComponent::initSliders()
{
    initSlider(&masterVolSlider, &masterVolLabel, "Master\nVolume");
    masterVolAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "MASTER VOLUME", masterVolSlider);

    initSlider(&masterPanSlider, &masterPanLabel, "Master\nPanning");
    masterPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "MASTER PAN", masterPanSlider);

    initSlider(&vibeSpeedSlider, &vibeSpeedLabel, "Vibe\nSpeed");
    vibeSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "VIBRATO SPEED", vibeSpeedSlider);

    initSlider(&vibeIntstySlider, &vibeIntstyLabel, "Vibe\nAmount");
    vibeIntstyAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.getAPVTS(), "VIBRATO INTENSITY", vibeIntstySlider);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

    initSlider(&channelVolSlider, &channelVolLabel, "");
    initSlider(&channelPanSlider, &channelPanLabel, "");

<<<<<<< HEAD
    initSlider(&downSampleSlider, &downSampleLabel, "Sample\n& Hold");
=======
    initSlider(&downSampleSlider, &downSampleLabel, "Down\nSample");
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    initSlider(&glissandoSlider, &glissandoLabel, "Glide");
    initSlider(&fineTuneSlider, nullptr, "");

    initSlider(&attackSlider, &attackLabel, "Attack");
    initSlider(&decaySlider, &decayLabel, "Decay");
    initSlider(&sustainSlider, &sustainLabel, "Sustain");
    initSlider(&releaseSlider, &releaseLabel, "Release");
<<<<<<< HEAD

    initSlider(&sampleRateSlider, &sampleRateLabel, "Chan Sample Rate");
    sampleRateLabel.attachToComponent(&sampleRateSlider, false);
    sampleRateLabel.setJustificationType(juce::Justification::centred);

    sampleRateSlider.addListener(this);
    sampleRateSlider.setRange(1000.0, 48000.0, 1.0);
    sampleRateSlider.setValue(audioProcessor.getSourceSampleRate(currentSample), juce::NotificationType::dontSendNotification);
    sampleRateSlider.setVisible(false);
=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void SliderComponent::initSlider(juce::Slider* s, juce::Label* l, const juce::String name)
{
    const float fontHeight = name.containsChar('\n') || name.isEmpty() ? 14.f : 16.f;

<<<<<<< HEAD
    jassert(s != nullptr);

=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    addAndMakeVisible(*s);
    s->setScrollWheelEnabled(false);
    s->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    if (l == nullptr) return;

    l->setFont(fontHeight);
    l->setJustificationType(juce::Justification::centredRight);
    l->attachToComponent(s, true);

    if (name.isEmpty()) return;

    l->setText(name, juce::NotificationType::dontSendNotification);
}

<<<<<<< HEAD
void SliderComponent::initSlider(juce::Slider *s, juce::Label *l, const juce::String &name, const juce::String &id, 
                                 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> *attachment)
{
    jassert(s != nullptr);

    addAndMakeVisible(*s);
    s->setScrollWheelEnabled(false);
    s->setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    s->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    attachSlider(s, l, name, id, attachment);
}


void SliderComponent::attachSlider(juce::Slider *s, juce::Label *l, const juce::String &name, const juce::String &id, 
                                   std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> *attachment)
{
    const float fontHeight = name.containsChar('\n') || name.isEmpty() ? 14.f : 16.f;

    attachSlider(s, id, attachment);

    if (l == nullptr) return;

    l->setFont(fontHeight);
    l->setJustificationType(juce::Justification::centredRight);
    l->attachToComponent(s, true);

    if (name.isEmpty()) return;

    l->setText(name, juce::NotificationType::dontSendNotification);
}

void SliderComponent::attachSlider(juce::Slider *s, const juce::String &id, 
                                    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> *attachment)
{
    jassert(s != nullptr);

    if(id.isEmpty() || attachment == nullptr) return;

    attachment->reset();
    *attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), id, *s);
}

void SliderComponent::sliderValueChanged(juce::Slider *slider)
{
    if(slider == &sampleRateSlider)
        audioProcessor.setSourceSampleRate(currentSample, sampleRateSlider.getValue());
}

void SliderComponent::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID.containsIgnoreCase("PAULA STEREO" + juce::String(currentSample)))
        changePanWidth(newValue > 0, juce::String(currentSample), juce::String(currentSample + 1).paddedLeft('0', 2));

    if (parameterID.containsIgnoreCase("MONO POLY" + juce::String(currentSample)))
        enableGliss((int) newValue == 1);
}

void SliderComponent::changePanWidth(const bool stereoOn, const juce::String& currentSampleAtch, const juce::String& currentSampleLbl)
{
    juce::String panLbl, panAtch;

    if (stereoOn)
    {
        panLbl  = "Chan " + currentSampleLbl + "\nWidth";
        panAtch = "CHANNEL WIDTH" + currentSampleAtch;
    }
    else
    {
        panLbl = "Chan " + currentSampleLbl + "\nPanning";
        panAtch = "CHANNEL PAN" + currentSampleAtch;
    }

    attachSlider(&channelPanSlider, &channelPanLabel, panLbl, panAtch, &channelPanAttachment);
=======
void SliderComponent::parameterChanged(const juce::String& parameterID, float newValue)
{
    const juce::String currentSampleAtch = juce::String(currentSample);
    const juce::String currentSampleLbl = juce::String(currentSample + 1).paddedLeft('0', 2);
    const juce::String plaAtch = "PAULA STEREO" + currentSampleAtch;

    if (parameterID.containsIgnoreCase(plaAtch))
    {
        const juce::String chanPanLbl = newValue == 0.0 ? "Chan " + currentSampleLbl + "\nPanning" :
                                                          "Chan " + currentSampleLbl + "\nWidth";

        const juce::String chanPanAtch = newValue == 0.0 ? "CHANNEL PAN"   + currentSampleAtch :
                                                           "CHANNEL WIDTH" + currentSampleAtch;

        channelPanAttachment.reset();
        channelPanLabel.setText(chanPanLbl, juce::NotificationType::dontSendNotification);
        channelPanAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
            (audioProcessor.getAPVTS(), chanPanAtch, channelPanSlider);
    }
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}
