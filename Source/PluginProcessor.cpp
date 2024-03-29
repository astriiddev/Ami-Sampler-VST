/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "NewSamplerVoice.h"

//==============================================================================
AmiAudioProcessor::AmiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), APVTS(*this, nullptr, "Parameters", createParameters())
#endif
{
    formatManager.registerBasicFormats();

    for(int n = 0; n < numSamplers; n++)
    {
        sampler[n].setNoteStealingEnabled(true);

        numVoices[n] = 8;

        setNumVoices(n);

        sampleName[n] = "";

        loopEnable[n] = loopStart[n] = loopEnd[n] = 0;
        paulaStereo[n] = panCounter[n] = 0;

        channelMute[n] = channelSolo[n] = sampleMidiChannel[n] = 0;

        snh[n] = 1;
        sourceSampleRate[n] = 16726.;

        midiLowNote[n] = 0;
        midiRootNote[n] = 60;
        midiHiNote[n] = 127;

        channelVolume[n] = channelGliss[n] = 1.f;
        channelPan[n] = 128.f;

        channelAttack[n] = 0.0003f;
        channelDecay[n] = tune[n] = 0.f;
        channelSustain[n] = 1.f;
        channelRelease[n] = 0.001f;
    }

    sampleMidiChannel[0] = 0;

    APVTS.state.addListener(this);
    keyState.addListener(this);
}

AmiAudioProcessor::~AmiAudioProcessor()
{
    for (int n = 0; n < numSamplers; n++)
        sampler[n].clearSounds();

    formatManager.clearFormats();
    keyState.removeListener(this);
}

//==============================================================================
const juce::String AmiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmiAudioProcessor::setCurrentProgram (int /*index*/)
{
}

const juce::String AmiAudioProcessor::getProgramName (int /*index*/)
{
    return {};
}

void AmiAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/)
{
}

//==============================================================================
void AmiAudioProcessor::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    devSampleRate = sampleRate;

    for (int n = 0; n < numSamplers; n++)
        sampler[n].setCurrentPlaybackSampleRate(devSampleRate);

    initFilters();
    
    midiCollector.reset(devSampleRate);

    init = false;
}

void AmiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AmiAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels  = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    const float* sampReadL = buffer.getReadPointer(0);
    const float* sampReadR = totalNumOutputChannels > 1 ? buffer.getReadPointer(1) : nullptr;

    float* sampWriteL = buffer.getWritePointer(0);
    float* sampWriteR = totalNumOutputChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    int numSamples = buffer.getNumSamples();

    midiBuffer = midiMessages;
    midiCollector.removeNextBlockOfMessages(midiBuffer, buffer.getNumSamples());
    updateParams();

    keyState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
    
    if (!midiBuffer.isEmpty())
    {
        for (int i = 0; i < midiBuffer.data.size() - 2; i++)
        {
            if (i == 6 && (midiBuffer.data.getUnchecked(i) & 0xB0) == 0xB0)
            {
                if (midiBuffer.data.getUnchecked(i + 1) == 1)
                {
                    modIntensity = midiBuffer.data.getUnchecked(i + 2);
                    APVTS.getParameter("VIBRATO INTENSITY")->beginChangeGesture();
                    APVTS.getParameterAsValue("VIBRATO INTENSITY").setValue(modIntensity);
                    APVTS.getParameter("VIBRATO INTENSITY")->endChangeGesture();
                    break;
                }
            }
        }
    }

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int n = 0; n < numSamplers; n++)
        sampler[n].renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    while (--numSamples >= 0)
    {
        float l = *sampReadL++, 
              r = sampReadR == nullptr ? l : *sampReadR++;

        float outL = 0, outR = 0;

        incVibratoTable();

        getAmiFilter(&l, &r, &outL, &outR);

        outL *= masterVol;
        outR *= masterVol;
  
        if (sampReadR == nullptr)
        {
            *sampWriteL++ = (outL + outR) * 0.5f;
        }
        else
        {
            outL *= masterPanL;
            outR *= masterPanR;

            *sampWriteL++ = outL;
            *sampWriteR++ = outR;
        }
    }
}

