/*
  ==============================================================================

    ButtonComponent.cpp
    Created: 7 Jun 2023 10:32:26am
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ButtonComponent.h"

/*
  ==============================================================================


  //// Button controls ////


  ==============================================================================
*/

//==============================================================================
ButtonComponent::ButtonComponent(AmiSamplerAudioProcessor& p) : audioProcessor(p)
{
    /* Button initialization */
    addAndMakeVisible(mSampleBacking);
    addAndMakeVisible(mStartBacking);
    addAndMakeVisible(mEndBacking);
    addAndMakeVisible(mReplenBacking);

    addAndMakeVisible(mEnableLoop);
    addAndMakeVisible(mLoadSample);
    addAndMakeVisible(mClearSample);
    addAndMakeVisible(mSaveSample);

    addAndMakeVisible(mModelType);
    addAndMakeVisible(mLEDFilter);
    addAndMakeVisible(mMonoOn);
    addAndMakeVisible(mStereoOn);

    /* Grey colour for all the text label backings*/
    auto boxColour = juce::Colour(0xff9e9e9e);

    mSampleBacking.setColour(juce::TextButton::buttonColourId, boxColour);
    mStartBacking.setColour(juce::TextButton::buttonColourId, boxColour);
    mEndBacking.setColour(juce::TextButton::buttonColourId, boxColour);
    mReplenBacking.setColour(juce::TextButton::buttonColourId, boxColour);

    /* Disables the actual button ability for these as they just lie behind the loop point/sample name text
        and aren't really buttons, but are using the button graphics for ease of use and to keep uniformity */
    mSampleBacking.setEnabled(false);
    mStartBacking.setEnabled(false);
    mEndBacking.setEnabled(false);
    mReplenBacking.setEnabled(false);

    /* Loop enable button customization */
    mEnableLoop.addListener(this);
    mEnableLoop.setToggleable(true);
    mEnableLoop.setToggleState(false, juce::NotificationType::dontSendNotification);

    mEnableLoop.setColour(juce::TextButton::buttonColourId, boxColour);
    mEnableLoop.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mEnableLoop.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffc92d28));
    mEnableLoop.setColour(juce::TextButton::textColourOnId, juce::Colours::white);

    /* Load button customization */
    mLoadSample.addListener(this);
    mLoadSample.setColour(juce::TextButton::buttonColourId, boxColour);
    mLoadSample.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    /* Save button customization */
    mSaveSample.addListener(this);
    mSaveSample.setColour(juce::TextButton::buttonColourId, boxColour);
    mSaveSample.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

    /* Sample clear/Trash button customization */
    mClearSample.addListener(this);
    auto clearOff = juce::ImageCache::getFromMemory(BinaryData::amiTrashOff_png, BinaryData::amiTrashOff_pngSize);
    mTrashOff = clearOff.rescaled(60, 66, juce::Graphics::lowResamplingQuality);

    auto clearClicked = juce::ImageCache::getFromMemory(BinaryData::amiTrashOn_png, BinaryData::amiTrashOn_pngSize);
    mTrashClicked = clearClicked.rescaled(60, 66, juce::Graphics::lowResamplingQuality);

    mClearSample.setImages(true, true, true, mTrashOff, 1.0f, juce::Colour(0),
        mTrashOff, 1.0, juce::Colour(0x60000000),
        mTrashClicked, 1.0f, juce::Colour(0), 0.0f);

    /* A500/A1200 button customization */
    mModelType.addListener(this);
    mModelType.setClickingTogglesState(true);
    mModelType.setColour(juce::TextButton::buttonColourId, boxColour);
    mModelType.setColour(juce::TextButton::buttonOnColourId, boxColour);
    mModelType.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mModelType.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    /* LED button customization */
    mLEDFilter.addListener(this);
    mLEDFilter.setClickingTogglesState(true);
    mLEDFilter.setColour(juce::TextButton::buttonColourId, boxColour);
    mLEDFilter.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mLEDFilter.setColour(juce::TextButton::buttonOnColourId, juce::Colour(boxColour));
    mLEDFilter.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    /* Mono/Poly button customization */
    mMonoOn.addListener(this);
    mMonoOn.setColour(juce::TextButton::buttonColourId, boxColour);
    mMonoOn.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mMonoOn.setColour(juce::TextButton::textColourOnId, juce::Colours::black);

    /* Paula Stereo button customization */
    mStereoOn.addListener(this);
    mStereoOn.setClickingTogglesState(true);
    mStereoOn.setColour(juce::TextButton::buttonColourId, boxColour);
    mStereoOn.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    mStereoOn.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffc92d28));
    mStereoOn.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
}

ButtonComponent::~ButtonComponent()
{
}

void ButtonComponent::paint (juce::Graphics& g)
{
    /* Sets button states based on processor settings */
    numVoiceState = audioProcessor.getNumVoiceState();
    setNumVoiceState(numVoiceState);
    mModelType.setToggleState(audioProcessor.isModelA500(), juce::NotificationType::sendNotificationSync);
    mLEDFilter.setToggleState(audioProcessor.isLEDOn(), juce::NotificationType::sendNotificationSync);
    mStereoOn.setToggleState(audioProcessor.isStereo(), juce::NotificationType::sendNotificationSync);

    /* Enable and disable loop point text boxes occuring to current loop enable button setting */
    audioProcessor.getLoopEnable()?
        mEnableLoop.setToggleState(true, juce::NotificationType::dontSendNotification) :
        mEnableLoop.setToggleState(false, juce::NotificationType::dontSendNotification);
}

