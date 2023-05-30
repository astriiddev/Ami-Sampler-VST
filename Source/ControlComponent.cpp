/*
ce/*
  ==============================================================================

    ControlComponent.cpp
    Created: 13 May 2023 5:43:35pm
    Author:  finle

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
ControlComponent::ControlComponent(AmiSamplerAudioProcessor& p) :
    keyboardComponent(p.getKeyState(), juce::MidiKeyboardComponent::horizontalKeyboard), 
    mADSR(p), audioProcessor(p)
{
    /* Adding all of the controls and passing the custom cursor to each of them */

    setMouseCursor(getMouseCursor().ParentCursor);

    addAndMakeVisible(mLogoImage);
    addAndMakeVisible(mADSR);

    mLogoImage.setMouseCursor(getMouseCursor().ParentCursor);
    mADSR.setMouseCursor(getMouseCursor().ParentCursor);

    addAndMakeVisible(mBoxBacking);
    addAndMakeVisible(mStartBacking);
    addAndMakeVisible(mEndBacking);

    mBoxBacking.setMouseCursor(getMouseCursor().ParentCursor);
    mStartBacking.setMouseCursor(getMouseCursor().ParentCursor);
    mEndBacking.setMouseCursor(getMouseCursor().ParentCursor);

    addAndMakeVisible(startLoopText);
    addAndMakeVisible(endLoopText);
    addAndMakeVisible(mEnableLoop);

    startLoopText.setMouseCursor(getMouseCursor().ParentCursor);
    endLoopText.setMouseCursor(getMouseCursor().ParentCursor);
    mEnableLoop.setMouseCursor(getMouseCursor().ParentCursor);

    addAndMakeVisible(mLoadSample);
    addAndMakeVisible(mClearSample);
    addAndMakeVisible(mSaveSample);

    mLoadSample.setMouseCursor(getMouseCursor().ParentCursor);
    mClearSample.setMouseCursor(getMouseCursor().ParentCursor);
    mSaveSample.setMouseCursor(getMouseCursor().ParentCursor);

    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setMouseCursor(getMouseCursor().ParentCursor);

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
            startLoopText.setText(juce::String(mLoopStart).toUpperCase(), juce::NotificationType::sendNotificationAsync);

        /* Converts start loop sample text from hex value to integer and sets start loop point sample and loop point slider to integer value */
        mLoopStart = startLoopText.getText().getHexValue32();
        startLoopText.setText(startLoopText.getText().paddedLeft('0', 5), juce::NotificationType::dontSendNotification);
        audioProcessor.setLoopStart(mLoopStart);

        repaint();
    };

    /* End loop text box customization */
    endLoopText.setColour(juce::Label::backgroundColourId, textBoxColour);
    endLoopText.setColour(juce::Label::textColourId, juce::Colours::black);
    endLoopText.setColour(juce::Label::textWhenEditingColourId, juce::Colours::black);
    endLoopText.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0));
    endLoopText.setColour(juce::Label::ColourIds::outlineWhenEditingColourId, juce::Colour(0));

    /* Start loop text box initialization */
    endLoopText.setEditable(true, false, false);
    endLoopText.setEnabled(false);
    endLoopText.setJustificationType(juce::Justification::centred);
    endLoopText.setBorderSize(juce::BorderSize < int>(0));

    endLoopText.onTextChange = [&] 
    {
        /* Retains previously stored end loop sample number if input is incorrect or cleared out without new entry */
        if (!endLoopText.getText().containsOnly("0123456789abcdefABCDEF") || endLoopText.getText().isEmpty() || endLoopText.getText().getIntValue() < 0)
            endLoopText.setText(juce::String(mLoopEnd).toUpperCase(), juce::NotificationType::sendNotificationAsync);

        /* Converts end loop sample text from hex value to integer and sets end loop point sample and loop point slider to integer value */
        mLoopEnd = endLoopText.getText().getHexValue32();
        endLoopText.setText(endLoopText.getText().paddedLeft('0', 5), juce::NotificationType::dontSendNotification);
        audioProcessor.setLoopEnd(mLoopEnd);

        repaint();
    };

    //!!!! TODO: change button to drawn rectangles/line rather than image to optimize graphics !!!!// 
    //???? Possible TODO: scrap loop enable button for three-way radio button when PING-PONG looping is implemented ????//
    /* Loop enable button customization */
    auto loopOff = juce::ImageCache::getFromMemory(BinaryData::amiLoopOff_png, BinaryData::amiLoopOff_pngSize);
    auto loopOn = juce::ImageCache::getFromMemory(BinaryData::amiLoopOn_png, BinaryData::amiLoopOn_pngSize);

    mEnableLoop.setImages(true, true, false, loopOff, 1.0f, juce::Colour(0),
        loopOff, 1.0, juce::Colour(0x60000000),
        loopOn, 1.0f, juce::Colour(0), 0.0f);

    /* Loop enable button optimization */
    mEnableLoop.addListener(this);
    mEnableLoop.setToggleable(true);
    mEnableLoop.setToggleState(false, juce::NotificationType::dontSendNotification);

    /* Sample clear/Trash button customization */
    auto clearOff = juce::ImageCache::getFromMemory(BinaryData::amiTrashOff_png, BinaryData::amiClearOff_pngSize);
    mTrashOff = clearOff.rescaled(60, 66, juce::Graphics::lowResamplingQuality);
   
    auto clearClicked = juce::ImageCache::getFromMemory(BinaryData::amiTrashOn_png, BinaryData::amiClearOn_pngSize);
    mTrashClicked = clearClicked.rescaled(60, 66, juce::Graphics::lowResamplingQuality);

    mClearSample.setImages(true, true, true, mTrashOff, 1.0f, juce::Colour(0),
        mTrashOff, 1.0, juce::Colour(0x60000000),
        mTrashClicked, 1.0f, juce::Colour(0), 0.0f);

    /* Clear button initialization */
    mClearSample.addListener(this);

    //!!!! TODO: change button to drawn rectangles/line rather than image to optimize graphics !!!!// 
    //???? Possible TODO: create floppy disk icon for load ????//
    /* Load button customization */
    auto loadOff = juce::ImageCache::getFromMemory(BinaryData::amiLoadOff_png, BinaryData::amiLoadOff_pngSize);
    auto loadOn = juce::ImageCache::getFromMemory(BinaryData::amiLoadOn_png, BinaryData::amiLoadOn_pngSize);

    mLoadSample.setImages(true, true, false, loadOff, 1.0f, juce::Colour(0),
        loadOff, 1.0, juce::Colour(0x60000000),
        loadOn, 1.0f, juce::Colour(0), 0.0f);

    mLoadSample.addListener(this);

    //!!!!!!!! UNUSABLE RIGHT NOW!!!! NOT IMPLEMENTED !!!!!!!!//
    //!!!! TODO: literally the entire file export function !!!!//
    /* Save button customization */
    auto saveOff = juce::ImageCache::getFromMemory(BinaryData::amiSaveOff_png, BinaryData::amiSaveOff_pngSize);
    auto saveOn = juce::ImageCache::getFromMemory(BinaryData::amiSaveOn_png, BinaryData::amiSaveOn_pngSize);

    mSaveSample.setImages(true, true, false, saveOff, 1.0f, juce::Colour(0),
        saveOff, 1.0, juce::Colour(0xF0000000),
        saveOff, 1.0f, juce::Colour(0), 0.0f);

    mSaveSample.addListener(this);
    mSaveSample.setMouseCursor(getMouseCursor().ParentCursor);

    /* Clears previous ASCII-to-MIDI map for custom map */
    keyboardComponent.setAvailableRange(24, 108);
    keyboardComponent.clearKeyMappings();

    //???? Possible TODO: ASCII MIDI enable/disable button (pass key presses to DAW when disabled) ????//
    /* Creates first two octaves of ASCII-to-MIDI map, mapped to Protracker/Fasttracker ASCII MIDI map */
    /* First octave starts at key Z and ends at key M // Second octave starts at key Q and ends at key \ */
    int noteLower = 0;
    for (char c : "zsxdcvgbhnjmq2w3er5t6y7ui9o0p[")
        keyboardComponent.setKeyPressForNote({ c, 0, 0 }, noteLower++);

    /* JUCE ASCII map doesn't seem to capture keypresses for key = or \  so ] has to be 
       added separate in order for = and \ to be implemented in keypress function */
    keyboardComponent.setKeyPressForNote(juce::KeyPress(']'), 31);

    // Sets default C-note base octave to octave 5 (same as Fasttracker II default octave) //
    keyboardComponent.setKeyPressBaseOctave(numPadVal);
}

