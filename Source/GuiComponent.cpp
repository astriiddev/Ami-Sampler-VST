/*
  ==============================================================================

    GuiComponent.cpp
    Created: 7 Jun 2023 10:32:26am
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "GuiComponent.h"

#include "ami_palette.h"

//==============================================================================
GuiComponent::GuiComponent(AmiAudioProcessor& p) : handleSliders(p), audioProcessor(p)
{
    setInterceptsMouseClicks(false, false);
    setRepaintsOnMouseActivity(false);

    addAndMakeVisible(handleSliders);
<<<<<<< HEAD
=======
    addAndMakeVisible(muteBox);
    addAndMakeVisible(soloBox);

    muteBox.setButtonText("Mute");
    soloBox.setButtonText("Solo");
    
    muteBox.onClick = [&] {
        if (soloBox.getToggleState())
            audioProcessor.setSolo(currentSample, 0);
        audioProcessor.setMute(currentSample, !audioProcessor.isMuted(currentSample)); 
    };

    soloBox.onClick = [&] { 
        if(muteBox.getToggleState())
            audioProcessor.setMute(currentSample, 0);
        audioProcessor.setSolo(currentSample, !audioProcessor.isSoloed(currentSample)); 
    };

    addAndMakeVisible(monoBox);
    addAndMakeVisible(ptpolyBox);
    addAndMakeVisible(octapolyBox);
    
    monoBox.setButtonText("Mono");
    ptpolyBox.setButtonText("PT Poly");
    octapolyBox.setButtonText("Octa Poly");

    monoBox.onClick = [&] 
    {
        audioProcessor.setMonoPoly(currentSample, 1); 
        if(audioProcessor.paulaStereoOn(currentSample)) paulaStereo.triggerClick();
    };
    
    ptpolyBox.onClick = [&] {audioProcessor.setMonoPoly(currentSample, 2); };
    octapolyBox.onClick = [&] {audioProcessor.setMonoPoly(currentSample, 3); };

    monoBox.setRadioGroupId(1001, juce::NotificationType::dontSendNotification);
    ptpolyBox.setRadioGroupId(1001, juce::NotificationType::dontSendNotification);
    octapolyBox.setRadioGroupId(1001, juce::NotificationType::dontSendNotification);

    octapolyBox.setToggleState(true, juce::NotificationType::dontSendNotification);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

    initButton(&enableLoop, true);
    initButton(&paulaStereo, true);

<<<<<<< HEAD
    enableLoop.setClickingTogglesState(true);
    paulaStereo.setClickingTogglesState(true);

    initAllLabels();
    initAllCheckboxes();
    
    for (int i = 0; i < 12; i++)
    {
        audioProcessor.getAPVTS().addParameterListener("LOOP ENABLE" + juce::String(i), this);
        audioProcessor.getAPVTS().addParameterListener("LOOP START" + juce::String(i), this);
        audioProcessor.getAPVTS().addParameterListener("LOOP END" + juce::String(i), this);
        
        audioProcessor.getAPVTS().addParameterListener("SAMPLE ROOT NOTE" + juce::String(i), this);
        audioProcessor.getAPVTS().addParameterListener("SAMPLE LOW NOTE" + juce::String(i), this);
        audioProcessor.getAPVTS().addParameterListener("SAMPLE HIGH NOTE" + juce::String(i), this);
    }
}

GuiComponent::~GuiComponent() 
{
    for (int i = 0; i < 12; i++)
    {
        audioProcessor.getAPVTS().removeParameterListener("LOOP ENABLE" + juce::String(i), this);
        audioProcessor.getAPVTS().removeParameterListener("LOOP START" + juce::String(i), this);
        audioProcessor.getAPVTS().removeParameterListener("LOOP END" + juce::String(i), this);
        
        audioProcessor.getAPVTS().removeParameterListener("SAMPLE ROOT NOTE" + juce::String(i), this);
        audioProcessor.getAPVTS().removeParameterListener("SAMPLE LOW NOTE" + juce::String(i), this);
        audioProcessor.getAPVTS().removeParameterListener("SAMPLE HIGH NOTE" + juce::String(i), this);
    }
=======
    initButton(&modelType, false);
    initButton(&ledFilter, false);

    ledFilter.onClick = [&] { repaint(ledRectangle); };

    modelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "MODEL TYPE", modelType);

    ledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "LED FILTER", ledFilter);
        
    initLabel(&startLoopText,true);
    startLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    startLoopText.onTextChange = [&]
    {
            const juce::String starttxt = startLoopText.getText().toUpperCase();

            if (starttxt.isEmpty()) return;
            if (starttxt.containsOnly("0123456789ABCDEF"))
            {
                const int startLoop = starttxt.getHexValue32();

                if (startLoop < 0) return;
                if (startLoop >= audioProcessor.getLoopEnd(currentSample)) return;

                audioProcessor.setLoopStart(currentSample, startLoop);
                startLoopText.setText(starttxt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
            }
    };

    /* End loop text box customization */

    initLabel(&endLoopText,true);
    endLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    endLoopText.onTextChange = [&]
    {
        const juce::String endtxt = endLoopText.getText().toUpperCase();

        if (endtxt.isEmpty()) return;
        if (endtxt.containsOnly("0123456789ABCDEF"))
        {
            const int endLoop = endtxt.getHexValue32();

            if (endLoop <= audioProcessor.getLoopStart(currentSample)) return;
            if (endLoop > audioProcessor.getWaveForm(currentSample).getNumSamples()) return;

            audioProcessor.setLoopEnd(currentSample, endLoop);
            endLoopText.setText(endtxt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
        }
    };

    initLabel(&replenLoopText,true);
    replenLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    replenLoopText.onTextChange = [&]
    {
        const juce::String replentxt = replenLoopText.getText().toUpperCase();

        if (replentxt.isEmpty()) return;
        if (replentxt.containsOnly("0123456789ABCDEF"))
        {
            const int endLoop = replentxt.getHexValue32() + startLoopText.getText().getHexValue32();

            if (endLoop <= audioProcessor.getLoopStart(currentSample)) return;
            if (endLoop > audioProcessor.getWaveForm(currentSample).getNumSamples()) return;

            audioProcessor.setLoopEnd(currentSample, endLoop);
            replenLoopText.setText(replentxt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
        }
    };

    initLabel(&sampleMidiChannel, false);
    sampleMidiChannel.onTextChange = [&]
    {
        if (sampleMidiChannel.getText().isEmpty()) return;

        if(sampleMidiChannel.getText().containsOnly("0123456789"))
        {
            const int channel = sampleMidiChannel.getText().getIntValue();
            if (channel < 0) return;
            if (channel > 16) return;

            audioProcessor.setMidiChannel(currentSample, channel);
        }
        else if(sampleMidiChannel.getText().compareIgnoreCase("ALL") == 0)
            audioProcessor.setMidiChannel(currentSample, 0);

    };

    initLabel(&midiRootNote, false);
    midiRootNote.onTextChange = [&]
    {
        juce::String rootNote = midiRootNote.getText().toUpperCase();

        if (rootNote.isEmpty()) return;

        if (rootNote.containsOnly("0123456789"))
        {
            const int note = rootNote.getIntValue();
            if (note < 0) return;
            if (note > 127) return;

            audioProcessor.setRootNote(currentSample, note);
        }
        else if (rootNote.containsOnly("0123456789ABCDEFG-#"))
        {
            for (int i = 0; i < 128; i++)
                if (notes[i].compare(rootNote) == 0)
                {
                    audioProcessor.setRootNote(currentSample, i);
                    break;
                }
        }

    };

    initLabel(&midiLowNote, false);
    midiLowNote.onTextChange = [&]
    {
        juce::String lowNote = midiLowNote.getText().toUpperCase();

        if (lowNote.isEmpty()) return;

        if (lowNote.containsOnly("0123456789"))
        {
            const int note = lowNote.getIntValue();
            if (note < 0) return;
            if (note > 127) return;

            audioProcessor.setLowNote(currentSample, note);
        }
        else if (lowNote.containsOnly("0123456789ABCDEFG-#"))
        {
            for (int i = 0; i < 128; i++)
                if (notes[i].compare(lowNote) == 0)
                {
                    audioProcessor.setLowNote(currentSample, i);
                    break;
                }
        }
    };

    initLabel(&midiHiNote, false);
    midiHiNote.onTextChange = [&]
    {
        juce::String highNote = midiHiNote.getText().toUpperCase();

        if (highNote.isEmpty()) return;

        if (highNote.containsOnly("0123456789"))
        {
            const int note = highNote.getIntValue();
            if (note < 0) return;
            if (note > 127) return;

            audioProcessor.setHighNote(currentSample, note);
        }
        else if (highNote.containsOnly("0123456789ABCDEFG-#"))
        {
            for (int i = 0; i < 128; i++)
                if (notes[i].compare(highNote) == 0)
                {
                    audioProcessor.setHighNote(currentSample, i);
                    break;
                }
        }
    };
}

GuiComponent::~GuiComponent()
{
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void GuiComponent::paint (juce::Graphics& g)
{
<<<<<<< HEAD
    if (audioProcessor.getWaveForm(currentSample).getNumSamples() < 1)
        audioProcessor.setLoopEnable(currentSample, false);
    else if(!enableLoop.isEnabled()) enableLoop.setEnabled(true);

    ledRectangle = juce::Rectangle<int>(proportionOfWidth(0.905f), proportionOfHeight(0.63f), proportionOfWidth(0.07f), proportionOfHeight(0.02f));

    ledFilter.getToggleState() ? g.setColour(juce::Colours::red) : g.setColour(juce::Colours::darkred);
 
=======
    currentSample = audioProcessor.getCurrentSample();

    const juce::String lpAtch  = "LOOP ENABLE"  + juce::String(currentSample);
    const juce::String plaAtch = "PAULA STEREO" + juce::String(currentSample);

    const int loopStart = audioProcessor.getLoopStart(currentSample);
    const int loopEnd = audioProcessor.getLoopEnd(currentSample);

    const int numVoices = audioProcessor.getSampler(currentSample).getNumVoices();

    const int loopRpln = loopEnd - loopStart;
    const int midiChannel = audioProcessor.getMidiChannel(currentSample);

    ledRectangle = juce::Rectangle<int>(proportionOfWidth(0.905f), proportionOfHeight(0.63f), proportionOfWidth(0.07f), proportionOfHeight(0.02f));

    if(lastSample != currentSample)
    {
        loopAttachment.reset();
        paulaAttachment.reset();

        loopAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.getAPVTS(), lpAtch, enableLoop);

        paulaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
            (audioProcessor.getAPVTS(), plaAtch, paulaStereo);

        lastSample = currentSample;
    }

    if (audioProcessor.getWaveForm(currentSample).getNumSamples() < 1)
    {
        audioProcessor.setLoopEnable(currentSample, false);
        enableLoop.setEnabled(false);
    }
    else if(!enableLoop.isEnabled()) enableLoop.setEnabled(true);

    if (enableLoop.getToggleState() && !startLoopText.isEnabled())
    {
        startLoopText.setEnabled(true);
        endLoopText.setEnabled(true);
        replenLoopText.setEnabled(true);
    }
    else if (!enableLoop.getToggleState() && startLoopText.isEnabled())
    {
        startLoopText.setEnabled(false);
        endLoopText.setEnabled(false);
        replenLoopText.setEnabled(false);
    }

    if (paulaStereo.isEnabled() && numVoices <= 1)
    {
        if (paulaStereo.getToggleState()) paulaStereo.setToggleState(false, juce::NotificationType::dontSendNotification);
        paulaStereo.setEnabled(false);
    }
    else if (!paulaStereo.isEnabled() && numVoices > 1)
        paulaStereo.setEnabled(true);

    if (numVoices == 1 && !monoBox.getToggleState()) monoBox.setToggleState(true, juce::NotificationType::dontSendNotification);
    if (numVoices == 4 && !ptpolyBox.getToggleState()) ptpolyBox.setToggleState(true, juce::NotificationType::dontSendNotification);
    if (numVoices == 8 && !octapolyBox.getToggleState()) octapolyBox.setToggleState(true, juce::NotificationType::dontSendNotification);

    muteBox.setToggleState(audioProcessor.isMuted(currentSample), juce::NotificationType::dontSendNotification);
    soloBox.setToggleState(audioProcessor.isSoloed(currentSample), juce::NotificationType::dontSendNotification);

    ledFilter.getToggleState() == true ? g.setColour(juce::Colours::red) : g.setColour(juce::Colours::darkred);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    g.fillRect(ledRectangle);

    g.setColour(juce::Colours::lime);
    g.fillRect(proportionOfWidth(0.905f), proportionOfHeight(0.67f), proportionOfWidth(0.07f), proportionOfHeight(0.02f));

    g.setFont(getLookAndFeel().getLabelFont(startLoopText).withHeight(18.f));
    g.setColour(JPAL(AMI_WHT));

<<<<<<< HEAD
    g.drawText("Filter", juce::Rectangle<int>(proportionOfWidth(0.72f), proportionOfHeight(0.74f),
        proportionOfWidth(0.05f), (int) g.getCurrentFont().getHeight()), juce::Justification::centred, false);

=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    g.drawText("LOOP  START:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.72f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("LOOP REPEAT:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.75f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("LOOP REPLEN:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.78f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);

<<<<<<< HEAD
    if(showExtendedOptions) 
    {
        g.drawText("Text Val Type:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.51f),
        proportionOfWidth(0.15f), proportionOfHeight(0.03f)), juce::Justification::centredLeft, false);
        
        g.drawText("ASCII To MIDI:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.55f),
        proportionOfWidth(0.15f), proportionOfHeight(0.03f)), juce::Justification::centredLeft, false);
        
        g.setFont(20.f);
        g.drawText("CHAN SAMPLE RATE:", juce::Rectangle<int>(proportionOfWidth(0.47f), proportionOfHeight(0.615f),
        proportionOfWidth(0.2f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);

        g.drawText("RESAMPLING  RATE:", juce::Rectangle<int>(proportionOfWidth(0.47f), proportionOfHeight(0.655f),
        proportionOfWidth(0.2f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);

        g.setFont(14.f);
        g.drawText("Loop Type:", juce::Rectangle<int>(proportionOfWidth(0.145f), proportionOfHeight(0.6f),
        proportionOfWidth(0.2f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);

        automateLabelText(&sampleRateText, (int) audioProcessor.getSourceSampleRate(currentSample));
        automateLabelText(&resampleRateText, (int) audioProcessor.getResampleRate(currentSample));

        return;
    }
    
=======
    automateLoopText(&startLoopText, loopStart);
    automateLoopText(&endLoopText, loopEnd);
    automateLoopText(&replenLoopText, loopRpln);

    if(!sampleMidiChannel.isBeingEdited())
    {
        if (midiChannel > 0) sampleMidiChannel.setText(juce::String(midiChannel).paddedLeft('0', 2), juce::NotificationType::dontSendNotification);
        else sampleMidiChannel.setText("ALL", juce::NotificationType::dontSendNotification);
    }

    if (!midiRootNote.isBeingEdited())
        midiRootNote.setText(notes[audioProcessor.getRootNote(currentSample)], juce::NotificationType::dontSendNotification);
    if (!midiLowNote.isBeingEdited())
        midiLowNote.setText(notes[audioProcessor.getLowNote(currentSample)], juce::NotificationType::dontSendNotification);
    if (!midiHiNote.isBeingEdited())
        midiHiNote.setText(notes[audioProcessor.getHighNote(currentSample)], juce::NotificationType::dontSendNotification);

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    g.setFont(20.f);

    g.drawText("MIDI", juce::Rectangle<int>(proportionOfWidth(0.458f), proportionOfHeight(0.64f),
        proportionOfWidth(0.05f), proportionOfHeight(0.03f)), juce::Justification::centredRight, false);

    g.setFont(16.f);
    g.drawText("Chan", juce::Rectangle<int>(proportionOfWidth(0.508f), proportionOfHeight(0.675f),
        proportionOfWidth(0.05f), proportionOfHeight(0.05f)), juce::Justification::centred, false);

    g.drawText("Base", juce::Rectangle<int>(proportionOfWidth(0.548f), proportionOfHeight(0.675f),
        proportionOfWidth(0.05f), proportionOfHeight(0.05f)), juce::Justification::centred, false);

    g.drawText("Low", juce::Rectangle<int>(proportionOfWidth(0.588f), proportionOfHeight(0.675f),
        proportionOfWidth(0.05f), proportionOfHeight(0.05f)), juce::Justification::centred, false);

    g.drawText("High", juce::Rectangle<int>(proportionOfWidth(0.628f), proportionOfHeight(0.675f),
        proportionOfWidth(0.05f), proportionOfHeight(0.05f)), juce::Justification::centred, false);
}

void GuiComponent::paintOverChildren(juce::Graphics& g)
{
<<<<<<< HEAD
=======
    g.setColour(JPAL(AMI_BLD));
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), sampleMidiChannel.getBounds().getWidth(), 1);
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), 1, sampleMidiChannel.getBounds().getHeight());

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), midiRootNote.getBounds().getWidth(), 1);
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), 1, midiRootNote.getBounds().getHeight());
    
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    g.setColour(JPAL(AMI_ORG));
    g.fillRect(windowRect[0]);

    g.setColour(JPAL(AMI_WHT));

    for(int i = 1; i < 5; i++) g.fillRect(windowRect[i]);
    
    g.setColour(JPAL(AMI_BLU));
    for(int i = 5; i < 9; i++) g.fillRect(windowRect[i]);
    
    g.setColour(JPAL(AMI_ORG));
    g.fillRect(windowRect[9]);

    g.setColour(JPAL(AMI_BLK));
    for(int i = 10; i < 12; i++) g.fillRect(windowRect[i]);

    g.setColour(JPAL(AMI_WHT));
    for(int i = 12; i < 19; i++) g.fillRect(windowRect[i]);

    g.setColour(JPAL(AMI_BLU));
    for(int i = 19; i < 21; i++) g.drawRect(windowRect[i], 4);