void ButtonComponent::resized()
{
    mSampleBacking.setBoundsRelative(0.45f, 0.4f, 0.55f, 0.07f);

    mClearSample.setBoundsRelative(0.884f, 0.657f, 0.09375f, 0.1375f);

    mModelType.setBoundsRelative(0.25f, 0.47f, 0.1f, 0.07f);
    mLEDFilter.setBoundsRelative(0.35f, 0.47f, 0.1f, 0.07f);
    mMonoOn.setBoundsRelative(0.1f, 0.4f, 0.15f, 0.07f);
    mStereoOn.setBoundsRelative(0.25f, 0.54f, 0.2f, 0.07f);

    mLoadSample.setBoundsRelative(0.35f, 0.4f, 0.1f, 0.07f);
    mSaveSample.setBoundsRelative(0.25f, 0.4f, 0.1f, 0.07f);

    mEnableLoop.setBoundsRelative(0.0f, 0.4f, 0.1, 0.07f);

    mStartBacking.setBoundsRelative(0.0f, 0.47f, 0.25f, 0.07f);
    mEndBacking.setBoundsRelative(0.0f, 0.54f, 0.25f, 0.07f);
    mReplenBacking.setBoundsRelative(0.0f, 0.61f, 0.25f, 0.07f);
}


void ButtonComponent::buttonClicked(juce::Button* button)
{
    /* Enables and disables sample looping occording to Loop enable button setting*/
    if (button == &mEnableLoop && audioProcessor.getWaveForm().getNumSamples() > 0)
       audioProcessor.getLoopEnable() ? audioProcessor.setLoopEnable(false) : audioProcessor.setLoopEnable(true);

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
    }

    //!!!! TODO: include 8SVX formatted .IFF files as file type !!!!//
    //!!!! TODO: have directory window specify file types !!!!//
    /* Opens directory window for sample loading */
    if (button == &mLoadSample)
    {
        /* bool for green LED animation */
        diskOpClicked = true;

        /* Button turns red when clicked */
        mLoadSample.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffc92d28));
        mLoadSample.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        /* Loads file from browser, sets loop points */
        audioProcessor.buttonLoadFile();

        audioProcessor.setLoopStart(juce::roundToInt(audioProcessor.getSliderStart() * (double)audioProcessor.getWaveForm().getNumSamples()));
        audioProcessor.setLoopEnd(juce::roundToInt(audioProcessor.getSliderEnd() * (double)audioProcessor.getWaveForm().getNumSamples()));

        /* Reset button graphic state */
        mLoadSample.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff9e9e9e));
        mLoadSample.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        
        diskOpClicked = false;
    }

    if (button == &mSaveSample)
    {
        /* bool for green LED animation */
        diskOpClicked = true;

        /* Button turns red when clicked */
        mSaveSample.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffc92d28));
        mSaveSample.setColour(juce::TextButton::textColourOffId, juce::Colours::white);

        /* Saves audio from buffer as file*/
        audioProcessor.saveFile(audioProcessor.getSampleName());

        /* Allows user to load saved file into sampler */
        if (audioProcessor.loadingSaveFile())
            audioProcessor.loadFile(audioProcessor.getFilePath());

        /* Reset button graphic state */
        mSaveSample.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff9e9e9e));
        mSaveSample.setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        diskOpClicked = false;
    }

    if (button == &mModelType)
    {
        /* Switch between A500 model (4.4khz low pass filter) and A1200 model (no low pass filter) */
        if (mModelType.getToggleState())
        {
            mModelType.setButtonText("A500");
            audioProcessor.setModelType(true);
        }
        if (!mModelType.getToggleState())
        {
            mModelType.setButtonText("A1200");
            audioProcessor.setModelType(false);
        }
    }

    if (button == &mLEDFilter)
    {
        /* Turns 3khz Butterworth LED filter on */
        mLEDFilter.getToggleState() ? 
            audioProcessor.setFilterOn(true) : 
            audioProcessor.setFilterOn(false);
    }

    if (button == &mMonoOn)
    {
        /* Tri-state monophonic/Polyphonic button */

        numVoiceState++;

        /* Clears number of voices if mono/poly button is clicked */
        audioProcessor.getSampler().clearVoices();

        if (numVoiceState == 3)
            numVoiceState = 0;

        setNumVoiceState(numVoiceState);
    }

    if (button == &mStereoOn)
    {
        /* Paula Stereo switch, every other note played gets panned left or right, 
            simulating the Paula sound chip's hard left and right panning  on the Amiga */
        if (mStereoOn.getToggleState())
            audioProcessor.setStereo(true);
        else
        {
            audioProcessor.setStereo(false);

            if ((numVoiceState == 0 || numVoiceState == 2))
                stereoState = false;
        }
    }
}
void ButtonComponent::setNumVoiceState(int state)
{
    /* "ProTracker" polyphony mode: up to four note polyphony */
    if (numVoiceState == 2)
    {
        mMonoOn.setButtonText("PT POLY");
        mStereoOn.setEnabled(true);
        if (mStereoOn.getToggleState())
            stereoState = true;
    }

    /* Monophonic mode: only one note at a time, great for basslines ;) */
    if (numVoiceState == 1)
    {
        mMonoOn.setButtonText("MONO");
        audioProcessor.setStereo(false);

        /* Disables the note auto-panning */
        mStereoOn.setEnabled(false);
    }

    /* "OctaMED" polyphony mode: up to eight note polyphony */
    if (numVoiceState == 0)
    {
        mMonoOn.setButtonText("MEDPOLY");
        mStereoOn.setEnabled(true);
        if (mStereoOn.getToggleState())
            stereoState = true;
    }

    /* Retains Paula stereo state for PT and MED poly when switch from monophonic back to polyphonic */
    if ((numVoiceState == 0 || numVoiceState == 2) && stereoState == true)
        audioProcessor.setStereo(true);

    audioProcessor.setNumVoiceState(numVoiceState);
}