//==============================================================================
bool AmiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmiAudioProcessor::createEditor()
{
    return new AmiAudioProcessorEditor (*this);
}

//==============================================================================
void AmiAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree state = APVTS.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AmiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    init = true;

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(APVTS.state.getType()))
        {
            APVTS.replaceState(juce::ValueTree::fromXml(*xmlState));
            int soloedSample = -1;
            for (int i = 0; i < numSamplers; i++)
            {
                currentSample = i;
                juce::String pathAPVTS = "pathname";
                pathAPVTS.append(juce::String(i), 2);
                juce::String path = APVTS.state.getProperty(pathAPVTS).toString();

                if (path.isNotEmpty()) 
                {
                    loadFile(path);

                    paramsUpdated = true;
                    updateParams();

                    if (channelSolo[i]) soloedSample = i;
                }
            }

            currentSample = 0;
        }

    init = false;
}

void AmiAudioProcessor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);

    if (source->isNoteOn(midiChannel, midiNoteNumber)) return;

    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(m);
}

void AmiAudioProcessor::handleNoteOff(juce::MidiKeyboardState* /*source*/, int midiChannel, int midiNoteNumber, float velocity)
{
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(m);
}

void AmiAudioProcessor::saveFile(const juce::String& name)
{
    juce::WavAudioFormat wavFormat;
    juce::AiffAudioFormat aifFormat;
    juce::IffAudioFormat iffFormat;
    juce::MuLawFormat muFormat;

    std::unique_ptr<juce::AudioFormatWriter> writer;

    juce::FileChooser chooser{ "Save file", juce::File::getCurrentWorkingDirectory().getChildFile(name), "*.wav;*.aif;*.aiff;*.iff;*.raw;*.bin" };
    juce::File file;

    juce::StringPairArray wavMetaData, iffMetaData, aifMetaData;

    for (int i = 0; i < 16; i++)
        keyState.allNotesOff(i);

    if (loopEnable[currentSample])
    {
        wavMetaData.set("Loop0Start", juce::String(getLoopStart(currentSample)));
        wavMetaData.set("Loop0End", juce::String(getLoopEnd(currentSample) - 1));
        wavMetaData.set("NumSampleLoops", "1");
        
        iffMetaData.set("Loop0Start", juce::String(getLoopStart(currentSample)));
        iffMetaData.set("Loop0Repeat", juce::String(getLoopEnd(currentSample) - getLoopStart(currentSample)));

        aifMetaData.set("NumSampleLoops", "1");
        aifMetaData.set("CueNote0Identifier", "0");
        aifMetaData.set("CueNote0Text", "Created with Ami Sampler");
        aifMetaData.set("CueNote0TimeStamp", "3768748643");
        aifMetaData.set("NumCueNotes", "1");

        aifMetaData.set("Loop0Type", "0");
        aifMetaData.set("Loop0StartIdentifier", juce::String(getLoopStart(currentSample)));
        aifMetaData.set("Loop0EndIdentifier", juce::String(getLoopEnd(currentSample) - 1));
        aifMetaData.set("Loop1Type", "0");
        aifMetaData.set("Loop1StartIdentifier", juce::String(getLoopStart(currentSample)));
        aifMetaData.set("Loop1EndIdentifier", juce::String(getLoopEnd(currentSample) - 1));
    }
    else
        wavMetaData = aifMetaData = NULL;

    if (chooser.browseForFileToSave(true))
    {
        file = chooser.getResult();

        if (file.hasFileExtension(""))
        {
            if (file.withFileExtension("raw").exists())
                file.withFileExtension("raw").deleteFile();

            else if (file.withFileExtension("smp").exists())
                file.withFileExtension("smp").deleteFile();
        }

        if (file.exists())
            file.deleteFile();

        if (file.hasFileExtension(".wav") || file.hasFileExtension(""))
            writer.reset(wavFormat.createWriterFor(new juce::FileOutputStream(file.withFileExtension("wav")),
                sourceSampleRate[currentSample], waveForm[currentSample].getNumChannels(), 8, wavMetaData, 0));

        if (file.hasFileExtension(".iff"))
            writer.reset(iffFormat.createWriterFor(new juce::FileOutputStream(file.withFileExtension("iff")),
                8363, 1, 8, iffMetaData, 0));

        if (file.hasFileExtension(".raw") || file.hasFileExtension("smp"))
            writer.reset(iffFormat.createWriterFor(new juce::FileOutputStream(file)));

        if (file.hasFileExtension(".bin"))
            writer.reset(muFormat.createWriterFor(new juce::FileOutputStream(file)));

        if (file.hasFileExtension(".aif") || file.hasFileExtension(".aiff"))
            writer.reset(aifFormat.createWriterFor(new juce::FileOutputStream(file),
                sourceSampleRate[currentSample], waveForm[currentSample].getNumChannels(), 8, aifMetaData, 0));

        if (writer != nullptr && (file.hasFileExtension(".wav") || file.hasFileExtension(".aif") || file.hasFileExtension(".bin") ||
            file.hasFileExtension(".iff") || file.hasFileExtension(".raw") || file.hasFileExtension(".smp") || file.hasFileExtension("")))
        {
            writer->writeFromAudioSampleBuffer(waveForm[currentSample], 0, waveForm[currentSample].getNumSamples());
        }
        else
            juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
                "File not saved!", "Could not save file.",
                "OK", nullptr);
    }
}

