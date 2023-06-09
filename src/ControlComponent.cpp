/*
  ==============================================================================

    ControlComponent.cpp
    Created: 13 May 2023 5:43:35pm
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ControlComponent.h"

/*
  ==============================================================================


  //// GUI button and slider controls ////


  ==============================================================================
*/

/*???? Possible TODO : move all buttons to own component and then
  inheret them into this component like the ADSR sliders ????*/

//==============================================================================
ControlComponent::ControlComponent(AmiSamplerAudioProcessor& p) : mADSR(p), mButtons(p),
    keyboardComponent(p.getKeyState(), juce::MidiKeyboardComponent::horizontalKeyboard), 
    audioProcessor(p)
{
    /* GUI control initialization */

    addAndMakeVisible(mLogoImage);
    addAndMakeVisible(mButtons);
    addAndMakeVisible(mADSR);

    addAndMakeVisible(startLoopText);
    addAndMakeVisible(endLoopText);
    addAndMakeVisible(replenLoopText);

    addAndMakeVisible(keyboardComponent);

    auto textBoxColour = juce::Colour(0);

    /* Start loop text box customization */
    startLoopText.setColour(juce::Label::backgroundColourId, textBoxColour);
    startLoopText.setColour(juce::Label::textColourId, juce::Colours::black);
    startLoopText.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    startLoopText.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0));
    startLoopText.setColour(juce::Label::ColourIds::outlineWhenEditingColourId, juce::Colour(0));

    /* Start loop text box initialization */
    startLoopText.setEditable(true, false, false);
    startLoopText.setEnabled(false);
    startLoopText.setJustificationType(juce::Justification::centred);
    startLoopText.setBorderSize(juce::BorderSize < int>(0));

    startLoopText.onTextChange = [&] 
    {
        /* Retains previously stored start loop sample number if input is incorrect or cleared out without new entry */
        if (!startLoopText.getText().containsOnly("0123456789abcdefABCDEF") || startLoopText.getText().isEmpty() || startLoopText.getText().getIntValue() < 0)
            mLoopStart = audioProcessor.getLoopStart();
        else
        /* Converts start loop sample text from hex value to integer and sets start loop point sample and loop point slider to integer value */
        mLoopStart = startLoopText.getText().getHexValue32();

        audioProcessor.setLoopStart(mLoopStart);
        startLoopText.setText(startLoopText.getText().paddedLeft('0', 7), juce::NotificationType::dontSendNotification);

        repaint();
    };

    /* End loop text box customization */
    endLoopText.setColour(juce::Label::backgroundColourId, textBoxColour);
    endLoopText.setColour(juce::Label::textColourId, juce::Colours::black);
    endLoopText.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    endLoopText.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0));
    endLoopText.setColour(juce::Label::ColourIds::outlineWhenEditingColourId, juce::Colour(0));

    /* Start loop text box customization */
    endLoopText.setEditable(true, false, false);
    endLoopText.setEnabled(false);
    endLoopText.setJustificationType(juce::Justification::centred);
    endLoopText.setBorderSize(juce::BorderSize < int>(0));

    endLoopText.onTextChange = [&] 
    {
        /* Retains previously stored end loop sample number if input is incorrect or cleared out without new entry */
        if (!endLoopText.getText().containsOnly("0123456789abcdefABCDEF") || endLoopText.getText().isEmpty() || (endLoopText.getText().getHexValue32() < 0))
            mLoopEnd = audioProcessor.getLoopEnd();
        else
            /* Converts end loop sample text from hex value to integer and sets end loop point sample and loop point slider to integer value */
            mLoopEnd = endLoopText.getText().getHexValue32();

        audioProcessor.setLoopEnd(mLoopEnd);
        endLoopText.setText(endLoopText.getText().paddedLeft('0', 7), juce::NotificationType::dontSendNotification);
        
        repaint();
    };

    /* Replen loop text box customization */
    replenLoopText.setColour(juce::Label::backgroundColourId, textBoxColour);
    replenLoopText.setColour(juce::Label::textColourId, juce::Colours::black);
    replenLoopText.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    replenLoopText.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0));
    replenLoopText.setColour(juce::Label::ColourIds::outlineWhenEditingColourId, juce::Colour(0));

    /* Start loop text box customization */
    replenLoopText.setEditable(true, false, false);
    replenLoopText.setEnabled(false);
    replenLoopText.setJustificationType(juce::Justification::centred);
    replenLoopText.setBorderSize(juce::BorderSize < int>(0));

    replenLoopText.onTextChange = [&]
    {
        /* Retains previously stored end loop sample number if input is incorrect or cleared out without new entry */
        if (!replenLoopText.getText().containsOnly("0123456789abcdefABCDEF") || replenLoopText.getText().isEmpty() || (replenLoopText.getText().getHexValue32() < 0))
            mLoopEnd = audioProcessor.getLoopEnd();
        else
            /* Converts end loop sample text from hex value to integer and sets end loop point sample and loop point slider to integer value */
            mLoopEnd = replenLoopText.getText().getHexValue32() + mLoopStart;

        audioProcessor.setLoopEnd(mLoopEnd);
        replenLoopText.setText(replenLoopText.getText().paddedLeft('0', 7), juce::NotificationType::dontSendNotification);

        repaint();
    };

    keyboardComponent.setAvailableRange(24, 108);

    /* Clears previous ASCII-to-MIDI map for custom map */
    keyboardComponent.clearKeyMappings();
}

