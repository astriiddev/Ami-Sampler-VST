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

NewSamplerSound::NewSamplerSound (const juce::String& soundName, int sampleNumber,
                        juce::AudioFormatReader& source,
                        const juce::BigInteger& notes,
                        int midiNoteForNormalPitch,
                        double attackTimeSecs,
                        double releaseTimeSecs,
                        double maxSampleLengthSeconds, AmiAudioProcessor& p)
    : name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch), audioProcessor(p)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);

        currentSample = sampleNumber;
    }
}

NewSamplerSound::~NewSamplerSound()
{
}

bool NewSamplerSound::appliesToNote (int midiNoteNumber)
{
    if (midiNoteNumber < audioProcessor.getLowNote(currentSample)) return false;
    if (midiNoteNumber > audioProcessor.getHighNote(currentSample)) return false;

    return true;
}

bool NewSamplerSound::appliesToChannel (int midiChannel)
{
    const int channel2Play = audioProcessor.getMidiChannel(currentSample);

    if (channel2Play <= 0) return true;
    if (channel2Play != midiChannel) return false;

    return true;
}

//==============================================================================
NewSamplerVoice::NewSamplerVoice(AmiAudioProcessor& p) : audioProcessor(p)
{
}

NewSamplerVoice::~NewSamplerVoice() {}

bool NewSamplerVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<const NewSamplerSound*> (sound) != nullptr;
}

void NewSamplerVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int pitchwheel)
{
    if (auto* sound = dynamic_cast<NewSamplerSound*> (s))
    {
        currentSample = sound->currentSample;

        sound->midiRootNote = 120 - audioProcessor.getRootNote(currentSample);

        if (sourceSamplePosition >= sound->length) releasedNote = true;

        numVoices = audioProcessor.getSampler(currentSample).getNumVoices();

        bendRatio = std::pow(2., ((double) newValue - 8192.0) / 49152.0);
        fineTune = audioProcessor.getFineTune(currentSample) / 1200.;

        pitchTarget = std::pow(2.0, (double)(midiNoteNumber - sound->midiRootNote) / 12.0) 
            * sound->sourceSampleRate / audioProcessor.getSampleRate();

        glissRatio = (pitchTarget - pitchRatio) / audioProcessor.getGlissando(currentSample);
        slideUp = pitchTarget > pitchRatio ? true : false;

        lgain = audioProcessor.shouldPan(currentSample, 0) ? 0 : velocity;
        rgain = audioProcessor.shouldPan(currentSample, 1) ? 0 : velocity;

        audioProcessor.incPanCount(currentSample);

        adsr.setSampleRate(sound->sourceSampleRate);
        adsr.setParameters(sound->params);

        if (releasedNote || numVoices > 1 || audioProcessor.getGlissando(currentSample) <= 1)
        {
            sourceSamplePosition = 0.0;

            pitchRatio = pitchTarget;
            releasedNote = false;

            adsr.noteOn();
        }
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
        releasedNote = true;
    }
    else
    {
        clearCurrentNote();
        if (releasedNote || (numVoices <= 1 && audioProcessor.getGlissando(currentSample) <= 1)) adsr.reset();
    }

    audioProcessor.setSamplePos(0);
}

void NewSamplerVoice::pitchWheelMoved(int newValue)
{
    bendRatio = std::pow(2., ((double) newValue - 8192.0) / 49152.0);
}

void NewSamplerVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void NewSamplerVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<NewSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        /* Sets start and end loop from GUI controls */
        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer(0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        const int  loopStart = audioProcessor.getLoopStart(currentSample), 
                   loopEnd    = audioProcessor.getLoopEnd(currentSample);
        const bool loopEnable = audioProcessor.getLoopEnable(currentSample);
        const bool isSilenced = audioProcessor.isMuted(currentSample);

        float* outL = outputBuffer.getWritePointer(0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

        const float vol = audioProcessor.getChanVol(currentSample);

        fineTune = 1. + audioProcessor.getFineTune(currentSample) / 1200.;

        if (audioProcessor.getGlissando(currentSample) == 1 || numVoices > 1) pitchRatio = pitchTarget;

        while (--numSamples >= 0)
        {
            int pos = (int)sourceSamplePosition;

            if (pos <= 0)
                pos = 0;

            if (sourceSamplePosition <= 0)
                sourceSamplePosition = 0.0f;

            if(currentSample == audioProcessor.getCurrentSample())
                audioProcessor.setSamplePos(pos);

            if(lgain <= 0 && rgain <= 0)
                audioProcessor.setSamplePos(0);

            if (juce::Time::getMillisecondCounter() > glissandoTimer && pitchRatio != pitchTarget)
            {
                pitchRatio += glissRatio;
                glissandoTimer = juce::Time::getMillisecondCounter();
            }

            if (slideUp && pitchRatio > pitchTarget) pitchRatio = pitchTarget;
            else if (!slideUp && pitchRatio < pitchTarget) pitchRatio = pitchTarget;
            /* JUCE's linear interpolation. May add back in and make toggleable */

            /*auto alpha = (float) (sourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                                       : l;*/

            const int snh = audioProcessor.getSnH(currentSample);
            float l = pos % snh != 0 ? -getAmi8Bit(inL[pos - (pos % snh)]) : -getAmi8Bit(inL[pos]);
            float r = (inR != nullptr) ? pos % snh != 0 ? -getAmi8Bit(inL[pos - (pos % snh)]) : -getAmi8Bit(inR[pos]) : l;

            auto envelopeValue = adsr.getNextSample();

            l *= isSilenced ? 0 : lgain * envelopeValue * vol;
            r *= isSilenced ? 0 : rgain * envelopeValue * vol;

            handleChannelPanning(currentSample, &l, &r);

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio * audioProcessor.getVibrato() * fineTune * bendRatio;

            if (loopEnable)
                if (sourceSamplePosition >= loopEnd)
                    sourceSamplePosition = (double)loopStart;

            if (!adsr.isActive() || sourceSamplePosition > playingSound->length)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}

float NewSamplerVoice::getAmi8Bit(const float samp)
{
    const int_fast8_t ami8bit = samp < 0 ? (int_fast8_t)ceil (samp * 128.f) : 
                                           (int_fast8_t)floor(samp * 127.f);

    const float amiSamp = samp < 0 ? (float)ami8bit / 128.f : (float)ami8bit / 127.f;

    return abs(amiSamp) >= 1 ? amiSamp > 0 ? 1.f : -1.f : amiSamp;
}

void NewSamplerVoice::handleChannelPanning(const int i, float* l, float* r)
{
    const bool stereoOn = audioProcessor.paulaStereoOn(i) && audioProcessor.getSampler(i).getNumVoices() > 1;
    const float pan = audioProcessor.getChanPan(i);

    if (stereoOn)
    {
        const float width = pan / 255;

        const float widthL = (*l * width) + (*r * std::fabs(1.f - width));
        const float widthR = (*r * width) + (*l * std::fabs(1.f - width));

        *l = widthL;
        *r = widthR;
    }
    else
    {
        const float panL = pan <= 128 ? 1.f : std::fabs(pan - 255) / 127;
        const float panR = pan >= 128 ? 1.f : pan / 127;
        
        *l *= panL;
        *r *= panR;
    }
}