ControlComponent::~ControlComponent()
{
    /* Button dereferencing */
    mEnableLoop.removeListener(this);
    mLoadSample.removeListener(this);
    mClearSample.removeListener(this);
    mSaveSample.removeListener(this);
}

void ControlComponent::paint (juce::Graphics& g)
{
    //???? Possible TODO: clean up all proportionOfWidth/Height // works well but seemingly-arbitrary values are harder to read ????//

    g.fillAll(juce::Colour(0xff0054aa));

    /* Recalls last base octave setting from previous session */
    if (numPadVal != audioProcessor.getBaseOctave())
    {
        numPadVal = audioProcessor.getBaseOctave();
    }

    /* Enable and disable loop point text boxes occuring to current loop enable button setting */
    if(audioProcessor.getLoopEnable())
    {
        mEnableLoop.setToggleState(true, juce::NotificationType::dontSendNotification);
        startLoopText.setEnabled(true);
        endLoopText.setEnabled(true);
    }
    else if (!audioProcessor.getLoopEnable())
    {
        mEnableLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
        startLoopText.setEnabled(false);
        endLoopText.setEnabled(false);
    }
    
    juce::Font theFont("AmiDOS Regular", "Regular", proportionOfWidth(0.035f));
    startLoopText.setFont(theFont);
    endLoopText.setFont(theFont);

    if (!startLoopText.isBeingEdited() && !endLoopText.isBeingEdited())
    {
        /* Tracks current loop point slider positions and passes loop point samples to text boxes in hexadecimal values*/
        std::stringstream startLoopToHex;
        std::stringstream endLoopToHex;
        startLoopToHex << std::hex << audioProcessor.getLoopStart();
        endLoopToHex << std::hex << audioProcessor.getLoopEnd();

        startLoopText.setText(juce::String(startLoopToHex.str()).toUpperCase(), juce::NotificationType::sendNotificationAsync);
        endLoopText.setText(juce::String(endLoopToHex.str()).toUpperCase(), juce::NotificationType::sendNotificationAsync);

    }
    else
    {
        /* Text editor customizations */
        if (startLoopText.getCurrentTextEditor() != nullptr)
        {
            startLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff0054aa));
            startLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
            startLoopText.getCurrentTextEditor()->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xfffc8a00));
            startLoopText.getCurrentTextEditor()->setMouseCursor(juce::MouseCursor::ParentCursor);
        }

        if(endLoopText.getCurrentTextEditor() != nullptr)
        {
            endLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightColourId, juce::Colour(0xff0054aa));
            endLoopText.getCurrentTextEditor()->setColour(juce::TextEditor::highlightedTextColourId, juce::Colours::white);
            endLoopText.getCurrentTextEditor()->setColour(juce::CaretComponent::caretColourId, juce::Colour(0xfffc8a00));
            endLoopText.getCurrentTextEditor()->setMouseCursor(juce::MouseCursor::ParentCursor);
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
    {
        mLogoImage.setImage(logo, juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        jassert(!logo.isNull());
    }

    //!!!! TODO: "POWER" LED "brightness" change when Buttersworth filter enable/disable !!!!//
    /* "Power" and "DRIVE" LED drawing */
    g.setColour(juce::Colour(0xFF880000));
    g.fillRect(juce::Rectangle<float>(proportionOfWidth(0.894f), proportionOfHeight(0.58f), proportionOfWidth(0.075f), proportionOfHeight(0.02f)));
    g.setColour(juce::Colour(0xFF008800));
    g.fillRect(juce::Rectangle<float>(proportionOfWidth(0.894f), proportionOfHeight(0.62f), proportionOfWidth(0.075f), proportionOfHeight(0.02f)));
    keyboardComponent.setKeyWidth(keyboardComponent.getWhiteNoteLength() * 0.1776f);
    
    /* Draws box graphics behind loop point text boxes and sample name */
    auto sampleBox = juce::ImageCache::getFromMemory(BinaryData::amiBoxBacking_png, BinaryData::amiBoxBacking_pngSize);

    if (!sampleBox.isNull())
    {
        mBoxBacking.setImage(sampleBox, juce::RectanglePlacement::stretchToFit);
        mStartBacking.setImage(sampleBox, juce::RectanglePlacement::stretchToFit);
        mEndBacking.setImage(sampleBox, juce::RectanglePlacement::stretchToFit);
    }
    else
    {
        jassert(!sampleBox.isNull());
    }
}
void ControlComponent::paintOverChildren(juce::Graphics& g)
{

    juce::Font theFont("AmiDOS Regular", "Regular", (proportionOfWidth(0.0325f)));
    g.setFont(theFont);

    g.setColour(juce::Colours::black);

    auto sampleTextBounds = juce::Rectangle<int>(proportionOfWidth(0.415f), proportionOfHeight(0.4f),
        proportionOfWidth(0.49f), proportionOfHeight(0.07f));

    /* Displays sample name, right adjusted, with blank spaces to the left filled in with underscore character */
    /* Fills entire sample name area with underscore character if no sample is loaded */
    if (audioProcessor.getWaveForm().getNumSamples() > 0)
    {
        g.drawText("SAMPLE:" + audioProcessor.getSampleName().toUpperCase().paddedLeft('_', 17), sampleTextBounds, juce::Justification::centredLeft, false);

        if (audioProcessor.isNewFile()) { repaint(); audioProcessor.setNewFile(false); }
    }
    else
    {
        g.drawText("SAMPLE:_________________", sampleTextBounds, juce::Justification::centredLeft, false);
    }

    g.drawRect(juce::Rectangle<int>(0, proportionOfHeight(0.4f), getWidth(), proportionOfHeight(0.07f)), proportionOfWidth(0.00156f));

    g.setColour(juce::Colour(0xF0000000));
    g.fillRect(juce::Rectangle<float>(proportionOfWidth(0.8f), proportionOfHeight(0.4f), proportionOfWidth(0.1f), proportionOfHeight(0.07f)));

    g.setColour(juce::Colours::black);

    /* Start and End labels for corresponding textboxes */
    g.drawText("S:", juce::Rectangle<int>(proportionOfWidth(0.1f), proportionOfHeight(0.4f),
        proportionOfWidth(0.04f), proportionOfHeight(0.07f)), juce::Justification::centredRight, false);
    g.drawText("E:", juce::Rectangle<int>(proportionOfWidth(0.25f), proportionOfHeight(0.4f),
        proportionOfWidth(0.04f), proportionOfHeight(0.07f)), juce::Justification::centredRight, false);
}

