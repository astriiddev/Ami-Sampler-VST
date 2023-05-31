/*
  ==============================================================================

    NewSamplerVoice.cpp
    Created: 16 May 2023 8:24:19pm
    Author:  finle

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
NewSamplerVoice::NewSamplerVoice(AmiSamplerAudioProcessor& p) : mLoopStart(), mLoopEnd(), audioProcessor(p) 
{   
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

        lgain = velocity;
        rgain = velocity;

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

            /* Simple nearest-neighbor interpolation */
            //!!!! TODO: add toggleable Amiga-500 style RC filter and toggleable LED Butterworth filter !!!!//
            float l = inL[pos];
            float r = (inR != nullptr) ? inR[pos] : l;

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
            {
                mSamplePos = 0;
            }
            
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