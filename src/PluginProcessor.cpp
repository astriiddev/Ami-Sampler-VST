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
    mSampler.setNoteStealingEnabled(true);
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

    /* Updates value tree  */
    getLoopStart(); getLoopEnd();
    mSliderStart = (double)mLoopStart / mWaveForm.getNumSamples();
    mSliderEnd = (double)mLoopEnd / mWaveForm.getNumSamples();
    mAPVTS.state.setProperty("loopStart", getLoopStart(), nullptr);
    mAPVTS.state.setProperty("loopEnd", getLoopEnd(), nullptr);
    mAPVTS.state.setProperty("loopEnable", (bool)getLoopEnable(), nullptr);

    mAPVTS.state.setProperty("baseOctave", getBaseOctave(), nullptr);

    mAPVTS.state.setProperty("modeltype", (bool)isModelA500(), nullptr);
    mAPVTS.state.setProperty("filter", (bool)isLEDOn(), nullptr);

    if (mShouldUpdate)
    {
        updateADSR();
    }
    
    /* Virtual MIDI event callback */
    keyState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    /* Monophonic/Polyphonic state*/
    for (int i = 0; i < 7; ++i)
    {    
        /* One-voice monophonic */
        if (mNumVoiceState == 1 && mSampler.getNumVoices() < 1)
        {
            mSampler.addVoice(new NewSamplerVoice(*this));
        }
        
        /* Four-voice polyphonic (PT Poly) */
        else if (mNumVoiceState == 2 && mSampler.getNumVoices() < 4)
        {
            mSampler.addVoice(new NewSamplerVoice(*this));
        }

        /* Eight-voice polyphonic (MED Poly) */
        else if (mNumVoiceState == 0 && mSampler.getNumVoices() < 8)
        {
            mSampler.addVoice(new NewSamplerVoice(*this));
        }
    }

    mAPVTS.state.setProperty("numofvoices", (int)mNumVoiceState, nullptr);
    mAPVTS.state.setProperty("stereoOn", (bool)mStereo, nullptr);

    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
   
    /* Output buffer with volume and pan controls */
    auto* channelDataLeft = buffer.getWritePointer(0);
    auto* channelDataRight = buffer.getWritePointer(1);

    for (int n = 0; n < buffer.getNumSamples(); ++n)
    {
        channelDataLeft[n] = buffer.getSample(0, n) * rawVolume * leftPan;
        channelDataRight[n] = buffer.getSample(1, n) * rawVolume * rightPan;
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

            mNumVoiceState = (int)mAPVTS.state.getProperty("numofvoices");
            mA500 = mAPVTS.state.getProperty("modeltype");
            mFilterOn = mAPVTS.state.getProperty("filter");
            mStereo = mAPVTS.state.getProperty("stereoOn");

        }
}

