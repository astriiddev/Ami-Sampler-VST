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

#pragma once

//==============================================================================
/**
    Reads and Writes IFF 8SVX format 8-bits audio files.

    @see AudioFormat

    @tags{Audio}
*/
class IffAudioFormat : public juce::AudioFormat
{
    
public:
    //==============================================================================
    /** Creates a format object. */
    IffAudioFormat();
    /** Destructor. */
    ~IffAudioFormat() override;

    //==============================================================================
    juce::Array<int> getPossibleSampleRates() override;
    juce::Array<int> getPossibleBitDepths() override;
    bool canDoStereo() override;
    bool canDoMono() override;
    bool isChannelLayoutSupported (const juce::AudioChannelSet& channelSet) override;

    //==============================================================================
    juce::AudioFormatReader* createReaderFor (juce::InputStream* sourceStream,
                                              bool deleteStreamIfOpeningFails) override;

    juce::AudioFormatWriter* createWriterFor (juce::OutputStream* streamToWriteTo);

    juce::AudioFormatWriter* createWriterFor (juce::OutputStream* streamToWriteTo,
                                              double sampleRateToUse,
                                              unsigned int numberOfChannels,
                                              int bitsPerSample,
                                              const juce::StringPairArray& metadataValues,
                                              int qualityOptionIndex) override;

    juce::AudioFormatWriter* createWriterFor (juce::OutputStream* streamToWriteTo, double sampleRateToUse,
                                              const juce::AudioChannelSet& channelLayout,
                                              int bitsPerSample,
                                              const juce::StringPairArray& metadataValues,
                                              int qualityOptionIndex) override;
    using juce::AudioFormat::createWriterFor;

    //==============================================================================
    /** Utility function to replace the metadata in a iff file with a new set of values.

        If possible, this cheats by overwriting just the metadata region of the file, rather
        than by copying the whole file again.
    */
    bool replaceMetadataInFile (const juce::File& iffFile, const juce::StringPairArray& newMetadata);


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IffAudioFormat)
};
