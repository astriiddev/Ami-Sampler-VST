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

    initButton(&enableLoop, true);
    initButton(&paulaStereo, true);

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
}

void GuiComponent::paint (juce::Graphics& g)
{
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
    g.fillRect(ledRectangle);

    g.setColour(juce::Colours::lime);
    g.fillRect(proportionOfWidth(0.905f), proportionOfHeight(0.67f), proportionOfWidth(0.07f), proportionOfHeight(0.02f));

    g.setFont(getLookAndFeel().getLabelFont(startLoopText).withHeight(18.f));
    g.setColour(JPAL(AMI_WHT));

    g.drawText("LOOP  START:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.72f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("LOOP REPEAT:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.75f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("LOOP REPLEN:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.78f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);

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
    g.setColour(JPAL(AMI_BLD));
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), sampleMidiChannel.getBounds().getWidth(), 1);
    g.fillRect(sampleMidiChannel.getBounds().getX(), sampleMidiChannel.getBounds().getY(), 1, sampleMidiChannel.getBounds().getHeight());

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), midiRootNote.getBounds().getWidth(), 1);
    g.fillRect(midiRootNote.getBounds().getX(), midiRootNote.getBounds().getY(), 1, midiRootNote.getBounds().getHeight());
    
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
}

void GuiComponent::resized()
{
    handleSliders.setBoundsRelative(0.f, 0.f, 1.f, 1.f);

    paulaStereo.setBoundsRelative(0.008f, 0.615f, 0.075f, 0.05f);
    enableLoop.setBoundsRelative(0.008f, 0.664f, 0.075f, 0.05f);

    monoBox.setBoundsRelative(0.085f, 0.615f, 0.05f, 0.03f);
    ptpolyBox.setBoundsRelative(0.085f, 0.65f, 0.07f, 0.03f);
    octapolyBox.setBoundsRelative(0.085f, 0.685f, 0.08f, 0.03f);

    muteBox.setBoundsRelative(0.165f, 0.628f, 0.05f, 0.03f);
    soloBox.setBoundsRelative(0.165f, 0.673f, 0.05f, 0.03f);

    modelType.setBoundsRelative(0.72f, 0.73f, 0.15f, 0.06f);
    ledFilter.setBoundsRelative(0.72f, 0.785f, 0.15f, 0.06f);

    startLoopText.setBoundsRelative(0.08f, 0.735f, 0.11f, 0.04f);
    endLoopText.setBoundsRelative(0.08f, 0.765f, 0.11f, 0.04f);
    replenLoopText.setBoundsRelative(0.08f, 0.795f, 0.11f, 0.04f);

    sampleMidiChannel.setBoundsRelative(0.515f, 0.63f, 0.035f, 0.05f);
    midiRootNote.setBoundsRelative(0.555f, 0.63f, 0.035f, 0.05f);
    midiLowNote.setBoundsRelative(0.595f, 0.63f, 0.035f, 0.05f);
    midiHiNote.setBoundsRelative(0.635f, 0.63f, 0.035f, 0.05f);
}

void GuiComponent::mouseDown(const juce::MouseEvent& e)
{
    if (enableLoop.getBounds().contains(e.getMouseDownPosition())) repaint();
}

void GuiComponent::mouseUp(const juce::MouseEvent& e)
{
    if (enableLoop.getBounds().contains(e.getMouseDownPosition())) repaint();
}

void GuiComponent::initLabel(juce::Label* l, const bool clearBack)
{
    /* Start loop text box customization */
    const juce::Colour backColour = clearBack ? JPAL(0) : JPAL(0xFF00113D);

    addAndMakeVisible(*l);
    l->setFont(20.f);
    l->setRepaintsOnMouseActivity(false);

    l->setColour(juce::Label::backgroundColourId, backColour);
    l->setColour(juce::Label::textColourId, JPAL(AMI_WHT));
    l->setColour(juce::Label::textWhenEditingColourId, JPAL(AMI_WHT));
    l->setColour(juce::Label::backgroundWhenEditingColourId, backColour);
    l->setColour(juce::Label::ColourIds::outlineWhenEditingColourId, JPAL(0));

    l->setEditable(true, false, false);
    l->setEnabled(true);
    l->setJustificationType(juce::Justification::centred);
    l->setBorderSize(juce::BorderSize < int>(0));
}

void GuiComponent::initButton(juce::Button* b, const bool redWhenDown)
{
    const juce::Colour boxColour = JPAL(AMI_GRY);
    const juce::Colour downColour = redWhenDown ? JPAL(AMI_RED) : boxColour;
    const juce::Colour downTxtColour = redWhenDown ? JPAL(AMI_WHT) : JPAL(AMI_BLK);

    addAndMakeVisible(*b);
    b->setRepaintsOnMouseActivity(false);
    b->setClickingTogglesState(true);
    b->setColour(juce::TextButton::buttonColourId, boxColour);
    b->setColour(juce::TextButton::buttonOnColourId, downColour);
    b->setColour(juce::TextButton::textColourOffId, JPAL(AMI_BLK));
    b->setColour(juce::TextButton::textColourOnId, downTxtColour);
}

void GuiComponent::automateLoopText(juce::Label* l, const int value)
{
    if(l->isBeingEdited())
    {
        l->getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, JPAL(AMI_ORG));
        l->getCurrentTextEditor()->setJustification(juce::Justification::centred);
        return;
    }

    if (l->getText().getHexValue32() == value) return;

    l->setText(juce::String::toHexString(value).paddedLeft('0', 6).toUpperCase(), juce::NotificationType::dontSendNotification);
}
