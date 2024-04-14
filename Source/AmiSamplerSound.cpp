/*
  ==============================================================================

    AmiSamplerSound.cpp
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
#include "AmiSamplerSound.h"

/*
  ==============================================================================


  //// Reimplementation of JUCE's Sampler sound and sampler voice to include looping and nearest-neighbor interpolation ////


  ==============================================================================
*/

AmiSamplerSound::AmiSamplerSound (const juce::String& soundName, int sampleNumber,
                        juce::AudioSampleBuffer& source, const double& sampleRate,
                        const juce::BigInteger& notes,
                        int midiNoteForNormalPitch,
                        double attackTimeSecs,
                        double releaseTimeSecs,
                        AmiAudioProcessor& p)
    : name (soundName),
      sourceSampleRate (sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch), audioProcessor(p)
{
    if (sourceSampleRate > 0 && source.getNumSamples() > 0)
    {
        data.reset();
        data = std::make_unique<juce::AudioSampleBuffer>(source);
        length = source.getNumSamples();

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);

        currentSample = sampleNumber;
    }
}

AmiSamplerSound::~AmiSamplerSound()
{
}

bool AmiSamplerSound::appliesToNote (int midiNoteNumber)
{
    if (midiNoteNumber < audioProcessor.getLowNote(currentSample)) return false;
    if (midiNoteNumber > audioProcessor.getHighNote(currentSample)) return false;

    return true;
}

bool AmiSamplerSound::appliesToChannel (int midiChannel)
{
    const int channel2Play = audioProcessor.getMidiChannel(currentSample);

    if (channel2Play <= 0) return true;
    if (channel2Play != midiChannel) return false;

    return true;
}

//==============================================================================
AmiSamplerVoice::AmiSamplerVoice(AmiAudioProcessor& p) : audioProcessor(p)
{
}

AmiSamplerVoice::~AmiSamplerVoice() {}

bool AmiSamplerVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<const AmiSamplerSound*> (sound) != nullptr;
}

void AmiSamplerVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int pitchwheel)
{
    if (auto* sound = dynamic_cast<AmiSamplerSound*> (s))
    {
        const double playbackSampleRate = audioProcessor.getSourceSampleRate((currentSample = sound->currentSample));
        const double devSampleRate = audioProcessor.getDevSampleRate();

        sound->midiRootNote = 120 - audioProcessor.getRootNote(currentSample);

        if (sourceSamplePosition >= sound->length) releasedNote = true;

        numVoices = audioProcessor.getSampler(currentSample).getNumVoices();

        bendRatio = std::pow(2., ((double) pitchwheel - 8192.) / 49152.);
        fineTune  = 1 + audioProcessor.getFineTune(currentSample) / 1200.;

        pitchTarget = std::pow(2., (double) (midiNoteNumber - sound->midiRootNote) / 12.) * playbackSampleRate / devSampleRate;

        slideUp = (pitchTarget > pitchRatio);

        lgain = audioProcessor.shouldPan(currentSample, 0) ? 0 : velocity;
        rgain = audioProcessor.shouldPan(currentSample, 1) ? 0 : velocity;

        audioProcessor.incPanCount(currentSample);

        adsr.setSampleRate(devSampleRate);
        adsr.setParameters(sound->params);
        
        glissRatio = (pitchTarget - pitchRatio) / (audioProcessor.getGlissando(currentSample) * devSampleRate * 0.01);

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

void AmiSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
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

void AmiSamplerVoice::pitchWheelMoved(int newValue)
{
    bendRatio = std::pow(2., ((double) newValue - 8192.0) / 49152.0);
}

void AmiSamplerVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void AmiSamplerVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if(audioProcessor.isMuted(currentSample)) return;

    if (AmiSamplerSound* playingSound = static_cast<AmiSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;

        const float* const inL = data.getReadPointer(0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer(1) : nullptr;

        float* outL = outputBuffer.getWritePointer(0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer(1, startSample) : nullptr;

        const int  loopStart = audioProcessor.getLoopStart(currentSample), 
                   loopEnd    = audioProcessor.getLoopEnd(currentSample),
                   snh = audioProcessor.getSnH(currentSample);

        const bool loopEnable = audioProcessor.getLoopEnable(currentSample),
                   pingPongLoop = audioProcessor.getPingPongLoop(currentSample) && loopEnable,
                   stereoOn = audioProcessor.paulaStereoOn(currentSample) && numVoices > 1;

        const float vol = audioProcessor.getChanVol(currentSample),
                    vibrato = audioProcessor.getVibrato(),
                    pan = audioProcessor.getChanPan(currentSample);

        fineTune = 1. + audioProcessor.getFineTune(currentSample) / 1200.;

        if (sourceSamplePosition <= 0) sourceSamplePosition = 0.0f;

        if(currentSample == audioProcessor.getCurrentSample())
            audioProcessor.setSamplePos(lgain <= 0 && rgain <= 0 ? 0 : (int) sourceSamplePosition);

        if (audioProcessor.getGlissando(currentSample) <= 1.f) pitchRatio = pitchTarget;
        
        while (--numSamples >= 0)
        {
            const double totalPitchRatio = (pitchRatio = gliss2pitch()) * vibrato * fineTune * bendRatio;
            const int pos = (int) std::floor(sourceSamplePosition);

            const float envelopeValue = adsr.getNextSample();

            float l = -getAmi8Bit(inL[pos - (pos % snh)]);
            float r = (inR != nullptr) ? -getAmi8Bit(inR[pos - (pos % snh)]) : l;

            l *= lgain * vol * envelopeValue;
            r *= rgain * vol * envelopeValue;

            handleChannelPanning(stereoOn, pan, &l, &r);

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition = handleLoop(loopEnable, pingPongLoop, loopStart, loopEnd, sourceSamplePosition, totalPitchRatio);
            
            if (!adsr.isActive() || sourceSamplePosition > playingSound->length)
            {
                stopNote(0.0f, false);
                break;
            }
        }
    }
}

float AmiSamplerVoice::getAmi8Bit(const float samp) const
{
    const float amiSamp = samp < 0 ? std::floor(samp * 128.f) / 128.f : std::floor(samp * 127.f) / 127.f;

    return amiSamp >= 1 ? 1.f : amiSamp <= -1 ? -1.f : amiSamp;
}

double AmiSamplerVoice::gliss2pitch() const
{
    const double nextPitch = pitchRatio + glissRatio;

    if (numVoices > 1) return pitchTarget;
    if (pitchRatio <= 0) return pitchTarget;

    if (slideUp && nextPitch > pitchTarget) return pitchTarget;
    if (!slideUp && nextPitch < pitchTarget) return pitchTarget;

    return nextPitch;
}

void AmiSamplerVoice::handleChannelPanning(const bool& stereoOn, const float& pan, float* l, float* r) const
{
    if (stereoOn)
    {
        const float width = pan / 255;

        const float widthL = (*l * width) + (*r * std::abs(1.f - width));
        const float widthR = (*r * width) + (*l * std::abs(1.f - width));

        *l = widthL;
        *r = widthR;
    }
    else
    {
        const float panL = pan <= 128 ? 1.f : std::abs(pan - 255.f) / 127.f;
        const float panR = pan >= 128 ? 1.f : pan / 127.f;
        
        *l *= panL;
        *r *= panR;
    }
}

double AmiSamplerVoice::handleLoop(const bool enable, const bool pingpong, const int start, const int end, const double pos, const double pitch)
{
    const double nextPos = pos + pitch;
    if(!enable) return nextPos;
    if(!pingpong) return nextPos < end ? nextPos : (float) start;
    return (playForward = playForward ? (nextPos < end) : ((pos - pitch) < start)) ? nextPos : pos - pitch;
}