<<<<<<< HEAD

    if(showExtendedOptions) return;

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), sampleMidiChannel.getBounds().getWidth(), 1);
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), 1, sampleMidiChannel.getBounds().getHeight());

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), midiRootNote.getBounds().getWidth(), 1);
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), 1, midiRootNote.getBounds().getHeight());
=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void GuiComponent::resized()
{
    handleSliders.setBoundsRelative(0.f, 0.f, 1.f, 1.f);

<<<<<<< HEAD
    paulaStereo.setBoundsRelative(0.008f, 0.617f, 0.075f, 0.051f);
    enableLoop.setBoundsRelative(0.008f, 0.665f, 0.075f, 0.051f);
    
=======
    paulaStereo.setBoundsRelative(0.008f, 0.615f, 0.075f, 0.05f);
    enableLoop.setBoundsRelative(0.008f, 0.664f, 0.075f, 0.05f);

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    monoBox.setBoundsRelative(0.085f, 0.615f, 0.05f, 0.03f);
    ptpolyBox.setBoundsRelative(0.085f, 0.65f, 0.07f, 0.03f);
    octapolyBox.setBoundsRelative(0.085f, 0.685f, 0.08f, 0.03f);

    muteBox.setBoundsRelative(0.165f, 0.628f, 0.05f, 0.03f);
    soloBox.setBoundsRelative(0.165f, 0.673f, 0.05f, 0.03f);

<<<<<<< HEAD
    modelType.setBoundsRelative(0.775f, 0.73f, 0.075f, 0.05f);
    ledFilter.setBoundsRelative(0.83f,  0.73f, 0.075f, 0.05f);
=======
    modelType.setBoundsRelative(0.72f, 0.73f, 0.15f, 0.06f);
    ledFilter.setBoundsRelative(0.72f, 0.785f, 0.15f, 0.06f);

    startLoopText.setBoundsRelative(0.08f, 0.735f, 0.11f, 0.04f);
    endLoopText.setBoundsRelative(0.08f, 0.765f, 0.11f, 0.04f);
    replenLoopText.setBoundsRelative(0.08f, 0.795f, 0.11f, 0.04f);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

    sampleMidiChannel.setBoundsRelative(0.515f, 0.63f, 0.035f, 0.05f);
    midiRootNote.setBoundsRelative(0.555f, 0.63f, 0.035f, 0.05f);
    midiLowNote.setBoundsRelative(0.595f, 0.63f, 0.035f, 0.05f);
    midiHiNote.setBoundsRelative(0.635f, 0.63f, 0.035f, 0.05f);
<<<<<<< HEAD
    
    startLoopText.setBoundsRelative(0.1f, 0.735f, 0.07f, 0.04f);
    endLoopText.setBoundsRelative(0.1f, 0.765f, 0.07f, 0.04f);
    replenLoopText.setBoundsRelative(0.1f, 0.795f, 0.07f, 0.04f);

    sampleRateText.setBoundsRelative(0.62f, 0.63f, 0.07f, 0.04f);
    resampleRateText.setBoundsRelative(0.62f, 0.67f, 0.07f, 0.04f);

    decText.setBoundsRelative(0.12f, 0.51f, 0.05f, 0.03f);
    hexText.setBoundsRelative(0.17f, 0.51f, 0.05f, 0.03f);

    enableAscii2Note.setBoundsRelative(0.12f, 0.55f, 0.1f, 0.03f);

    forwardLoop.setBoundsRelative(0.14f, 0.655f, 0.1f, 0.03f);
    pingpongLoop.setBoundsRelative(0.14f, 0.685f, 0.1f, 0.03f);
=======
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void GuiComponent::mouseDown(const juce::MouseEvent& e)
{
    if (enableLoop.getBounds().contains(e.getMouseDownPosition())) repaint();
}

void GuiComponent::mouseUp(const juce::MouseEvent& e)
{
    if (enableLoop.getBounds().contains(e.getMouseDownPosition())) repaint();
}

<<<<<<< HEAD
void GuiComponent::showMoreOptions(const bool& show)
{
    showExtendedOptions = show; 

    hideMainOptions(); 
    hideMoreOptions();
    
    if(audioProcessor.getPingPongLoop(currentSample))
    {
        pingpongLoop.setToggleState(true, juce::NotificationType::dontSendNotification);
        forwardLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
    }
    else
    {
        pingpongLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
        forwardLoop.setToggleState(true, juce::NotificationType::dontSendNotification);
    }
}

void GuiComponent::changeSampleChannel(const int &channel)
{
    const int numVoices = audioProcessor.getSampler((currentSample = channel)).getNumVoices();
    const int midiChannel = audioProcessor.getMidiChannel(currentSample);

    const int loopStart = audioProcessor.getLoopStart(currentSample);
    const int loopEnd = audioProcessor.getLoopEnd(currentSample);
    const int loopRpln = loopEnd - loopStart;

    const bool loopEnabled = audioProcessor.getLoopEnable(currentSample) && audioProcessor.getWaveForm(currentSample).getNumSamples() > 1;

    automateLabelText(&startLoopText, loopStart);
    automateLabelText(&endLoopText, loopEnd);
    automateLabelText(&replenLoopText, loopRpln);

    enableLoop.setToggleState(loopEnabled, juce::NotificationType::dontSendNotification);
    enableLoop.setEnabled(loopEnabled);

    startLoopText.setEnabled(loopEnabled);
    endLoopText.setEnabled(loopEnabled);
    replenLoopText.setEnabled(loopEnabled);

    handleSliders.changeSampleChannel(channel);
    
    if (midiChannel > 0) sampleMidiChannel.setText(juce::String(midiChannel).paddedLeft('0', 2), juce::NotificationType::dontSendNotification);
    else sampleMidiChannel.setText("ALL", juce::NotificationType::dontSendNotification);

    midiRootNote.setText(notes[audioProcessor.getRootNote(currentSample)], juce::NotificationType::dontSendNotification);
    midiLowNote.setText(notes[audioProcessor.getLowNote(currentSample)], juce::NotificationType::dontSendNotification);
    midiHiNote.setText(notes[audioProcessor.getHighNote(currentSample)], juce::NotificationType::dontSendNotification);

    if(audioProcessor.getPingPongLoop(currentSample))
    {
        pingpongLoop.setToggleState(true, juce::NotificationType::dontSendNotification);
        forwardLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
    }
    else
    {
        pingpongLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
        forwardLoop.setToggleState(true, juce::NotificationType::dontSendNotification);
    }

    if (paulaStereo.isEnabled() && numVoices <= 1)
    {
        if (paulaStereo.getToggleState()) paulaStereo.setToggleState(false, juce::NotificationType::dontSendNotification);
        paulaStereo.setEnabled(false);
    }
    else if (!paulaStereo.isEnabled() && numVoices > 1)
        paulaStereo.setEnabled(true);

    if (numVoices == 1 && !monoBox.getToggleState()) monoBox.setToggleState(true, juce::NotificationType::dontSendNotification);
    if (numVoices == 4 && !ptpolyBox.getToggleState()) ptpolyBox.setToggleState(true, juce::NotificationType::dontSendNotification);
    if (numVoices == 8 && !octapolyBox.getToggleState()) octapolyBox.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    muteBox.setToggleState(audioProcessor.isMuted(currentSample), juce::NotificationType::dontSendNotification);
    soloBox.setToggleState(audioProcessor.isSoloed(currentSample), juce::NotificationType::dontSendNotification);

    loopAttachment.reset();
    paulaAttachment.reset();

    loopAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "LOOP ENABLE"  + juce::String(channel), enableLoop);

    paulaAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "PAULA STEREO" + juce::String(channel), paulaStereo);
}

