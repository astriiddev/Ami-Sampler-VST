/*
  ==============================================================================

    NewSamplerVoice.h
    Created: 16 May 2023 8:24:19pm
    Author:  finle

  ==============================================================================
*/

#pragma once
/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
  ==============================================================================


  //// Reimplementation of JUCE's SamplerSound and SamplerVoice to include looping and nearest-neighbor interpolation ////


  ==============================================================================
*/

class NewSamplerSound    : public juce::SynthesiserSound
{
public:
    //==============================================================================
    /** Creates a sampled sound from an audio reader.

        This will attempt to load the audio from the source into memory and store
        it in this object.

        @param name         a name for the sample
        @param source       the audio to load. This object can be safely deleted by the
                            caller after this constructor returns
        @param midiNotes    the set of midi keys that this sound should be played on. This
                            is used by the SynthesiserSound::appliesToNote() method
        @param midiNoteForNormalPitch   the midi note at which the sample should be played
                                        with its natural rate. All other notes will be pitched
                                        up or down relative to this one
        @param attackTimeSecs   the attack (fade-in) time, in seconds
        @param releaseTimeSecs  the decay (fade-out) time, in seconds
        @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
                                        source, in seconds
    */
    NewSamplerSound (const juce::String& name,
                  juce::AudioFormatReader& source,
                  const juce::BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds, AmiSamplerAudioProcessor&);

    /** Destructor. */
    ~NewSamplerSound() override;

    //==============================================================================
    /** Returns the sample's name */
    const juce::String& getName() const noexcept                  { return name; }

    /** Returns the audio sample data.
        This could return nullptr if there was a problem loading the data.
    */
    juce::AudioBuffer<float>* getAudioData() const noexcept       { return data.get(); }

    //==============================================================================
    /** Changes the parameters of the ADSR envelope which will be applied to the sample. */
    void setEnvelopeParameters (juce::ADSR::Parameters parametersToUse)    { params = parametersToUse; }

    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;

private:
    //==============================================================================
    friend class NewSamplerVoice;

    juce::String name;
    std::unique_ptr<juce::AudioBuffer<float>> data;
    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int length = 0, midiRootNote = 0;

    juce::ADSR::Parameters params;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_LEAK_DETECTOR (NewSamplerSound)
};


//==============================================================================
/**
    A subclass of SynthesiserVoice that can play a SamplerSound.

    To use it, create a Synthesiser, add some SamplerVoice objects to it, then
    give it some SampledSound objects to play.

    @see SamplerSound, Synthesiser, SynthesiserVoice

    @tags{Audio}
*/
class NewSamplerVoice    : public juce::SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    NewSamplerVoice(AmiSamplerAudioProcessor&);

    /** Destructor. */
    ~NewSamplerVoice() override;

    //==============================================================================
    bool canPlaySound (juce::SynthesiserSound*) override;

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;

    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    using juce::SynthesiserVoice::renderNextBlock;

    double& getSamplePos() { return mSamplePos; }
    void setSamplePos(double pos) { mSamplePos = pos; }

    int& setLoopStart(int loopStart) { return mLoopStart = loopStart; }
    int& setLoopEnd(int loopEnd) { return mLoopEnd = loopEnd; }

    int getLoopStart() const { return mLoopStart; }
    int getLoopEnd() const { return mLoopEnd; }

private:
    //==============================================================================
    double pitchRatio = 0;
    
    double sourceSamplePosition = 0.0f;
    float lgain = 0, rgain = 0;

    /* Loop point and playhead tracker initialization */
    double mSamplePos = 0.0f;
    int mLoopStart = 0;
    int mLoopEnd = 0;

    juce::ADSR adsr;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_LEAK_DETECTOR (NewSamplerVoice)
};