bool AmiAudioProcessor::buttonLoadFile()
{
    for (int i = 0; i < 16; i++)
        keyState.allNotesOff(i);

    juce::FileChooser chooser{ "Open File", juce::File::getCurrentWorkingDirectory(), "*.wav;*.aif;*.aiff;*.iff;*.raw;*.brr;*.bin;" };

    if (chooser.browseForFileToOpen())
    {
        auto file = chooser.getResult();
    
        if (file.hasFileExtension("wav") || file.hasFileExtension("aif") || file.hasFileExtension("aiff") || file.hasFileExtension("bin") ||
            file.hasFileExtension("brr") || file.hasFileExtension("iff") || file.hasFileExtension("raw")  || file.hasFileExtension("smp") ||
            !file.getFileName().contains("."))
        {
            if(!loadFile(file.getFullPathName())) return false;
            return true;
        }

        juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
                "File type invalid!", "Please select a valid file type",
                                              "OK", nullptr);
        return false;
    }
    
    return false;
}

bool AmiAudioProcessor::loadFile(const juce::String& path)
{
    juce::File file = juce::File(path);

    if ((!file.exists() || file.getSize() <= 0))
    {
        juce::AlertWindow::showMessageBox(juce::AlertWindow::NoIcon,
            "File not found!", path + " not found!", "OK", nullptr);

        /* Clears out sample data if previous file isn't found */
        sampler[currentSample].clearSounds();
        
        return false;
    }

    sampler[currentSample].clearSounds();
    
    juce::String pathAPVTS = "pathname";
    pathAPVTS.append(juce::String(currentSample), 2);
    APVTS.state.setProperty(juce::Identifier(pathAPVTS), path, nullptr);

    sampleName[currentSample] = file.getFileNameWithoutExtension();
    std::unique_ptr<juce::AudioFormatReader> formatReader(formatManager.createReaderFor(file));

    if(formatReader == nullptr) return false;
    
    auto sampleLength = static_cast<int>(formatReader->lengthInSamples);
    if(sampleLength <= 1) return false;
    
    waveForm[currentSample].setSize(1, sampleLength);
    formatReader->read(&waveForm[currentSample], 0, sampleLength, 0, true, false);

    juce::BigInteger range;
    range.setRange(0, 128, true);

    sampler[currentSample].addSound(new NewSamplerSound("Sample", currentSample, *formatReader,
        range, 60, 0.1, 0.1, 10.0, *this));

    juce::StringPairArray metaData = formatReader.get()->metadataValues;

    int fileLoopEnable = 0, fileLoopStart = 0, fileLoopEnd = 0;

    if (init) return true;

    if (metaData.containsKey("Loop0Start") && metaData.containsKey("Loop0End"))
    {
        fileLoopEnable = 1;
        fileLoopStart = metaData.getValue("Loop0Start", "int").getIntValue();
        fileLoopEnd = metaData.getValue("Loop0End", "int").getIntValue() + 1;
    }

    if (!fileLoopEnable || fileLoopStart == fileLoopEnd || fileLoopEnd <= 0)
    {
        setLoopEnable(currentSample, 0);
        setLoopStart(currentSample, 0);
        setLoopEnd(currentSample, waveForm[currentSample].getNumSamples());
    }
    else
    {
        setLoopEnable(currentSample, 1);
        setLoopStart(currentSample,fileLoopStart);
        setLoopEnd(currentSample, fileLoopEnd);
    }

    sourceSampleRate[currentSample] = formatReader.get()->sampleRate;

    if(sourceSampleRate[currentSample] > 28000)
    {
        juce::String samphold = "SAMP N HOLD" + juce::String(currentSample);

        APVTS.getParameter(samphold)->beginChangeGesture();
        APVTS.getParameterAsValue(samphold).setValue(-round(sourceSampleRate[currentSample] / 16726.0));
        APVTS.getParameter(samphold)->endChangeGesture();
    }
    
    updateParams();
    
    return true;
}