void GuiComponent::visibilityChanged()
{
    if(audioProcessor.getAPVTS().state.getProperty("ascii2note").isVoid())
        audioProcessor.getAPVTS().state.setProperty("ascii2note", true, nullptr);

    showExtendedOptions = audioProcessor.extendedOptionsShowing();
    
    changeSampleChannel(audioProcessor.getCurrentSample());
    
    hideMainOptions();
    hideMoreOptions();

    textInHex = audioProcessor.showHexVal();
    decText.setToggleState(!textInHex, juce::NotificationType::dontSendNotification);
    hexText.setToggleState(textInHex,  juce::NotificationType::dontSendNotification);

    enableAscii2Note.setToggleState(audioProcessor.getAPVTS().state.getProperty("ascii2note").operator bool(), juce::NotificationType::dontSendNotification);
}

void GuiComponent::buttonClicked(juce::Button *button)
{
    if(button == &decText)
    {
        const int loopStart = audioProcessor.getLoopStart(currentSample);
        const int loopEnd = audioProcessor.getLoopEnd(currentSample);

        audioProcessor.switchValType((textInHex = false)); 

        automateLabelText(&startLoopText, loopStart);
        automateLabelText(&endLoopText, loopEnd);
        automateLabelText(&replenLoopText, loopEnd - loopStart);

        repaint(); 
    };
    
    if(button == &hexText)
    {
        const int loopStart = audioProcessor.getLoopStart(currentSample);
        const int loopEnd = audioProcessor.getLoopEnd(currentSample);

        audioProcessor.switchValType((textInHex = true)); 

        automateLabelText(&startLoopText, loopStart);
        automateLabelText(&endLoopText, loopEnd);
        automateLabelText(&replenLoopText, loopEnd - loopStart);

        repaint(); 
    };
    
    if(button == &muteBox)
    {
        if (soloBox.getToggleState()) 
        {
            audioProcessor.setSolo(currentSample, false);
            soloBox.setToggleState(0, juce::NotificationType::dontSendNotification);
        }
        audioProcessor.setMute(currentSample, !audioProcessor.isMuted(currentSample)); 
    };

    if(button == &soloBox)
    {
        if(muteBox.getToggleState()) 
        {
            audioProcessor.setMute(currentSample, false);
            muteBox.setToggleState(false, juce::NotificationType::dontSendNotification);
        }
        audioProcessor.setSolo(currentSample, !audioProcessor.isSoloed(currentSample)); 
    };
}

