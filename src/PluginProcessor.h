/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
class AmiSamplerAudioProcessor : public juce::AudioProcessor,
    public juce::ValueTree::Listener,
    public juce::MidiKeyboardStateListener
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    AmiSamplerAudioProcessor();
    ~AmiSamplerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    /* Audio file loading */
    void loadFile(const juce::String& path);
    void buttonLoadFile();
    void saveFile(const juce::String&);

    bool& isNewFile() { return newFile; }
    void setNewFile(bool isNew) { newFile = isNew; }
    bool& loadingSaveFile() { return loadSavedFile; }

    /* Sampler initialization */
    int getNumSamplerSounds() { return mSampler.getNumSounds(); }
    juce::AudioBuffer<float>& getWaveForm() { return mWaveForm; }
    juce::Synthesiser& getSampler() { return mSampler; }
    juce::String& getSampleName() { return mSampleName; }
    juce::String& getFilePath() { return mFilePath; }

    /* ADSR initialization and update callback */
    void updateADSR();
    juce::ADSR::Parameters& getADSRParams() { return mADSRParams; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return mAPVTS; }

    /* Virtual MIDI Keybaord input and MIDI Callback */
    juce::MidiKeyboardState& getKeyState() { return keyState; }
    juce::MidiBuffer& getMidiBuffer() { return mMidiBuffer; }
    juce::MidiMessageCollector& getMidiCollector() { return midiCollector; }

    void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    void setBaseOctave(int baseOctave) { mBaseOctave = baseOctave; }
    int& getBaseOctave() { return mBaseOctave; }

    /* Loop point start and enable callbacks */
    void setSliderStart(double sliderStart) { mSliderStart = sliderStart; }
    void setSliderEnd(double sliderEnd) { mSliderEnd = sliderEnd; }

    double& getSliderStart() { return mSliderStart; }
    double& getSliderEnd() { return mSliderEnd; }

    int& setLoopStart(int loopStart) { return mLoopStart = loopStart; }
    int& setLoopEnd(int loopEnd) { return mLoopEnd = loopEnd; }

    int getLoopStart() const { return mLoopStart; }
    int getLoopEnd() const { return mLoopEnd; }

    void setLoopEnable(bool isOn) { mLoopEnable = isOn; }
    std::atomic<bool>& getLoopEnable() { return mLoopEnable; }
    std::atomic<int>& getSamplePos() { return mSamplePos; }

    void setSamplePos(int currentSample) { mSamplePos = currentSample; }
    std::atomic<bool>& isNotePlayed() { return mIsNotePlayed; }
    std::atomic<bool>& isNewNote() { return mNewNote; }

    std::atomic<bool>& isModelA500() { return mA500; }
    void setModelType(bool isA500) { mA500 = isA500; }

    std::atomic<bool>& isLEDOn() { return mFilterOn; }
    void setFilterOn(bool filterOn) { mFilterOn = filterOn; }

    std::atomic<int>& getNumVoiceState() { return mNumVoiceState; }
    void setNumVoiceState(int numVoiceState) { mNumVoiceState = numVoiceState; }

    std::atomic<bool>& isStereo() { return mStereo; }
    void setStereo(bool stereo) { mStereo = stereo; }

private:

    juce::Synthesiser mSampler;
    const int mNumVoices{ 8 };
    juce::AudioBuffer<float> mWaveForm;
    juce::String mSampleName, mFilePath;
    
    bool newFile{ false }, loadSavedFile{ false };
    double fileSampleRate{ 0 }, fileBitDepth{ 8 };

    /* Loop point value intialization */
    int mLoopStart{ 0 }, mLoopEnd{ 0 };
    double mSliderStart{ 0.0f }, mSliderEnd{ 1.0f };

    /* ADSR intialization */
    juce::ADSR adsr;
    double rawVolume{ 1.0f }, leftPan{ 1.0f }, rightPan{ 1.0f };
    juce::ADSR::Parameters mADSRParams;
    juce::AudioFormatManager mFormatManager;

    /* Value tree intialization: for recalling previous session settings */
    juce::AudioProcessorValueTreeState mAPVTS;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged, const juce::Identifier& property) override;


    /* Virtual MIDI Keybaord input and MIDI Callback initialization */
    juce::MidiMessageCollector midiCollector;
    juce::MidiBuffer mMidiBuffer;
    juce::MidiKeyboardState keyState;
    int mBaseOctave{ 5 };
    
    std::atomic<int>  mSamplePos{ 0 }, mNumVoiceState{ 0 };
    std::atomic<bool> mA500{ false }, mFilterOn{ false }, mStereo{ false }, mNewNote{ false },
                      mShouldUpdate{ false }, mIsNotePlayed{ false }, mLoopEnable{ false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiSamplerAudioProcessor)
};