void AmiAudioProcessor::setNumVoices(const int i)
{
    const int newNumVoices = numVoices[i] == 1 ? 1 : numVoices[i] == 2 ? 4 : 8;

    if (newNumVoices == sampler[i].getNumVoices()) return;

    sampler[i].clearVoices();

    for (int n = 0; n < newNumVoices; n++)
        sampler[i].addVoice(new NewSamplerVoice(*this));
}

void AmiAudioProcessor::incVibratoTable()
{
    const double vibeFreq = (vibeSpeed * 32.) / devSampleRate;
    const int vibePos = (int)floor(vibeRate);

    if (modIntensity == 0) { vibeRatio = 1.; return; }

    vibeRatio = 1.f + ((float)(128 - vibratoTable[vibePos]) * (float)modIntensity) / 409600.f;

    vibeRate += vibeFreq;
    if (vibeRate >= 32.) vibeRate = 0.;
}

void AmiAudioProcessor::initFilters()
{
    double  R  = 0., C  = 0.,
            R1 = 0., R2 = 0., 
            C1 = 0., C2 = 0., 
            cutoff  = 0., 
            qfactor = 0.;

    const double twoPi = juce::MathConstants<double>::twoPi;

    rcFilter.clearOnePoleFilterState(&a500FilterLo);

    rcFilter.clearOnePoleFilterState(&a500FilterHi);

    rcFilter.clearOnePoleFilterState(&a1200FilterHi);

    rcFilter.clearTwoPoleFilterState(&filterLED);

    R = 360.0; // R321 (360 ohm)
    C = 1e-7;  // C321 (0.1uF)
    cutoff = 1.0 / (twoPi * R * C); // ~4420.971Hz
    rcFilter.setupOnePoleFilter(devSampleRate, cutoff, &a500FilterLo);

    // A500 1-pole (6dB/oct) RC high-pass filter:
    R = 1390.0;   // R324 (1K ohm) + R325 (390 ohm)
    C = 2.233e-5; // C334 (22uF) + C335 (0.33uF)
    cutoff = 1.0 / (twoPi * R * C); // ~5.128Hz
    rcFilter.setupOnePoleFilter(devSampleRate, cutoff, &a500FilterHi);

    // A1200 1-pole (6dB/oct) RC high-pass filter:
    R = 1360.0; // R324 (1K ohm resistor) + R325 (360 ohm resistor)
    C = 2.2e-5; // C334 (22uF capacitor)
    cutoff = 1.0 / (twoPi * R * C); // ~5.319Hz
    rcFilter.setupOnePoleFilter(devSampleRate, cutoff, &a1200FilterHi);

    R1 = 10000.0; // R322 (10K ohm)
    R2 = 10000.0; // R323 (10K ohm)
    C1 = 6.8e-9;  // C322 (6800pF)
    C2 = 3.9e-9;  // C323 (3900pF)
    cutoff = 1.0 / (twoPi * std::sqrt(R1 * R2 * C1 * C2)); // ~3090.533Hz
    qfactor = std::sqrt(R1 * R2 * C1 * C2) / (C2 * (R1 + R2)); // ~0.660225
    rcFilter.setupTwoPoleFilter(devSampleRate, cutoff, qfactor, &filterLED);
}