void GuiComponent::parameterChanged(const juce::String &parameterID, float newValue)
{
    if (parameterID.contains("LOOP ENABLE" + juce::String(currentSample)))
    {
        const bool enabled = newValue > 0;

        enableLoop.setToggleState(enabled, juce::NotificationType::dontSendNotification);

        startLoopText.setEnabled(enabled);
        endLoopText.setEnabled(enabled);
        replenLoopText.setEnabled(enabled);
    }
    
    if (parameterID.contains("LOOP START" + juce::String(currentSample)))
    {
        const int loopRpln = audioProcessor.getLoopEnd(currentSample) - (int) newValue;

        automateLabelText(&startLoopText, (int) newValue);
        automateLabelText(&replenLoopText, loopRpln);
    }

    if (parameterID.contains("LOOP END" + juce::String(currentSample)))
    {
        const int loopRpln = (int) newValue - audioProcessor.getLoopStart(currentSample);

        automateLabelText(&endLoopText, (int) newValue);
        automateLabelText(&replenLoopText, loopRpln);
    }

    if (parameterID.contains("SAMPLE ROOT NOTE" + juce::String(currentSample)))
    {
        const int midiNote = newValue < 0 ? 0 : newValue > 127 ? 127 : (int) newValue;
        midiRootNote.setText(notes[midiNote], juce::NotificationType::dontSendNotification);
    }

    if (parameterID.contains("SAMPLE LOW NOTE" + juce::String(currentSample)))
    {
        const int midiNote = newValue < 0 ? 0 : newValue > 127 ? 127 : (int) newValue;
        midiHiNote.setText(notes[midiNote], juce::NotificationType::dontSendNotification);
    }

    if (parameterID.contains("SAMPLE HIGH NOTE" + juce::String(currentSample)))
    {
        const int midiNote = newValue < 0 ? 0 : newValue > 127 ? 127 : (int) newValue;
        midiRootNote.setText(notes[midiNote], juce::NotificationType::dontSendNotification);
    }
}