void ControlComponent::resized()
{
    /* Draws positioning and sizing for components, maintaining position and size when window resized */
    //???? Possible TODO: clean this up // works well but seemingly-arbitrary values are harder to read ????//
    mBoxBacking.setBoundsRelative(0.4f, 0.4f, 0.4f, 0.07f);

    mClearSample.setBoundsRelative(0.884f, 0.657f, 0.09375f, 0.1375f);

    mLoadSample.setBoundsRelative(0.9f, 0.4f, 0.1f, 0.07f);
    mSaveSample.setBoundsRelative(0.8f, 0.4f, 0.1f, 0.07f);

    mEnableLoop.setBoundsRelative(0.0f, 0.4f, 0.1, 0.07f);
    mStartBacking.setBoundsRelative(0.1f, 0.4f, 0.15f, 0.07f);
    mEndBacking.setBoundsRelative(0.25f, 0.4f, 0.15f, 0.07f);
    startLoopText.setBoundsRelative(0.15f, 0.4f, 0.1f, 0.07f);
    endLoopText.setBoundsRelative(0.3f, 0.4f, 0.1f, 0.07f);

    keyboardComponent.setBoundsRelative(0.0f, 0.85f, 1.0f, 0.15f);
    mADSR.setBoundsRelative(0.0f, 0.65f, 0.33f, 0.675f);
    mLogoImage.setBoundsRelative(0.7f, 0.49f, 0.289506f, 0.06796875f);
}

