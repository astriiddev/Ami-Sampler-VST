/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RCFilters.h"

//==============================================================================
/**
*/

constexpr int NUM_SAMPLERS = 12;

class AmiAudioProcessor : public juce::AudioProcessor,
                          public juce::MidiMessageCollector,
                          public juce::ValueTree::Listener
{
public:
    //==============================================================================
    AmiAudioProcessor();
    ~AmiAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using juce::AudioProcessor::processBlock;

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

    void handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    bool saveFile(juce::File& file);
    void saveFileButton(const juce::String& name, std::function<void (const juce::FileChooser&)>& callback);
    void buttonLoadFile(std::function<void (const juce::FileChooser&)>&);
    bool loadFile(const juce::String& path);
    void resampleAudioData(const int, const double);

    inline juce::AudioBuffer<float>& getWaveForm(const int i) { return waveForm[i]; }

    juce::MidiKeyboardState& getKeyState() { return keyState; }
    juce::MidiMessageCollector& getMidiCollector() { return midiCollector; }

    inline void setCurrentSample(const int i) { currentSample = i; }
    inline int& getCurrentSample() { return currentSample; }

    juce::String& getSampleName(const int i) { return sampleName[i]; }
    void setSampleName(const int i, const juce::String name) { sampleName[i] = name; }
    void setSamplerEnvelopes(const int i, void* sound);

    inline juce::Synthesiser& getSampler(const int i) { return sampler[i]; }

    inline void setAVPTSvalue(const juce::String& param, const juce::var val)
    {
        APVTS.getParameter(param)->beginChangeGesture();
        APVTS.getParameterAsValue(param).setValue(val);
        APVTS.getParameter(param)->endChangeGesture();
    }

    void setLoopEnable(const int i, const int on) 
    { 
        const juce::String sampleLoopEnable = "LOOP ENABLE" + juce::String(i);
        if (init) return;
        setAVPTSvalue(sampleLoopEnable, on);
    }
    
    void setLoopStart(const int i, const int start)
    {
        const juce::String sampleLoopStart = "LOOP START" + juce::String(i);
        if (init) return;
        setAVPTSvalue(sampleLoopStart, start);
    }

    void setLoopEnd(const int i, const int end)
    {
        const juce::String sampleLoopEnd = "LOOP END" + juce::String(i);
        if (init) return;
        setAVPTSvalue(sampleLoopEnd, end);
    }

    std::atomic<int>& getLoopEnable(const int i) { return loopEnable[i]; }
    std::atomic<int>& getLoopStart(const int i) { return loopStart[i]; }
    std::atomic<int>& getLoopEnd(const int i) { return loopEnd[i]; }

    std::atomic<int>& getSamplePos() { return samplePos; }
    void setSamplePos(const int pos) { samplePos = pos; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return APVTS; }
    
    std::atomic<int>& isModelA500() { return isA500; }
    std::atomic<int>& isLEDOn() { return ledFilterOn; }

    std::atomic<int>& isMuted(const int i) { return channelMute[i]; }
    
    void setMute(const int i, const bool on)
    {
        const juce::String muteChannel = "MUTE" + juce::String(i);
        setAVPTSvalue(muteChannel, on);
        if (on) channelSolo[i] = 0;
    }

    std::atomic<int>& isSoloed(const int i) { return channelSolo[i]; }

    void setSolo(const int i, const bool on)
    {
        for (int n = 0; n < NUM_SAMPLERS; n++)
        {
            const juce::String soloChannel = "SOLO" + juce::String(n);

            APVTS.getParameter(soloChannel)->beginChangeGesture();

            if (on)
            {
                if (n == i)
                {
                    setMute(n, 0);
                    channelMute[n] = 0;
                    channelSolo[n] = 1;
                    APVTS.getParameterAsValue(soloChannel).setValue(1);
                }
                else
                {
                    setMute(n, 1);
                    channelMute[n] = 1;
                    channelSolo[n] = 0;
                    APVTS.getParameterAsValue(soloChannel).setValue(0);
                }
            }
            else
            {
                setMute(n, 0);
                channelMute[n] = 0;
                channelSolo[n] = 0;
                APVTS.getParameterAsValue(soloChannel).setValue(0);
            }

            APVTS.getParameter(soloChannel)->endChangeGesture();
        }
    }

    std::atomic<int>& paulaStereoOn(const int i) { return paulaStereo[i]; }

    void incPanCount(const int i) { if (paulaStereo[i]) { panCounter[i] >= 7 ? panCounter[i] = 0 : panCounter[i]++; } }

    bool shouldPan(const int i, const int l)
    {
        if (!paulaStereo[i]) return false;

        if (panCounter[i] % 2 == l) return false;

        return true;
    }

    std::atomic<int>& getMidiChannel(const int i) { return sampleMidiChannel[i]; }

    void setMidiChannel(const int i, const int channel)
    {
        const juce::String midiChannel = "SAMPLE MIDI CHAN" + juce::String(i);
        setAVPTSvalue(midiChannel, channel);
    }

    std::atomic<int>& getRootNote(const int i) { return midiRootNote[i]; }

    void setRootNote(const int i, const int note)
    {
        const juce::String rootNote = "SAMPLE ROOT NOTE" + juce::String(i);
        setAVPTSvalue(rootNote, note);
    }

    std::atomic<int>& getLowNote(const int i) { return midiLowNote[i]; }

    void setLowNote(const int i, const int note)
    {
        const juce::String lowNote = "SAMPLE LOW NOTE" + juce::String(i);
        setAVPTSvalue(lowNote, note);
    }

    std::atomic<int>& getHighNote(const int i) { return midiHiNote[i]; }

    void setHighNote(const int i, const int note)
    {
        const juce::String hiNote = "SAMPLE HIGH NOTE" + juce::String(i);
        setAVPTSvalue(hiNote, note);
    }

    void setMonoPoly(const int i, const int newNumVoices)
    {
        const juce::String monoPoly = "MONO POLY" + juce::String(i);
        setAVPTSvalue(monoPoly, newNumVoices);
    }

    inline std::atomic<double>& getDevSampleRate() { return devSampleRate; }

    std::atomic<int>&   getSnH(const int i) { return snh[i]; }

    std::atomic<float>& getFineTune(const int i) { return tune[i]; }
    std::atomic<float>& getVibrato() { return vibeRatio; }
    std::atomic<float>& getGlissando(const int i) { return channelGliss[i]; }

    std::atomic<float>& getChanVol(const int i) { return channelVolume[i]; }
    std::atomic<float>& getChanPan(const int i) { return channelPan[i]; }

    inline void decScaleFactor() { scaleFactor = scaleFactor > 0.25f ? scaleFactor - 0.25f : 0.25f; }
    inline void incScaleFactor() { scaleFactor = scaleFactor < 1.75f ? scaleFactor + 0.25f : 2.f; }
    inline void resetScaleFactor() { scaleFactor = 1.f; }
    inline float& getCurrentScaleFactor() { return scaleFactor; }

    inline void setBaseOctave(const int octave) { baseOctave = octave; }
    inline int& getBaseOctave() { return baseOctave; }

    inline void setSourceSampleRate(const int i, const double rate) 
    { 
        sourceSampleRate[i] = rate; 
        APVTS.state.setProperty(juce::Identifier("samplerate" + juce::String(i)), rate, nullptr);
    }

    inline void setPingPongLoop(const int i, const int pingpong)
    {
        pingpongLoop[i] = pingpong;
        APVTS.state.setProperty(juce::Identifier("pingpongLoop" + juce::String(currentSample)), pingpongLoop[i].load(), nullptr);
    }

    inline std::atomic<int>& getPingPongLoop(const int i) { return pingpongLoop[i]; }

    inline std::atomic<double>& getSourceSampleRate(const int i) { return sourceSampleRate[i]; }

    inline void setResampleRate(const int i, const double rate) { resampleRate[i] = rate; }
    inline std::atomic<double>& getResampleRate(const int i) { return resampleRate[i]; }

    inline bool& isHostPlaying() { return hostIsPlaying; }

    inline bool& extendedOptionsShowing() { return showExtendedOptions; }
    inline void  switchOptions() { showExtendedOptions ^= 1; }
    inline bool& showHexVal() { return textInHex; }
    inline void  switchValType(const bool type) { textInHex = type; }

private:

    void initFilters();
    void getAmiFilter(const float *inL, const float* inR, float *outL, float *outR);
    void setNumVoices(const int i);
    void incVibratoTable();

    const uint8_t vibratoTable[32] =
    {
        0xFF, 0xFD, 0xFA, 0xF4, 0xEB, 0xE0, 0xD4, 0xC5,
        0xB4, 0xA1, 0x8D, 0x78, 0x61, 0x4A, 0x31, 0x18,
        0x00, 0x18, 0x31, 0x4A, 0x61, 0x78, 0x8D, 0xA1,
        0xB4, 0xC5, 0xD4, 0xE0, 0xEB, 0xF4, 0xFA, 0xFD
    };

    float scaleFactor = 0.75f;
    int baseOctave = 5;

    juce::Synthesiser sampler[NUM_SAMPLERS];
    juce::String sampleName[NUM_SAMPLERS];
    juce::AudioBuffer<float> waveForm[NUM_SAMPLERS];

    std::unique_ptr<juce::FileChooser> myChooser = nullptr;
    juce::String lastFileDir;

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    RCFilter rcFilter;
    RCFilter::OnePoleFilter_t a500FilterLo, a500FilterHi, a1200FilterHi;
    RCFilter::TwoPoleFilter_t filterLED;

    int currentSample = 0, modIntensity = 0, panCounter[NUM_SAMPLERS];
    int numVoices[NUM_SAMPLERS];
    juce::BigInteger voiceRange;

    juce::MidiMessageCollector midiCollector;
    juce::MidiBuffer midiBuffer;
    juce::MidiKeyboardState keyState;
    juce::AudioFormatManager formatManager;

    juce::AudioProcessorValueTreeState APVTS;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> listener;

    std::unique_ptr<juce::AudioParameterInt>   createParam(const juce::String&, const int, const int, const int);
    std::unique_ptr<juce::AudioParameterFloat> createParam(const juce::String&, const float&, const float&, const float&, const float);
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override;
    
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, int*);
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, std::atomic<int>*);
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, float*);
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, std::atomic<float>*);
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, double*);
    bool changeValueTreeParam(const juce::String&, const juce::String&, const juce::var&, std::atomic<double>*);

    bool init = true, hostIsPlaying = false, showExtendedOptions = false, textInHex = true;

    double vibeRate = 0.f;
    std::atomic<double> vibeSpeed = 5.f, devSampleRate = 44100.f;

    std::atomic<float> masterVol = 1.f, masterPanL = 1.f, masterPanR = 1.f, 
                       channelGliss[NUM_SAMPLERS], tune[NUM_SAMPLERS], vibeRatio = 1.f;

    std::atomic<float> channelVolume[NUM_SAMPLERS], channelPan[NUM_SAMPLERS],
                       channelAttack[NUM_SAMPLERS], channelDecay[NUM_SAMPLERS],
                       channelSustain[NUM_SAMPLERS], channelRelease[NUM_SAMPLERS];

    std::atomic<int> sampleMidiChannel[NUM_SAMPLERS], midiRootNote[NUM_SAMPLERS], midiLowNote[NUM_SAMPLERS], midiHiNote[NUM_SAMPLERS];

    std::atomic<int> loopStart[NUM_SAMPLERS], loopEnd[NUM_SAMPLERS], loopEnable[NUM_SAMPLERS], snh[NUM_SAMPLERS], pingpongLoop[NUM_SAMPLERS];

    std::atomic<double> sourceSampleRate[NUM_SAMPLERS], resampleRate[NUM_SAMPLERS];

    std::atomic<int> samplePos = 0;
    std::atomic<int> isA500 = 0, ledFilterOn = 0;
    std::atomic<int> paulaStereo[NUM_SAMPLERS], channelMute[NUM_SAMPLERS], channelSolo[NUM_SAMPLERS];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiAudioProcessor)
};
