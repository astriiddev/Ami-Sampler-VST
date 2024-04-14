/*
  ==============================================================================

    MuLawFormat.h
    Created: 11 Feb 2023 10:44:13am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    Reads and Writes mu-Law format audio files.

    @see AudioFormat

    @tags{Audio}
*/
class BrrAudioFormat : public juce::AudioFormat
{

public:
    BrrAudioFormat();
    ~BrrAudioFormat() override;

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
    
    //==============================================================================
    /** Utility function to replace the metadata in a iff file with a new set of values.

        If possible, this cheats by overwriting just the metadata region of the file, rather
        than by copying the whole file again.
    */
    bool replaceMetadataInFile (const juce::File& iffFile, const juce::StringPairArray& newMetadata);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrrAudioFormat)
};