ControlComponent::~ControlComponent()
{
}

void ControlComponent::paint (juce::Graphics& g)
{
    //???? Possible TODO: clean up all proportionOfWidth/Height // works well but seemingly-arbitrary values are harder to read ????//
    
    g.fillAll(juce::Colour(0xff0054aa));

    /* Recalls last base octave setting from previous session */
    if (numPadVal != audioProcessor.getBaseOctave())
        numPadVal = audioProcessor.getBaseOctave();

    /* Enable and disable loop point text boxes occuring to current loop enable button setting */
    if(audioProcessor.getLoopEnable())
    {
        startLoopText.setEnabled(true);
        endLoopText.setEnabled(true);
        replenLoopText.setEnabled(true);
    }
    else if (!audioProcessor.getLoopEnable())
    {
        startLoopText.setEnabled(false);
        endLoopText.setEnabled(false);
        replenLoopText.setEnabled(false);
    }

    theFont.setHeight(proportionOfWidth(0.035f));
    startLoopText.setFont(theFont);
    endLoopText.setFont(theFont);
    replenLoopText.setFont(theFont);

    if (!startLoopText.isBeingEdited() && !endLoopText.isBeingEdited() && !replenLoopText.isBeingEdited())
    {
        /* Tracks current loop point slider positions and passes loop point samples to text boxes in hexadecimal values*/
        std::stringstream startLoopToHex;
        std::stringstream endLoopToHex;
        std::stringstream replenLoopToHex;
        startLoopToHex << std::hex << audioProcessor.getLoopStart();
        endLoopToHex << std::hex << audioProcessor.getLoopEnd();
        replenLoopToHex << std::hex << audioProcessor.getLoopEnd() - audioProcessor.getLoopStart();

        startLoopText.setText(juce::String(startLoopToHex.str()).toUpperCase(), juce::NotificationType::sendNotificationAsync);
        endLoopText.setText(juce::String(endLoopToHex.str()).toUpperCase(), juce::NotificationType::sendNotificationAsync);
        replenLoopText.setText(juce::String(replenLoopToHex.str()).toUpperCase(), juce::NotificationType::sendNotificationAsync);
    }
    else
    {
        /* Text editor customizations */
        if (startLoopText.getCurrentTextEditor() != nullptr)
        {
            startLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff0054aa));
            startLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
            startLoopText.getCurrentTextEditor()->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xfffc8a00));
        }

        if(endLoopText.getCurrentTextEditor() != nullptr)
        {
            endLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff0054aa));
            endLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
            endLoopText.getCurrentTextEditor()->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xfffc8a00));
        }

        if (replenLoopText.getCurrentTextEditor() != nullptr)
        {
            replenLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff0054aa));
            replenLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
            replenLoopText.getCurrentTextEditor()->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xfffc8a00));
        }
    }

    /* Clear/Trash sample label */
    theFont.setHeight(proportionOfWidth(0.025f));
    g.setFont(theFont);
    g.setColour(juce::Colours::white);
    g.drawText("Trashcan", juce::Rectangle<int>(proportionOfWidth(0.886f), proportionOfHeight(0.785f), \
        proportionOfWidth(0.095f), proportionOfHeight(0.07f)), juce::Justification::centred, false);

    /* ASTRIID logo initialization */
    auto logo = juce::ImageCache::getFromMemory(BinaryData::astriid_amiga_png, BinaryData::astriid_amiga_pngSize);

    if (!logo.isNull())
        mLogoImage.setImage(logo, juce::RectanglePlacement::stretchToFit);
    else
        jassert(!logo.isNull());

    /* "POWER" LED drawing, turns to dark red when LED filter is off, 
        bright red when on, to simulate LED turning off or on */
    audioProcessor.isLEDOn() ? 
        g.setColour(juce::Colour(0xFFBB0000)) :
        g.setColour(juce::Colour(0xFF600000));

    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.894f), proportionOfHeight(0.58f), proportionOfWidth(0.075f), proportionOfHeight(0.02f)));
    
    /* "DRIVE" LED drawing, turns to dark green when file is being loaded or file is cleared, 
        bright green when a sample is loaded, to simulate LED turning off or on */
    mButtons.wasDiskOpClicked() || audioProcessor.getWaveForm().getNumSamples() <= 0 ?
        g.setColour(juce::Colour(0xFF008800)):
        g.setColour(juce::Colour(0xFF00BB00));

    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.894f), proportionOfHeight(0.62f), proportionOfWidth(0.075f), proportionOfHeight(0.02f)));

    keyboardComponent.setKeyWidth(keyboardComponent.getWhiteNoteLength() * 0.1776f);
}
void ControlComponent::paintOverChildren(juce::Graphics& g)
{
    
    theFont.setHeight(proportionOfWidth(0.0325f));

    g.setFont(theFont);

    g.setColour(juce::Colours::black);

    auto sampleTextBounds = juce::Rectangle<int>(proportionOfWidth(0.45f), proportionOfHeight(0.4f),
        proportionOfWidth(0.55f), proportionOfHeight(0.07f));

    /* Displays sample name, right adjusted, with blank spaces to the left filled in with underscore character */
    /* Fills entire sample name area with underscore character if no sample is loaded */
    if (audioProcessor.getWaveForm().getNumSamples() > 0)
    {
        g.drawText("SAMPLENAME:" + audioProcessor.getSampleName().toUpperCase().paddedLeft('_', 23), sampleTextBounds, juce::Justification::centred, false);

        if (audioProcessor.isNewFile()) { repaint(); audioProcessor.setNewFile(false); }
    }
    else
        g.drawText("SAMPLENAME:_______________________", sampleTextBounds, juce::Justification::centred, false);

    g.drawRect(juce::Rectangle<int>(0, proportionOfHeight(0.4f), getWidth(), proportionOfHeight(0.07f)), proportionOfWidth(0.00156f));

    g.setColour(juce::Colours::black);

    /* Start and End labels for corresponding textboxes */
    g.drawText(" START:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.47f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("REPEAT:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.54f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
    g.drawText("REPLEN:", juce::Rectangle<int>(proportionOfWidth(0.01f), proportionOfHeight(0.61f),
        proportionOfWidth(0.11f), proportionOfHeight(0.07f)), juce::Justification::centredLeft, false);
}

void ControlComponent::resized()
{
    /* Draws positioning and sizing for components, maintaining position and size when window resized */
    //???? Possible TODO: clean this up // works well but seemingly-arbitrary values are harder to read ????//

    mButtons.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    startLoopText.setBoundsRelative(0.11f, 0.47f, 0.14f, 0.07f);
    endLoopText.setBoundsRelative(0.11f, 0.54f, 0.14f, 0.07f);
    replenLoopText.setBoundsRelative(0.11f, 0.61f, 0.14f, 0.07f);

    keyboardComponent.setBoundsRelative(0.0f, 0.85f, 1.0f, 0.15f);
    mADSR.setBoundsRelative(0.0f, 0.68f, 0.67f, 0.64f);
    mLogoImage.setBoundsRelative(0.7f, 0.49f, 0.289506f, 0.06796875f);
}

bool ControlComponent::keyPressed(const juce::KeyPress& k)
{   
    /* Recaptures keyboard focus for ASCII MIDI if focus is lost */
    if(mButtons.hasKeyboardFocus(true) && !keyboardComponent.hasKeyboardFocus(true))
        keyboardComponent.grabKeyboardFocus();

    /* F12 key switches between A500 and A1200 a la Protracker 2.3 clone*/
    if (k.isKeyCode(juce::KeyPress::F12Key))
        audioProcessor.isModelA500() ? audioProcessor.setModelType(false) : audioProcessor.setModelType(true);

    /* Changes ASCII map note base octave with function keys a la Fasttracker II */
    if (k.getKeyCode() <= juce::KeyPress::F7Key && k.getKeyCode() >= juce::KeyPress::F1Key)
        numPadVal = k.getKeyCode() - juce::KeyPress::F1Key + 1;

    audioProcessor.setBaseOctave(numPadVal);

    /* JUCE has their own ASCII mapping function but it only allows one key per note 
        and triggers false key releases with mouse event within the window. 
        
        This is a more convoluted ASCII mapping function but allows for Protracker/Fasttracker style
        key mapping and seems to run cleaner than the default ASCII map, although '=' and '\' keys 
        are still buggy :// */

    /* Uses the integer value of ASCII keys to point to an array of note values for mapping */
    if (k.getKeyCode() >= ',' && k.getKeyCode() <= ']')
    {
        asciiNote = keyPress2Note[k.getKeyCode() - ','];

        if (asciiNote != 0)
            handleExtraNoteOn(asciiNote - 1);
    }

    return true;
}

bool ControlComponent::keyStateChanged(const bool isKeyDown)
{
    /* sends a Note Off for ASCII map keys */
    if (!isKeyDown)
    {
        for (int c = ','; c <= ']'; c++)
        {
            asciiNote = keyPress2Note[c - ','];

            if (!juce::KeyPress::isKeyCurrentlyDown(c))
                handleExtraNoteOff(asciiNote - 1);
        }
    }

    return true;
}

void ControlComponent::handleExtraNoteOn(const int note)
{
    /* Handles the Note On events for ASCII map */

    /* Blocks Note On event for extended map if second row keys of the same note are also held down */
    if(!audioProcessor.getKeyState().isNoteOn(1, (note + ((numPadVal) * 12))))
        audioProcessor.getKeyState().noteOn(1, (note + ((numPadVal) * 12)), 1.0f);
}

void ControlComponent::handleExtraNoteOff(const int note)
{
    /* Handles the Note Off events for extended ASCII map */

    /* The first five notes of the second octave have keys in the first and second row of the ASCII map.
        This makes sure the notes for both keys get a Note Off only when neither of the keys are down;
        otherwise the notes will be shut off by any key release */
    if ((note != 12 || !(juce::KeyPress::isKeyCurrentlyDown(',') || juce::KeyPress::isKeyCurrentlyDown('q'))) &&
        (note != 13 || !(juce::KeyPress::isKeyCurrentlyDown('l') || juce::KeyPress::isKeyCurrentlyDown('2'))) &&
        (note != 14 || !(juce::KeyPress::isKeyCurrentlyDown('.') || juce::KeyPress::isKeyCurrentlyDown('w'))) &&
        (note != 15 || !(juce::KeyPress::isKeyCurrentlyDown(';') || juce::KeyPress::isKeyCurrentlyDown('3'))) &&
        (note != 16 || !(juce::KeyPress::isKeyCurrentlyDown('/') || juce::KeyPress::isKeyCurrentlyDown('e'))))
        audioProcessor.getKeyState().noteOff(1, (note + ((numPadVal) * 12)), 0.0f);
}