void GuiComponent::initLabel(juce::Label *l, const bool clearBack)
{
    const juce::Colour backColour = clearBack ? JPAL(0) : JPAL(0xFF00113D);

    jassert(l != nullptr);

=======
void GuiComponent::initLabel(juce::Label* l, const bool clearBack)
{
    /* Start loop text box customization */
    const juce::Colour backColour = clearBack ? JPAL(0) : JPAL(0xFF00113D);

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    addAndMakeVisible(*l);
    l->setFont(20.f);
    l->setRepaintsOnMouseActivity(false);

    l->setColour(juce::Label::backgroundColourId, backColour);
    l->setColour(juce::Label::textColourId, JPAL(AMI_WHT));
    l->setColour(juce::Label::textWhenEditingColourId, JPAL(AMI_WHT));
    l->setColour(juce::Label::backgroundWhenEditingColourId, backColour);
    l->setColour(juce::Label::ColourIds::outlineWhenEditingColourId, JPAL(0));

<<<<<<< HEAD
    l->setEditable(true);
    l->setEnabled(true);
    l->setJustificationType(juce::Justification::centred);
    l->setBorderSize(juce::BorderSize<int>(0));
=======
    l->setEditable(true, false, false);
    l->setEnabled(true);
    l->setJustificationType(juce::Justification::centred);
    l->setBorderSize(juce::BorderSize < int>(0));
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void GuiComponent::initButton(juce::Button* b, const bool redWhenDown)
{
    const juce::Colour boxColour = JPAL(AMI_GRY);
    const juce::Colour downColour = redWhenDown ? JPAL(AMI_RED) : boxColour;
    const juce::Colour downTxtColour = redWhenDown ? JPAL(AMI_WHT) : JPAL(AMI_BLK);

<<<<<<< HEAD
    jassert(b != nullptr);

    addAndMakeVisible(*b);
    b->addListener(this);

    b->setRepaintsOnMouseActivity(false);

=======
    addAndMakeVisible(*b);
    b->setRepaintsOnMouseActivity(false);
    b->setClickingTogglesState(true);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    b->setColour(juce::TextButton::buttonColourId, boxColour);
    b->setColour(juce::TextButton::buttonOnColourId, downColour);
    b->setColour(juce::TextButton::textColourOffId, JPAL(AMI_BLK));
    b->setColour(juce::TextButton::textColourOnId, downTxtColour);
}

<<<<<<< HEAD
void GuiComponent::initCheckBox(juce::Button *b, const juce::String &name, const int groupID)
{
    jassert(b != nullptr && name.isNotEmpty());

    addAndMakeVisible(*b);
    b->addListener(this);

    b->setButtonText(name);
    b->setToggleState(true, juce::NotificationType::dontSendNotification);

    if(groupID > 0) b->setRadioGroupId(groupID, juce::NotificationType::dontSendNotification);
}

void GuiComponent::changeLoopText(juce::Label* l, const int type)
{
    juce::String txt;
    int val = 0;
    
    jassert(l != nullptr && type < 3);

    txt = l->getText().toUpperCase();

    if (txt.isEmpty()) return;
    if (!txt.containsOnly("0123456789ABCDEF") || (!textInHex && !txt.containsOnly("0123456789"))) return;

    val = textInHex ? txt.getHexValue32() : txt.getIntValue();

    switch(type)
    {
    case 0:

        if (val < 0) return;
        if (val >= audioProcessor.getLoopEnd(currentSample)) return;

        audioProcessor.setLoopStart(currentSample, val);

        break;
    
    case 1:

        val += textInHex ? startLoopText.getText().getHexValue32() : startLoopText.getText().getIntValue();
        [[fallthrough]];

    case 2:

        if (val <= audioProcessor.getLoopStart(currentSample)) return;
        if (val > audioProcessor.getWaveForm(currentSample).getNumSamples()) return;

        audioProcessor.setLoopEnd(currentSample, val);

        break;
    }
 
    l->setText(txt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
}

void GuiComponent::changeRateText(juce::Label *l)
{
    juce::String txt;
    int val = 0;
    
    jassert(l != nullptr);

    txt = l->getText().toUpperCase();

    if (txt.isEmpty()) return;
    if (!txt.containsOnly("0123456789ABCDEF") /* || (!textInHex && !txt.contains("0123456789")) */) return;

    val = textInHex ? txt.getHexValue32() : txt.getIntValue();

    if (val <= 0) return;
    if (val >= 1000000) return;

    audioProcessor.setSourceSampleRate(currentSample, val);
    l->setText(txt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
}

void GuiComponent::changeMidiNoteText(juce::Label *l, const int type)
{
    juce::String txt;
    int note = 0;
    
    jassert(l != nullptr);

    txt = l->getText().toUpperCase();

    if (txt.isEmpty()) return;

    if (txt.containsOnly("0123456789"))
    {
        note = txt.getIntValue();
    }
    else if (txt.containsOnly("0123456789ABCDEFG-#"))
    {
        for (int i = 0; i < 128; i++)
        {
            if (notes[i].compare(txt) == 0)
            {
                note = i;
                break;
            }
        }
    }
        
    if (note < 0 || note > 127) return;

    switch (type)
    {
    case 0:

        audioProcessor.setRootNote(currentSample, note);
        break;
    
    case 1:

        audioProcessor.setLowNote(currentSample, note);
        break;
        
    case 2:

        audioProcessor.setHighNote(currentSample, note);
        break;
    }
}

void GuiComponent::initAllLabels()
{
    initLabel(&startLoopText,true);
    startLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    startLoopText.onTextChange = [&] { changeLoopText(&startLoopText, 0); };

    initLabel(&endLoopText,true);
    endLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    endLoopText.onTextChange = [&] { changeLoopText(&endLoopText, 2); };

    initLabel(&replenLoopText,true);
    replenLoopText.setText("000000", juce::NotificationType::dontSendNotification);
    replenLoopText.onTextChange = [&] { changeLoopText(&replenLoopText, 1); };

    initLabel(&sampleRateText,true);
    sampleRateText.setText("000000", juce::NotificationType::dontSendNotification);
    sampleRateText.onTextChange = [&] { changeRateText(&sampleRateText); };
    sampleRateText.setVisible(false);

    initLabel(&resampleRateText,true);
    resampleRateText.setText("000000", juce::NotificationType::dontSendNotification);
    resampleRateText.onTextChange = [&] 
    { 
        double val = 0.0;
        const juce::String txt = resampleRateText.getText().toUpperCase();

        if (txt.isEmpty()) return;
        if (!txt.containsOnly("0123456789ABCDEF") || (!textInHex && !txt.containsOnly("0123456789"))) return;

        val = textInHex ? txt.getHexValue32() : txt.getIntValue();
        
        if(val <= 0) return;

        audioProcessor.setResampleRate(currentSample, val); 
        resampleRateText.setText(txt.paddedLeft('0', 6), juce::NotificationType::dontSendNotification);
    };

    resampleRateText.setVisible(false);

    initLabel(&sampleMidiChannel, false);
    sampleMidiChannel.onTextChange = [&]
    {
        if (sampleMidiChannel.getText().isEmpty()) return;

        if(sampleMidiChannel.getText().containsOnly("0123456789"))
        {
            const int channel = sampleMidiChannel.getText().getIntValue();

            if (channel < 0) return;
            if (channel > 16) return;

            audioProcessor.setMidiChannel(currentSample, channel);
        }
        else if(sampleMidiChannel.getText().compareIgnoreCase("ALL") == 0)
            audioProcessor.setMidiChannel(currentSample, 0);
    };

    initLabel(&midiRootNote, false);
    midiRootNote.onTextChange = [&] { changeMidiNoteText(&midiRootNote, 0); };

    initLabel(&midiLowNote, false);
    midiLowNote.onTextChange = [&] { changeMidiNoteText(&midiLowNote, 1); };

    initLabel(&midiHiNote, false);
    midiHiNote.onTextChange = [&] { changeMidiNoteText(&midiHiNote, 2); };
}

void GuiComponent::initAllCheckboxes()
{
    initCheckBox(&muteBox, "Mute", -1);
    initCheckBox(&soloBox, "Solo", -1);

    initCheckBox(&monoBox, "Mono", 1001);
    initCheckBox(&ptpolyBox, "PT Poly", 1001);
    initCheckBox(&octapolyBox, "Octa Poly", 1001);

    initCheckBox(&modelType, "A500", -1);
    initCheckBox(&ledFilter, "LED",  -1);

    initCheckBox(&decText, "Dec", 2002);
    initCheckBox(&hexText, "Hex", 2002);

    initCheckBox(&pingpongLoop, "Ping Pong", 3003);
    initCheckBox(&forwardLoop, "Forward", 3003);

    initCheckBox(&enableAscii2Note, "Enabled", -1);

    monoBox.onClick = [&] 
    {
        audioProcessor.setMonoPoly(currentSample, 1); 
        if(audioProcessor.paulaStereoOn(currentSample)) paulaStereo.triggerClick();
    };
    
    ptpolyBox.onClick   = [&] { audioProcessor.setMonoPoly(currentSample, 2); };
    octapolyBox.onClick = [&] { audioProcessor.setMonoPoly(currentSample, 3); };

    ledFilter.onClick = [&] { repaint(ledRectangle); };

    modelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "MODEL TYPE", modelType);

    ledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>
        (audioProcessor.getAPVTS(), "LED FILTER", ledFilter);


    forwardLoop.onClick  = [&] { audioProcessor.setPingPongLoop(currentSample, 0); };
    pingpongLoop.onClick = [&] { audioProcessor.setPingPongLoop(currentSample, 1); };

    enableAscii2Note.onClick = [&] { audioProcessor.getAPVTS().state.setProperty("ascii2note", enableAscii2Note.getToggleState(), nullptr); };
    
    enableAscii2Note.setVisible(false);

    forwardLoop.setVisible(false);
    pingpongLoop.setVisible(false);

    decText.setVisible(false);
    hexText.setVisible(false);
}

void GuiComponent::automateLabelText(juce::Label *l, const int value)
{
    const juce::String valueText = textInHex ? juce::String::toHexString(value) : juce::String(value);

    jassert(l != nullptr);

=======
void GuiComponent::automateLoopText(juce::Label* l, const int value)
{
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
    if(l->isBeingEdited())
    {
        l->getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, JPAL(AMI_ORG));
        l->getCurrentTextEditor()->setJustification(juce::Justification::centred);
        return;
    }

<<<<<<< HEAD
    if ((textInHex && (l->getText().getHexValue32() == value)) || (!textInHex && (l->getText().getIntValue() == value))) return;

    l->setText(valueText.paddedLeft('0', 6).toUpperCase(), juce::NotificationType::dontSendNotification);
}

void GuiComponent::hideMainOptions()
{
    const bool show = !(showExtendedOptions);

    handleSliders.hideSliders(showExtendedOptions); 
    
    monoBox.setVisible(show);
    ptpolyBox.setVisible(show);
    octapolyBox.setVisible(show);

    muteBox.setVisible(show);
    soloBox.setVisible(show);
    
    sampleMidiChannel.setVisible(show);
    midiRootNote.setVisible(show);
    midiLowNote.setVisible(show);
    midiHiNote.setVisible(show);
    
    repaint();
}

void GuiComponent::hideMoreOptions()
{
    sampleRateText.setVisible(showExtendedOptions);
    resampleRateText.setVisible(showExtendedOptions);

    decText.setVisible(showExtendedOptions);
    hexText.setVisible(showExtendedOptions);
    
    enableAscii2Note.setVisible(showExtendedOptions);
    
    forwardLoop.setVisible(showExtendedOptions);
    pingpongLoop.setVisible(showExtendedOptions);
=======
    if (l->getText().getHexValue32() == value) return;

    l->setText(juce::String::toHexString(value).paddedLeft('0', 6).toUpperCase(), juce::NotificationType::dontSendNotification);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}