void AmiAudioProcessor::getAmiFilter(const float* inL, const float* inR, float* outL, float* outR)
{
    float filteredL = 0.f;
    float filteredR = 0.f;

    if (isA500)
    {
        rcFilter.onePoleLPFilter(&a500FilterLo, inL, inR, &filteredL, &filteredR);
        rcFilter.onePoleHPFilter(&a500FilterHi, &filteredL, &filteredR, &filteredL, &filteredR);
    }
    else
    {
        rcFilter.onePoleHPFilter(&a1200FilterHi, inL, inR, &filteredL, &filteredR);
    }

    if( ledFilterOn )
        rcFilter.twoPoleLPFilter(&filterLED, &filteredL, &filteredR, &filteredL, &filteredR);

    *outL = filteredL;
    *outR = filteredR;

}

juce::AudioProcessorValueTreeState::ParameterLayout AmiAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    for (int i = 0; i < numSamplers; i++)
    {
        const juce::String sampleParam = juce::String(i);

        const juce::String loop_start = "Loop Start" + sampleParam;
        const juce::String loop_end = "Loop End" + sampleParam;
        const juce::String loop_enable = "Loop Enable" + sampleParam;

        const juce::String stereo_on = "Paula Stereo" + sampleParam;
        const juce::String mute_on = "Mute" + sampleParam;
        const juce::String solo_on = "Solo" + sampleParam;

        const juce::String midiChannel = "Sample MIDI Chan" + sampleParam;
        const juce::String rootNote = "Sample Root Note" + sampleParam;
        const juce::String lowNote = "Sample Low Note" + sampleParam;
        const juce::String highNote = "Sample High Note" + sampleParam;

        const juce::String sampleNhold = "Samp n Hold" + sampleParam;
        const juce::String monoPoly = "Mono Poly" + sampleParam;
        const juce::String fineTune = "Fine Tune" + sampleParam;

        const juce::String chanVol = "Channel Volume" + sampleParam;
        const juce::String chanPan = "Channel Pan" + sampleParam;
        const juce::String chanWidth = "Channel Width" + sampleParam;
        const juce::String chanGliss = "Channel Gliss" + sampleParam;

        const juce::String attack = "Attack" + sampleParam;
        const juce::String decay = "Decay" + sampleParam;
        const juce::String sustain = "Sustain" + sampleParam;
        const juce::String release = "Release" + sampleParam;

        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ loop_enable.toUpperCase(), 1 }, loop_enable, 0, 1, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ loop_start.toUpperCase(), 1 }, loop_start, 0, INT32_MAX, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ loop_end.toUpperCase(), 1 }, loop_end, 0, INT32_MAX, 0));
                                                                        
        parameters.push_back(std:: make_unique<juce::AudioParameterInt>(juce::ParameterID{ stereo_on.toUpperCase(), 1 }, stereo_on, 0, 1, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ mute_on.toUpperCase(), 1 }, mute_on, 0, 1, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ solo_on.toUpperCase(), 1 }, solo_on, 0, 1, 0));
 
        parameters.push_back(std:: make_unique<juce::AudioParameterInt>(juce::ParameterID{ midiChannel.toUpperCase(), 1 }, midiChannel, 0, 16, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ rootNote.toUpperCase(), 1 }, rootNote, 0, 127, 60));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ lowNote.toUpperCase(), 1 }, lowNote, 0, 127, 0));
        parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ highNote.toUpperCase(), 1 }, highNote, 0, 127, 127));
                                                                     
        parameters.push_back(std:: make_unique<juce::AudioParameterInt>(juce::ParameterID{ monoPoly.toUpperCase(), 1 }, monoPoly, 1, 3, 3));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ sampleNhold.toUpperCase(), 1 }, sampleNhold, juce::NormalisableRange<float>(-16.f, -1.f, 1.f), -1.f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ chanGliss.toUpperCase(), 1 }, chanGliss, juce::NormalisableRange<float>(1.f, 100.f, 1.f), 1.f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ fineTune.toUpperCase(), 1 }, fineTune, juce::NormalisableRange<float>(-50.f, 50.f, 50.f/127.f), 0.f));
 
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ chanVol.toUpperCase(), 1 }, chanVol, juce::NormalisableRange<float>(0.f, 128.f, 1.f), 64.f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ chanPan.toUpperCase(), 1 }, chanPan, juce::NormalisableRange<float>(0.f, 255.f, 1.f), 128.f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ chanWidth.toUpperCase(), 1 }, chanWidth, juce::NormalisableRange<float>(0.f, 255.f, 1.f), 255.f));

        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ attack.toUpperCase(), 1 }, attack,
            juce::NormalisableRange<float>(0.0003f, 10.0f, 0.15625f), 0.0003f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ decay.toUpperCase(), 1 }, decay,
            juce::NormalisableRange<float>(0.0f, 4.0f, 0.0625f), 0.0f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ sustain.toUpperCase(), 1 }, sustain,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.015625f), 1.0f));
        parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ release.toUpperCase(), 1 }, release,
            juce::NormalisableRange<float>(0.001f, 5.0f, 0.078125f), 0.001f));
    }

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "MASTER VOLUME", 1 }, "Master Volume", juce::NormalisableRange<float>(0.f, 64.f, 1.f), 32.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "MASTER PAN", 1 }, "Master Pan", juce::NormalisableRange<float>(0.f, 255.f, 1.f), 128.f));

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "VIBRATO SPEED", 1 }, "Vibrato Speed", juce::NormalisableRange<float>(1.f, 10.f, 0.01f), 5.f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "VIBRATO INTENSITY", 1 }, "Vibrato Intensity", juce::NormalisableRange<float>(0.f, 127.f, 1.f), 0.f));

    parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "LED FILTER", 1 }, "LED Filter", 0, 1, 0));
    parameters.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{ "MODEL TYPE", 1 }, "Model Type", 0, 1, 0));

    return { parameters.begin(), parameters.end() };
}