void ControlComponent::buttonClicked(juce::Button* button)
{
    /* Enables and disables sample looping occording to Loop enable button setting*/
    if (button == &mEnableLoop && audioProcessor.getWaveForm().getNumSamples() > 0)
    {
        if (!startLoopText.isEnabled())
        {
            audioProcessor.setLoopEnable(true);
            startLoopText.setEnabled(true);
            endLoopText.setEnabled(true);
        }

        else if (startLoopText.isEnabled())
        {
            audioProcessor.setLoopEnable(false);
            startLoopText.setEnabled(false);
            endLoopText.setEnabled(false);
        }

        keyboardComponent.grabKeyboardFocus();
        repaint();
    }

    /* Clears out sample, sets loop points to 0, saves cleared sample status to be recalled in next session */
    if (button == &mClearSample)
    {
        audioProcessor.setLoopStart(0);
        audioProcessor.setLoopEnd(0);
        mEnableLoop.setToggleState(false, juce::NotificationType::sendNotificationSync);

        audioProcessor.getSampler().clearSounds();
        audioProcessor.getWaveForm().setSize(1, 0);
        audioProcessor.getWaveForm().clear();
        audioProcessor.getAPVTS().state.setProperty("pathname", (const char*)nullptr, nullptr);
        keyboardComponent.giveAwayKeyboardFocus();
        repaint();
    }

    //!!!! TODO: include 8SVX formatted .IFF files as file type !!!!//
    //!!!! TODO: have directory window specify file types !!!!//
    /* Opens directory window for sample loading */
    if (button == &mLoadSample)
    {
        audioProcessor.buttonLoadFile();
        keyboardComponent.grabKeyboardFocus();
        audioProcessor.setLoopStart(audioProcessor.getSliderStart() * audioProcessor.getWaveForm().getNumSamples());
        audioProcessor.setLoopEnd(audioProcessor.getSliderEnd() * audioProcessor.getWaveForm().getNumSamples());
    }
}

