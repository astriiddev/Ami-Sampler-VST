/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmiSamplerAudioProcessor::AmiSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), mAPVTS(*this, nullptr, "Parameters", createParameters())
#endif
{

    mFormatManager.registerBasicFormats();
    mAPVTS.state.addListener(this);
    keyState.addListener(this);

    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new NewSamplerVoice(*this));
    }
}

AmiSamplerAudioProcessor::~AmiSamplerAudioProcessor()
{
    mSampler.clearSounds();
    mFormatManager.clearFormats();
    keyState.removeListener(this);
    mMidiBuffer.clear();
}

//==============================================================================
const juce::String AmiSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmiSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmiSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmiSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmiSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmiSamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmiSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmiSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmiSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmiSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmiSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    midiCollector.reset(sampleRate);
    updateADSR();
}

void AmiSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmiSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AmiSamplerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    /* MIDI buffer and MIDI collector processing*/
    mMidiBuffer = midiMessages;
    midiCollector.removeNextBlockOfMessages(mMidiBuffer, buffer.getNumSamples());

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    /* Captures loop point settings and saves them to value tree  */
    getLoopStart(); getLoopEnd();
    mSliderStart = (double)mLoopStart / mWaveForm.getNumSamples();
    mSliderEnd = (double)mLoopEnd / mWaveForm.getNumSamples();
    mAPVTS.state.setProperty("loopStart", getLoopStart(), nullptr);
    mAPVTS.state.setProperty("loopEnd", getLoopEnd(), nullptr);
    mAPVTS.state.setProperty("loopEnable", (bool)getLoopEnable(), nullptr);

    mAPVTS.state.setProperty("baseOctave", getBaseOctave(), nullptr);

    if (mShouldUpdate)
    {
        updateADSR();
    }
    
    /* Virtual MIDI event callback */
    keyState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    for (const juce::MidiMessageMetadata m : midiMessages)
    {
        juce::MidiMessage message = m.getMessage();
        if (message.isNoteOn())
        {
            // start increasing sample counter
            mIsNotePlayed = true;
            mCurrentChannel = message.getChannel();
            mCurrentNote = message.getNoteNumber();
        }
        else if (message.isNoteOff())
        {
            // reset sample counter
            mIsNotePlayed = false;
        }
    }

    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    for (int i = 0; i < totalNumOutputChannels; ++i)
    {
        auto* channelData = buffer.getWritePointer(i);

        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            channelData[n] = buffer.getSample(i, n) * rawVolume;
        }
    }

    midiMessages.clear();
}

//==============================================================================
bool AmiSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmiSamplerAudioProcessor::createEditor()
{
    return new AmiSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void AmiSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    /* Saves value process to XML to recall for next session */
    auto state = mAPVTS.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml()); 
    copyXmlToBinary(*xml, destData);
}

void AmiSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    /* Recalls ADSR, loaded sample, loop points, loop enable state, and ASCII note base */
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(mAPVTS.state.getType()))
        {
            mAPVTS.replaceState(juce::ValueTree::fromXml(*xmlState));
            loadFile(mAPVTS.state.getProperty("pathname").toString());
            mLoopStart = ((double)mAPVTS.state.getProperty("loopStart"));
            mLoopEnd = ((double)mAPVTS.state.getProperty("loopEnd"));
            mLoopEnable = (bool)mAPVTS.state.getProperty("loopEnable");
            mBaseOctave = ((int)mAPVTS.state.getProperty("baseOctave"));
        }
}

void AmiSamplerAudioProcessor::buttonLoadFile()
{
    /* Load button function */
    juce::FileChooser chooser{ "Please Load A File" };
    if (chooser.browseForFileToOpen())
    {
        /* Passes file from browser to file loader */
        auto file = chooser.getResult();
        /* Only loads file if valid file type */
        if (file.getFileName().contains(".wav") || file.getFileName().contains(".mp3") || 
            file.getFileName().contains(".aif") || file.getFileName().contains(".raw") ||
            file.getFileName().contains(".smp"))

            loadFile(file.getFullPathName());

        /* Error window for invalid file, but make it ~quirky~ */
        else
        {
            juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
                "File type invalid!", "Why are you trying to load something other than audio into an audio sampler?", 
                "I'm such a silly goose :(", nullptr);

            /* Reloads previously loaded file if invalid file */
            loadFile(mAPVTS.state.getProperty("pathname").toString());
        }
    }
}

