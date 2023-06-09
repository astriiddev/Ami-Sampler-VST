/*
  ==============================================================================

    NewSamplerVoice.cpp
    Created: 16 May 2023 8:24:19pm
    Author:  _astriid_

  ==============================================================================
*/

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
#include "NewSamplerVoice.h"

/*
  ==============================================================================


  //// Reimplementation of JUCE's Sampler sound and sampler voice to include looping and nearest-neighbor interpolation ////


  ==============================================================================
*/

NewSamplerSound::NewSamplerSound (const juce::String& soundName,
                        juce::AudioFormatReader& source,
                        const juce::BigInteger& notes,
                        int midiNoteForNormalPitch,
                        double attackTimeSecs,
                        double releaseTimeSecs,
                        double maxSampleLengthSeconds, AmiSamplerAudioProcessor& p)
    : name (soundName),
      sourceSampleRate (16726), //sets sample rate to 16726hz, the default for Amiga samples
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch), audioProcessor(p)
{
    /* Sampler sound initialization */
    //!!!! TODO: add GUI control for user to adjust base sample rate !!!!//
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

NewSamplerSound::~NewSamplerSound()
{
}

bool NewSamplerSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool NewSamplerSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

//==============================================================================
NewSamplerVoice::NewSamplerVoice(AmiSamplerAudioProcessor& p) : mRCFilter(p), audioProcessor(p) 
{
    mRCFilter.clearOnePoleFilterState(&filterLo);
    mRCFilter.clearOnePoleFilterState(&filterHi);
    mRCFilter.clearTwoPoleFilterState(&filterLED);
}

NewSamplerVoice::~NewSamplerVoice() {}

bool NewSamplerVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<const NewSamplerSound*> (sound) != nullptr;
}

void NewSamplerVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int pitchwheel)
{
    
    if (auto* sound = dynamic_cast<const NewSamplerSound*> (s))
    {
        pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
                               * sound->sourceSampleRate / getSampleRate();

        sourceSamplePosition = 0.0;
        mSamplePos = 0.0f;

        /* auto-pan counter intitialazion */
        static uint8_t counter = 0;

        /* Only activates if PaulaStereo button is pressed and is not in monophonic mode */
        if (audioProcessor.getNumVoiceState() != 1 && audioProcessor.isStereo())
        {
            /* first note left, second note right, and continues to switch back and forth */
            
            if (counter % 2 == 0)
            {
                lgain = velocity;
                rgain = 0;
            }
            if (counter % 2 != 0)
            {
                lgain = 0;
                rgain = velocity;
            }

            /* resets counter so it doesn't count forever */
            counter == 8 ? counter = 1 : counter++;

        }
        else
        {
            counter = 0;
            lgain = velocity;
            rgain = velocity;
        }

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void NewSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void NewSamplerVoice::pitchWheelMoved (int newValue) {}
void NewSamplerVoice::controllerMoved (int controllerNumber, int newValue) {}

//==============================================================================
void NewSamplerVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    static const double twoPi = juce::MathConstants<double>::twoPi;

    static double dPaulaOutputFreq = audioProcessor.getSampleRate();

    if (dPaulaOutputFreq <= 0.0)
        dPaulaOutputFreq = 44100.0;

    static double R, C, R1, R2, C1, C2, cutoff, qfactor;

    if (audioProcessor.isModelA500())
    {

        R = 360.0; // R321 (360 ohm)
        C = 1e-7;  // C321 (0.1uF)
        cutoff = 1.0 / (twoPi * R * C); // ~4420.971Hz
        mRCFilter.setupOnePoleFilter(audioProcessor.getSampleRate(), cutoff, &filterLo);

        // A500 1-pole (6dB/oct) RC high-pass filter:
        R = 1390.0;   // R324 (1K ohm) + R325 (390 ohm)
        C = 2.233e-5; // C334 (22uF) + C335 (0.33uF)
        cutoff = 1.0 / (twoPi * R * C); // ~5.128Hz
        mRCFilter.setupOnePoleFilter(dPaulaOutputFreq, cutoff, &filterHi);
    }
    else
    {
        mRCFilter.clearOnePoleFilterState(&filterLo);

        // A1200 1-pole (6dB/oct) RC high-pass filter:
        R = 1360.0; // R324 (1K ohm resistor) + R325 (360 ohm resistor)
        C = 2.2e-5; // C334 (22uF capacitor)
        cutoff = 1.0 / (twoPi * R * C); // ~5.319Hz
        mRCFilter.setupOnePoleFilter(dPaulaOutputFreq, cutoff, &filterHi);
    }

    // 2-pole (12dB/oct) RC low-pass filter ("LED" filter, same values on A500/A1200):
    R1 = 10000.0; // R322 (10K ohm)
    R2 = 10000.0; // R323 (10K ohm)
    C1 = 6.8e-9;  // C322 (6800pF)
    C2 = 3.9e-9;  // C323 (3900pF)
    cutoff = 1.0 / (twoPi * std::sqrt(R1 * R2 * C1 * C2)); // ~3090.533Hz
    qfactor = std::sqrt(R1 * R2 * C1 * C2) / (C2 * (R1 + R2)); // ~0.660225
    mRCFilter.setupTwoPoleFilter(dPaulaOutputFreq, cutoff, qfactor, &filterLED);


    if (auto* playingSound = static_cast<NewSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        /* Sets start and end loop from GUI controls */
        setLoopStart(audioProcessor.getLoopStart());
        setLoopEnd(audioProcessor.getLoopEnd());

        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer(0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        float* outL = outputBuffer.getWritePointer(0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;
        
        while (--numSamples >= 0)
        {
            int pos = (int)sourceSamplePosition;

            if (pos <= 0)
                pos = 0;

            if (sourceSamplePosition <= 0)
                sourceSamplePosition = 0.0f;

            /* Length of the loop. Not doing anything with it now but may use it in a GUI textbox */
            static int loopLength = getLoopEnd() - getLoopStart();

            /* JUCE's linear interpolation. May add back in and make toggleable */

            /*auto alpha = (float) (sourceSamplePosition - pos);
            /*auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                                       : l;*/

            float l = inL[pos];
            float r = l;

            /* Filter selection */
            if(audioProcessor.isModelA500())
            {
                l = mRCFilter.onePoleLPFilter(&filterLo, l);

                l = mRCFilter.onePoleHPFilter(&filterHi, l);
            }
            else
                l = mRCFilter.onePoleHPFilter(&filterHi, inL[pos]);

            if (audioProcessor.isLEDOn())
                l = mRCFilter.twoPoleLPFilter(&filterLED, l);

            r = l;
            

            auto envelopeValue = adsr.getNextSample();

            /* Amiga output has a 180 degree phase from original waveform so ( * -1 ) flips the phase */
            l *= (lgain * envelopeValue) * -1;
            r *= (rgain * envelopeValue) * -1;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio;

            /* Playhead tracker reset with new note */
            if (!audioProcessor.isNewNote() || !adsr.isActive())
                mSamplePos = 0;
            
            mSamplePos += pitchRatio;

            /* Loop point enable and disable. One shot if disable, Forward looping if disabled */
            //!!!! TODO: Add option for Ping-Pong looping !!!!//
            if (audioProcessor.getLoopEnable())
                if (sourceSamplePosition >= mLoopEnd)
                {
                    sourceSamplePosition = mLoopStart;
                    mSamplePos = mLoopStart;
                }

            /* Sends playhead position to processor to be used in GUI */
            audioProcessor.setSamplePos((int)mSamplePos);

            /* Stops note if ADSR is no longer active or if sample numbers run out */
            if (!adsr.isActive() || sourceSamplePosition > playingSound->length)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}