bool ControlComponent::keyPressed(const juce::KeyPress& k)
{   
    /*???? Possible TODO: currently have to press keys twice to initially recapture focus when focus lost,
      would be nice to automatically do it with first keypress but still works fine if can't ????*/
    /* Recaptures keyboard focus for ASCII MIDI if focus is lost */
    if(!mClearSample.hasKeyboardFocus(false) && !keyboardComponent.hasKeyboardFocus(true))
        keyboardComponent.grabKeyboardFocus();

    /* Changes ASCII map note base octave with function keys a la Fasttracker II */
    /* Any attempt at more "elegant" code worked fine in standalone but triggers DAW host key shortcuts when used as plugin :( */
    if (k.isKeyCode(juce::KeyPress::F1Key)) { numPadVal = 1; }
    if (k.isKeyCode(juce::KeyPress::F2Key)) { numPadVal = 2; }
    if (k.isKeyCode(juce::KeyPress::F3Key)) { numPadVal = 3; }
    if (k.isKeyCode(juce::KeyPress::F4Key)) { numPadVal = 4; }
    if (k.isKeyCode(juce::KeyPress::F5Key)) { numPadVal = 5; }
    if (k.isKeyCode(juce::KeyPress::F6Key)) { numPadVal = 6; }
    if (k.isKeyCode(juce::KeyPress::F7Key)) { numPadVal = 7; }

    keyboardComponent.setKeyPressBaseOctave(numPadVal);
    audioProcessor.setBaseOctave(numPadVal);

    /* JUCE only allows one ASCII key mapped to one note */
    /* This sends Note On MIDI events for note duplicates to maintain Protracker/Fastracker style map */
    /* m%Pressed bool is required to blocked repeated key presses after initial key press when key is held down */
    if (k.getTextCharacter() == ',' && !mCommaPressed) { handleExtraNoteOn(0); mCommaPressed = true; }
    if (k.getTextCharacter() == 'l' && !mLCharPressed) { handleExtraNoteOn(1); mLCharPressed = true; }
    if (k.getTextCharacter() == '.' && !mDotChPressed) { handleExtraNoteOn(2); mDotChPressed = true; }
    if (k.getTextCharacter() == ';' && !mColonPressed) { handleExtraNoteOn(3); mColonPressed = true; }
    if (k.getTextCharacter() == '/' && !mSlashPressed) { handleExtraNoteOn(4); mSlashPressed = true; }

    /* JUCE's ASCII mapping doesn't support = or \ for some reason. This /kinda/ gets around that but is still buggy
       = and \ keys sometimes get triggered off with other key releases */
    if (k.getTextCharacter() == '=' && !mEqualPressed) { handleExtraNoteOn(18);mEqualPressed = true; }
    if (k.getTextCharacter() == '\\' && !mBSlhPressed) { handleExtraNoteOn(20); mBSlhPressed = true; }

    return true;
}