void AmiSamplerAudioProcessor::loadFile(const juce::String& path)
{
    /* File loading */

    /* Clears previously loaded samples */
    mSampler.clearSounds();
    newFile = false;

    /* Saves loaded file path to value tree */
    //!!!! TODO: generate a temporary file to be used for recalling and file editing instead of original file !!!!//
    mAPVTS.state.setProperty("pathname", path, nullptr);

    auto file = juce::File(path);
    
    /* Error box if previously used is file not found */
    if(!file.exists() && (int)mAPVTS.state.getProperty("pathname") != NULL)
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
            "File not found!", path + " not found!", "OK", nullptr);

        /* Clears out sample data if previous file isn't found */
        mSampler.clearSounds();
    } 
    else if(file.exists())
    {
        newFile = true;
        mSampleName = file.getFileNameWithoutExtension();

        /* Format reader for file format */
        //!!!! TODO: Create format reader for 8SVX formated .IFF files !!!!//
        std::unique_ptr<juce::AudioFormatReader> mFormatReader (mFormatManager.createReaderFor(file));

        auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples) - 1;

        mWaveForm.setSize(1, sampleLength);
        mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

        /* Reads metadata to find sample loops */
        //???? Possible TODO: if audio is 16 bit or higher, or 44.1khz sample rate or higher, resample to 8-bit 16726hz for Amiga-style sample ???//
        auto metaData = mFormatReader.get()->metadataValues;

        if(metaData.containsKey("Loop0Start") && metaData.containsKey("Loop0End"))
        {
            /* If loop metadata is found, set loop points according to the metadata and enable looping */
            auto fileLoopStart = metaData.getValue("Loop0Start", "double").getDoubleValue();
            auto fileLoopEnd = metaData.getValue("Loop0End", "double").getDoubleValue();
            mSliderStart = fileLoopStart / double(mWaveForm.getNumSamples());
            mSliderEnd = fileLoopEnd / double(mWaveForm.getNumSamples());
            mLoopEnable = true;
        }
        else
        {
            /* If no loop metadata is found, sets start loop to 0 and end loop to last sample and disables looping */
            mSliderStart = 0.0f;
            mSliderEnd = 1.0f;
            mLoopEnable = false;
        }

        juce::BigInteger range;
        range.setRange(0, 128, true);

        /* Adds loaded file to sampler */
        mSampler.addSound(new NewSamplerSound("Sample", *mFormatReader,
            range, 60, 0.1, 0.1, 10.0, *this));
    }

}

void AmiSamplerAudioProcessor::updateADSR() 
{
    /* Exponential volume setting // mutes audio if volume slider is at min value */
    if (mAPVTS.getRawParameterValue("VOLUME")->load() == -47)
        rawVolume = 0;
    else
        rawVolume = pow(10, (mAPVTS.getRawParameterValue("VOLUME")->load()/20));

    /* Updates ADSR parameters in real time */
    mADSRParams.attack = mAPVTS.getRawParameterValue("ATTACK")->load();
    mADSRParams.sustain = mAPVTS.getRawParameterValue("SUSTAIN")->load();
    mADSRParams.decay = mAPVTS.getRawParameterValue("DECAY")->load();
    mADSRParams.release = mAPVTS.getRawParameterValue("RELEASE")->load();

    for (int i = 0; i < mSampler.getNumSounds(); ++i)
    {
        if (auto sound = dynamic_cast<NewSamplerSound*>(mSampler.getSound(i).get()))
        {
            sound->setEnvelopeParameters (mADSRParams);
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AmiSamplerAudioProcessor::createParameters()
{
    /* Creates parameters for volume and ADSR sliders*/
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume",
        juce::NormalisableRange<float>(-47.0f, 1.0f, 0.75f), -4.5f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("ATTACK", "Attack",
        juce::NormalisableRange<float>(0.0003f, 10.0f, 0.15625f), 0.0003f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("DECAY", "Decay",
        juce::NormalisableRange<float>(0.0f, 4.0f, 0.0625f), 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("SUSTAIN", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.015625f), 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("RELEASE", "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.078125f), 0.001f));

    
    return { parameters.begin(), parameters.end() };
}

void AmiSamplerAudioProcessor::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property)
{
    mShouldUpdate = true;
}
void AmiSamplerAudioProcessor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    /* Virtual MIDI keyboard Note On handling*/
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(m);

    /* Resets playhead if a new Note On event is sent */
    mNewNote = m.isNoteOn(true);
}

void AmiSamplerAudioProcessor::handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    /* Virtual MIDI keyboard Note Off handling*/
    
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(m);
    
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmiSamplerAudioProcessor();
}