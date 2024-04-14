/*
  ==============================================================================

    MuLawFormat.h
    Created: 11 Feb 2023 10:44:13am
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

    //==============================================================================
/*
    Reads and Writes mu-Law format audio files.

    @see AudioFormat

    @tags{Audio}
*/
class MuLawFormat : public juce::AudioFormat
{

public:
    MuLawFormat();
    ~MuLawFormat() override;

    //==============================================================================
    juce::Array<int> getPossibleSampleRates() override;
    juce::Array<int> getPossibleBitDepths() override;
    bool canDoStereo() override;
    bool canDoMono() override;
    bool isChannelLayoutSupported(const juce::AudioChannelSet& channelSet) override;
    //==============================================================================
    juce::AudioFormatReader* createReaderFor(juce::InputStream* sourceStream,
                                             bool deleteStreamIfOpeningFails) override;

    juce::AudioFormatWriter* createWriterFor(juce::OutputStream* streamToWriteTo);
    juce::AudioFormatWriter* createWriterFor(juce::OutputStream* streamToWriteTo,
                                             double sampleRateToUse,
                                             unsigned int numberOfChannels,
                                             int bitsPerSample,
                                             const juce::StringPairArray& metadataValues,
                                             int qualityOptionIndex) override;

    using AudioFormat::createWriterFor;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuLawFormat)
};