bool ControlComponent::keyStateChanged(const bool isKeyDown)
{
    /* sends a Note Off for extended ASCII map keys and resets m%Pressed bool to be used on next key press */
    /* ANDing key character and m%Pressed bool required to not send unwanted Note Off MIDI event to a key that may still be held down!!!! */ 
    if (!isKeyDown)
    {
        if (!juce::KeyPress::isKeyCurrentlyDown(',') && mCommaPressed) { handleExtraNoteOff(0); mCommaPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown('l') && mLCharPressed) { handleExtraNoteOff(1); mLCharPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown('.') && mDotChPressed) { handleExtraNoteOff(2); mDotChPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown(';') && mColonPressed) { handleExtraNoteOff(3); mColonPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown('/') && mSlashPressed) { handleExtraNoteOff(4); mSlashPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown('=') && mEqualPressed) { handleExtraNoteOff(18);mEqualPressed = false; }
        if (!juce::KeyPress::isKeyCurrentlyDown('\\')&& mBSlhPressed)  { handleExtraNoteOff(20); mBSlhPressed = false; }
    }

    return true;
}

void ControlComponent::handleExtraNoteOn(const int note)
{
    //???? Possible TODO: block Note On event in second row if same note in extended map is currently held down ????//
    /* Handles the Note On events for extended ASCII map */
    /* Blocks Note On event for extended map if second row keys of the same note are also held down */
    if(!audioProcessor.getKeyState().isNoteOn(1, (note + ((numPadVal + 1) * 12))))
        audioProcessor.getKeyState().noteOn(1, (note + ((numPadVal + 1) * 12)), 1.0f);
}

void ControlComponent::handleExtraNoteOff(const int note)
{
    /* Handles the Note Off events for extended ASCII map */
    audioProcessor.getKeyState().noteOff(1, (note + ((numPadVal + 1) * 12)), 0.0f);
}