void AmiSamplerAudioProcessor::saveFile(const juce::String& name)
{
    /* file formats */
    juce::WavAudioFormat wavFormat;
    juce::AiffAudioFormat aifFormat;
    juce::RawPcmFormat rawFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer;

    juce::FileChooser chooser{ "Save file" };
    juce::File file;

    /* Saves loop point metadata if loop points are set.
        Can't seem to get aif files to save loop point metadata but I also
        had issues getting SoX, Audacity, and Fasttracker II clone to save
        aif loop points so manybe it's an aif problem */
    juce::StringPairArray wavMetaData, aifMetaData;

    if (mLoopEnable)
    {
        wavMetaData.set("NumSampleLoops", "1");
        wavMetaData.set("Loop0Start", juce::String(getLoopStart()));
        wavMetaData.set("Loop0End", juce::String(getLoopEnd()));

        aifMetaData.set("NumSampleLoops", "1");
        aifMetaData.set("CueNote0Identifier", "0");
        aifMetaData.set("CueNote0Text", "Created with Ami Sampler");
        aifMetaData.set("CueNote0TimeStamp", "3768748643");
        aifMetaData.set("NumCueNotes", "1");

        aifMetaData.set("Loop0Type", "0");
        aifMetaData.set("Loop0StartIdentifier", juce::String(getLoopStart()));
        aifMetaData.set("Loop0EndIdentifier", juce::String(getLoopEnd()));
        aifMetaData.set("Loop1Type", "0");
        aifMetaData.set("Loop1StartIdentifier", juce::String(getLoopStart()));
        aifMetaData.set("Loop1EndIdentifier", juce::String(getLoopEnd()));
    }
    else
        wavMetaData = aifMetaData = NULL;

    /*!!!! TODO: include file types in drop down dialog box */
    if (chooser.browseForFileToSave(true))
    {
        file = chooser.getResult();

        /* if extension isn't specified, save as raw 8-bit signed PCM file */
        if(file.hasFileExtension(""))
        {
            if (file.withFileExtension("raw").exists())
                file.withFileExtension("raw").deleteFile();

            else if (file.withFileExtension("smp").exists())
                file.withFileExtension("smp").deleteFile();
        }

        /* Deletes old file if file is being written over */
        if (file.exists())
            file.deleteFile();

        /* WAV file saving */
        if(file.hasFileExtension(".wav"))
            writer.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file.withFileExtension("wav")),
                fileSampleRate, mWaveForm.getNumChannels(), fileBitDepth, wavMetaData, 0));

        /* Raw 8-bit signed PCM saving (.smp in this instance is original Fasttracker II's extension for a .raw audio file) */
        if (file.hasFileExtension(".raw") || file.hasFileExtension("smp") || file.hasFileExtension(""))
            writer.reset(rawFormat.createWriterFor(new juce::FileOutputStream(file), NULL, NULL, NULL, NULL, NULL));

        /* AIF/AIFF file saving, no loop points yet :(( */
        if(file.hasFileExtension(".aif") || file.hasFileExtension(".aiff"))
            writer.reset(aifFormat.createWriterFor(new juce::FileOutputStream(file),
                fileSampleRate, mWaveForm.getNumChannels(), fileBitDepth, aifMetaData, 0));

        /* Write file */
        if (writer != nullptr && file.hasFileExtension(".wav") || file.hasFileExtension(".aif") ||
            file.hasFileExtension(".raw") || file.hasFileExtension(".smp") || file.hasFileExtension(""))
        {
            writer->writeFromAudioSampleBuffer(mWaveForm, 0, mWaveForm.getNumSamples());

            /* Allows user to import saved file to continue working with said file or continue with old file if user says no*/
            if (juce::NativeMessageBox::showYesNoBox(juce::MessageBoxIconType::QuestionIcon, "Replace Sample?",
                "Replace currently loaded sample with saved file?", nullptr, nullptr))
            {
                loadSavedFile = true;
                mFilePath = file.getFullPathName();
            }
            else
                loadSavedFile = false;
        }
        else
            juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
                "File not saved!", "Could not save file.",
                "OK", nullptr);
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

        /* Only loads file if valid file type or no extension is found (loads as 8-bit signed PCM) */
        if (file.hasFileExtension("wav") || file.hasFileExtension("aif") || file.hasFileExtension("aiff") || 
            file.hasFileExtension("raw") || file.hasFileExtension("smp") || !file.getFileName().contains("."))
            

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
    loadSavedFile = false;
    /* Saves loaded file path to value tree */
    //!!!! TODO: generate a temporary file to be used for recalling and file editing instead of original file !!!!//
    mAPVTS.state.setProperty("pathname", path, nullptr);

    auto file = juce::File(path);
    
    /* Error box if previously used is file not found */
    if ((!file.exists() || file.getSize() <= 0) && (int)mAPVTS.state.getProperty("pathname") != NULL)
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
            "File not found!", path + " not found!", "OK", nullptr);

        /* Clears out sample data if previous file isn't found */
        mSampler.clearSounds();
    } 
    else if(file.exists() && file.getSize() > 0)
    {
        newFile = true;
        mSampleName = file.getFileNameWithoutExtension();

        /* Format reader for file format */
        //!!!! TODO: Create format reader for 8SVX formated .IFF files !!!!//
        std::unique_ptr<juce::AudioFormatReader> mFormatReader (mFormatManager.createReaderFor(file));

        auto sampleLength = static_cast<int>(mFormatReader->lengthInSamples);

        mWaveForm.setSize(1, sampleLength);
        mFormatReader->read(&mWaveForm, 0, sampleLength, 0, true, false);

        /* Reads metadata to find sample loops */
        //???? Possible TODO: if audio is 16 bit or higher, or 44.1khz sample rate or higher, resample to 8-bit 16726hz for Amiga-style sample ???//

        auto metaData = mFormatReader.get()->metadataValues;

        fileSampleRate = mFormatReader.get()->sampleRate;
        fileBitDepth = mFormatReader.get()->bitsPerSample;

        if(metaData.containsKey("Loop0Start") && metaData.containsKey("Loop0End"))
        {
            /* If loop metadata is found, set loop points according to the metadata and enable looping */
            auto fileLoopStart = metaData.getValue("Loop0Start", "double").getDoubleValue();
            auto fileLoopEnd = metaData.getValue("Loop0End", "double").getDoubleValue();
            mSliderStart = fileLoopStart / double(mWaveForm.getNumSamples());
            mSliderEnd = fileLoopEnd / double(mWaveForm.getNumSamples());
            mLoopEnable = true;
        }
        else if (metaData.containsKey("Loop0StartIdentifier") && metaData.containsKey("Loop0EndIdentifier"))
        {
            /* If loop metadata is found, set loop points according to the metadata and enable looping */
            auto fileLoopStart = metaData.getValue("Loop0StartIdentifier", "double").getDoubleValue();
            auto fileLoopEnd = metaData.getValue("Loop0EndIdentifier", "double").getDoubleValue();
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

    /* Pan controls */
    auto panVol = mAPVTS.getRawParameterValue("PANNER")->load();
    if (panVol < 1.0f)
    {
        rightPan = panVol;
        leftPan = 1.0f;
    }
    /* Center pan is 1.0f, with ranges 0.0-1.0 for left and 1.0 to 0.0f for right */
    else if(panVol > 1.0f)
    {
        rightPan = 1.0f;
        leftPan = (panVol - 2.0f) * -1;
    }
    else if (panVol == 1.0f)
    {
        rightPan = panVol;
        leftPan = panVol;
    }

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
    /* Creates parameters for volume, panning, and ADSR sliders*/
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume",
        juce::NormalisableRange<float>(-47.0f, 1.0f, 0.75f), -4.5f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("PANNER", "Panner",
        juce::NormalisableRange<float>(0.0f, 2.0f, 2.0/255), 1.0));

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