void AmiAudioProcessor::updateParams()
{
    if (!paramsUpdated) return;

    const juce::String sampleParam = juce::String(currentSample);

    const juce::String loop_start = "LOOP START" + sampleParam;
    const juce::String loop_end = "LOOP END" + sampleParam;
    const juce::String loop_enable = "LOOP ENABLE" + sampleParam;

    const juce::String stereo_on = "PAULA STEREO" + sampleParam;
    const juce::String mute_on = "MUTE" + sampleParam;
    const juce::String solo_on = "SOLO" + sampleParam;

    const juce::String midiChannel = "SAMPLE MIDI CHAN" + sampleParam;
    const juce::String rootNote = "SAMPLE ROOT NOTE" + sampleParam;
    const juce::String lowNote = "SAMPLE LOW NOTE" + sampleParam;
    const juce::String highNote = "SAMPLE HIGH NOTE" + sampleParam;

    const juce::String sampleNhold = "SAMP N HOLD" + sampleParam;

    const juce::String monoPoly = "MONO POLY" + sampleParam;
    const juce::String chanGliss = "CHANNEL GLISS" + sampleParam;
    const juce::String fineTune = "FINE TUNE" + sampleParam;

    const juce::String chanVol = "CHANNEL VOLUME" + sampleParam;
    const juce::String chanPan = "CHANNEL PAN" + sampleParam;
    const juce::String chanWidth = "CHANNEL WIDTH" + sampleParam;

    const juce::String attack = "ATTACK" + sampleParam;
    const juce::String decay = "DECAY" + sampleParam;
    const juce::String sustain = "SUSTAIN" + sampleParam;
    const juce::String release = "RELEASE" + sampleParam;

    const float pan = APVTS.getRawParameterValue("MASTER PAN")->load();

    masterVol = powf(APVTS.getRawParameterValue("MASTER VOLUME")->load(), 2)/powf(64, 2);

    masterPanL = pan <= 128 ? 1.f : abs(pan - 255) / 127;
    masterPanR = pan >= 128 ? 1.f : pan / 127;

    vibeSpeed = (double)APVTS.getRawParameterValue("VIBRATO SPEED")->load();
    modIntensity = (int)APVTS.getRawParameterValue("VIBRATO INTENSITY")->load();

    ledFilterOn = (int)APVTS.getRawParameterValue("LED FILTER")->load();
    isA500 = (int)APVTS.getRawParameterValue("MODEL TYPE")->load();
    
    sampleMidiChannel[currentSample] = (int)APVTS.getRawParameterValue(midiChannel)->load();
    midiRootNote[currentSample] = (int)APVTS.getRawParameterValue(rootNote)->load();
    midiLowNote[currentSample] = (int)APVTS.getRawParameterValue(lowNote)->load();
    midiHiNote[currentSample] = (int)APVTS.getRawParameterValue(highNote)->load();

    snh[currentSample] = (int)abs(APVTS.getRawParameterValue(sampleNhold)->load());

    channelVolume[currentSample] = powf(APVTS.getRawParameterValue(chanVol)->load(), 2) / powf(64, 2);

    loopEnable[currentSample] = (int)APVTS.getRawParameterValue(loop_enable)->load();
    loopStart[currentSample] = (int)APVTS.getRawParameterValue(loop_start)->load();
    loopEnd[currentSample] = (int)APVTS.getRawParameterValue(loop_end)->load();

    numVoices[currentSample] = (int)(int)APVTS.getRawParameterValue(monoPoly)->load();

    if (numVoices[currentSample] != sampler[currentSample].getNumVoices()) setNumVoices(currentSample);

    paulaStereo[currentSample] = (int)APVTS.getRawParameterValue(stereo_on)->load();

    channelGliss[currentSample] = APVTS.getRawParameterValue(chanGliss)->load();
    tune[currentSample] = APVTS.getRawParameterValue(fineTune)->load();

    channelMute[currentSample] = (int)APVTS.getRawParameterValue(mute_on)->load();
    channelSolo[currentSample] = (int)APVTS.getRawParameterValue(solo_on)->load();

    channelPan[currentSample] = paulaStereo[currentSample] ? APVTS.getRawParameterValue(chanWidth)->load() :
                                                             APVTS.getRawParameterValue(chanPan)->load();

    adsrParams.attack = APVTS.getRawParameterValue(attack)->load();
    adsrParams.sustain = APVTS.getRawParameterValue(sustain)->load();
    adsrParams.decay = APVTS.getRawParameterValue(decay)->load();
    adsrParams.release = APVTS.getRawParameterValue(release)->load();

    for (int i = 0; i < sampler[currentSample].getNumSounds(); ++i)
    {
        if (auto sound = dynamic_cast<NewSamplerSound*>(sampler[currentSample].getSound(i).get()))
        {
            sound->setEnvelopeParameters(adsrParams);
        }
    }

    paramsUpdated = false;
}

void AmiAudioProcessor::valueTreePropertyChanged(juce::ValueTree& /*treeWhosePropertyHasChanged*/, const juce::Identifier& /*property*/)
{
    paramsUpdated = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmiAudioProcessor();
}
