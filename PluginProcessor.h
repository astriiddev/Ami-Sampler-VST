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

    void saveFile(const juce::String& name);
    bool buttonLoadFile();
    bool loadFile(const juce::String& path);

    inline juce::AudioBuffer<float>& getWaveForm(const int i) { return waveForm[i]; }

    juce::MidiKeyboardState& getKeyState() { return keyState; }
    juce::MidiMessageCollector& getMidiCollector() { return midiCollector; }

    inline void setCurrentSample(const int i) { currentSample = i; }
    inline int& getCurrentSample() { return currentSample; }

    juce::String& getSampleName(const int i) { return sampleName[i]; }
    void setSampleName(const int i, const juce::String name) { sampleName[i] = name; }

    inline juce::Synthesiser& getSampler(const int i) { return sampler[i]; }

    void setLoopEnable(const int i, const int on) 
    { 
        const juce::String sampleLoopEnable = "LOOP ENABLE" + juce::String(i);

        if (init) return;

        APVTS.getParameter(sampleLoopEnable)->beginChangeGesture();
        APVTS.getParameterAsValue(sampleLoopEnable).setValue(on);
        APVTS.getParameter(sampleLoopEnable)->endChangeGesture();
    }
    
    void setLoopStart(const int i, const int start)
    {
        const juce::String sampleLoopStart = "LOOP START" + juce::String(i);

        if (init) return;

        APVTS.getParameter(sampleLoopStart)->beginChangeGesture();
        APVTS.getParameterAsValue(sampleLoopStart).setValue(start);
        APVTS.getParameter(sampleLoopStart)->endChangeGesture();
    }

    void setLoopEnd(const int i, const int end)
    {
        const juce::String sampleLoopEnd = "LOOP END" + juce::String(i);

        if (init) return;

        APVTS.getParameter(sampleLoopEnd)->beginChangeGesture();
        APVTS.getParameterAsValue(sampleLoopEnd).setValue(end);
        APVTS.getParameter(sampleLoopEnd)->endChangeGesture();
    }

    std::atomic<int>& getLoopEnable(const int i) { return loopEnable[i]; }
    std::atomic<int>& getLoopStart(const int i) { return loopStart[i]; }
    std::atomic<int>& getLoopEnd(const int i) { return loopEnd[i]; }

    int& getSamplePos() { return samplePos; }
    void setSamplePos(const int pos) { samplePos = pos; }

    juce::AudioProcessorValueTreeState& getAPVTS() { return APVTS; }
    void updateParams();
    
    int& isModelA500() { return isA500; }
    int& isLEDOn() { return ledFilterOn; }

    int& isMuted(const int i) { return channelMute[i]; }
    
    void setMute(const int i, const bool on)
    {
        const juce::String muteChannel = "MUTE" + juce::String(i);

        APVTS.getParameter(muteChannel)->beginChangeGesture();
        APVTS.getParameterAsValue(muteChannel).setValue(on);
        APVTS.getParameter(muteChannel)->endChangeGesture();

        if (on)
            channelSolo[i] = 0;
    }

    int& isSoloed(const int i) { return channelSolo[i]; }

    void setSolo(const int i, const bool on)
    {
        for (int n = 0; n < numSamplers; n++)
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

    int& paulaStereoOn(const int i) { return paulaStereo[i]; }

    void incPanCount(const int i) { if (paulaStereo[i]) { panCounter[i] >= 7 ? panCounter[i] = 0 : panCounter[i]++; } }

    bool shouldPan(const int i, const int l)
    {
        if (!paulaStereo[i]) return false;

        if (panCounter[i] % 2 == l) return false;

        return true;
    }

    int& getMidiChannel(const int i) { return sampleMidiChannel[i]; }

    void setMidiChannel(const int i, const int channel)
    {
        const juce::String midiChannel = "SAMPLE MIDI CHAN" + juce::String(i);

        APVTS.getParameter(midiChannel)->beginChangeGesture();
        APVTS.getParameterAsValue(midiChannel).setValue(channel);
        APVTS.getParameter(midiChannel)->endChangeGesture();
    }

    int& getRootNote(const int i) { return midiRootNote[i]; }

    void setRootNote(const int i, const int note)
    {
        const juce::String rootNote = "SAMPLE ROOT NOTE" + juce::String(i);

        APVTS.getParameter(rootNote)->beginChangeGesture();
        APVTS.getParameterAsValue(rootNote).setValue(note);
        APVTS.getParameter(rootNote)->endChangeGesture();
    }

    int& getLowNote(const int i) { return midiLowNote[i]; }

    void setLowNote(const int i, const int note)
    {
        const juce::String lowNote = "SAMPLE LOW NOTE" + juce::String(i);

        APVTS.getParameter(lowNote)->beginChangeGesture();
        APVTS.getParameterAsValue(lowNote).setValue(note);
        APVTS.getParameter(lowNote)->endChangeGesture();
    }

    int& getHighNote(const int i) { return midiHiNote[i]; }

    void setHighNote(const int i, const int note)
    {
        const juce::String hiNote = "SAMPLE HIGH NOTE" + juce::String(i);

        APVTS.getParameter(hiNote)->beginChangeGesture();
        APVTS.getParameterAsValue(hiNote).setValue(note);
        APVTS.getParameter(hiNote)->endChangeGesture();
    }

    void setMonoPoly(const int i, const int newNumVoices)
    {
        const juce::String monoPoly = "MONO POLY" + juce::String(i);

        APVTS.getParameter(monoPoly)->beginChangeGesture();
        APVTS.getParameterAsValue(monoPoly).setValue(newNumVoices);
        APVTS.getParameter(monoPoly)->endChangeGesture();
    }

    inline double& getDevSampleRate() { return devSampleRate; }

    std::atomic<int>&   getSnH(const int i) { return snh[i]; }

    std::atomic<float>& getFineTune(const int i) { return tune[i]; }
    std::atomic<float>& getVibrato() { return vibeRatio; }
    std::atomic<float>& getGlissando(const int i) { return channelGliss[i]; }

    std::atomic<float>& getChanVol(const int i) { return channelVolume[i]; }
    std::atomic<float>& getChanPan(const int i) { return channelPan[i]; }

    inline void decScaleFactor() { scaleFactor = scaleFactor - 0.25f > 0.f ? scaleFactor - 0.25f : 0.25f; }
    inline void incScaleFactor() { scaleFactor = scaleFactor + 0.25f < 2.f ? scaleFactor + 0.25f : 2.f; }
    inline void resetScaleFactor() { scaleFactor = 1.f; }
    inline float& getCurrentScaleFactor() { return scaleFactor; }

    inline void setBaseOctave(const int octave) { baseOctave = octave; }
    inline int& getBaseOctave() { return baseOctave; }

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

    juce::Synthesiser sampler[12];
    juce::String sampleName[12];
    juce::AudioBuffer<float> waveForm[12];

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    RCFilter rcFilter;
    RCFilter::OnePoleFilter_t a500FilterLo, a500FilterHi, a1200FilterHi;
    RCFilter::TwoPoleFilter_t filterLED;

    int currentSample = 0, modIntensity = 0, panCounter[12];
    int numVoices[12];
    const int numSamplers = 12;

    juce::MidiMessageCollector midiCollector;
    juce::MidiBuffer midiBuffer;
    juce::MidiKeyboardState keyState;
    juce::AudioFormatManager formatManager;

    juce::AudioProcessorValueTreeState APVTS;
    std::unique_ptr<juce::AudioProcessorValueTreeState::Listener> listener;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
        const juce::Identifier& property) override;

    bool init = true, paramsUpdated = false;

    double vibeSpeed = 5.f, vibeRate = 0.f, devSampleRate = 0.f;

    std::atomic<float> masterVol = 1.f, masterPanL = 1.f, masterPanR = 1.f, 
                       channelGliss[12], tune[12], vibeRatio = 1.f;

    std::atomic<float> channelVolume[12], channelPan[12],
                       channelAttack[12], channelDecay[12],
                       channelSustain[12], channelRelease[12];

    int sampleMidiChannel[12], midiRootNote[12], midiLowNote[12], midiHiNote[12];

    std::atomic<int> loopStart[12], loopEnd[12], loopEnable[12], snh[12];

    double sourceSampleRate[12];

    int samplePos = 0;
    int isA500 = 0, ledFilterOn = 0;
    int paulaStereo[12], channelMute[12], channelSolo[12];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmiAudioProcessor)
};
