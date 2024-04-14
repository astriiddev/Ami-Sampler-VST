/*
  ==============================================================================

    MuLawFormat.cpp
    Created: 11 Feb 2023 10:44:13am
    Author:  _astriid_

  ==============================================================================
*/

#include "astro_MuLawFormat.h"

static const char* const muFormatName = "BIN file";

class MuLawFormatReader : public juce::AudioFormatReader
{
public:
    MuLawFormatReader(juce::InputStream* in) : AudioFormatReader(in, muFormatName)
    {
        auto length = in->getTotalLength();
        numChannels = 1;
        sampleRate = 22050;
        bitsPerSample = 16;
        bytesPerFrame = 2;
        dataLength = length * 2;
        lengthInSamples = (bytesPerFrame > 0) ? (dataLength / bytesPerFrame) : 0;

        usesFloatingPointData = false;
    }

    void decodeMuLaw(juce::int16* destBuffer, const char* sourceBuffer, int samplesToDecode)
    {
        while(--samplesToDecode >= 0)
        {
            juce::uint8 inSample = (juce::uint8) *sourceBuffer++;

            double  temp_decode = (inSample & 0x80) ? 1.0 : -1.0;
            temp_decode *= (pow(256, ((double)(inSample & INT8_MAX) / INT8_MAX)) - 1);

            temp_decode /= 255;
            *destBuffer++ = (juce::int16)round(temp_decode * INT16_MAX);
        }
    }

    bool readSamples(int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
         juce::int64 startSampleInFile, int numSamples) override
    {
        clearSamplesBeyondAvailableLength(destSamples, numDestChannels, startOffsetInDestBuffer,
            startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        input->setPosition(0);

        while (numSamples > 0)
        {
            const int tempBufSize = 480 * 3 * 4; // (keep this a multiple of 3)
            char tempBuffer[tempBufSize];
            juce::int16 *tempMuBuffer = new juce::int16[tempBufSize];
                
            auto numThisTime = juce::jmin(tempBufSize, numSamples);
            auto bytesRead = input->read(tempBuffer, numThisTime);

            juce::zeromem(tempMuBuffer, tempBufSize * 2);

            if (bytesRead < numThisTime / bytesPerFrame)
            {
                jassert(bytesRead >= 0);
                juce::zeromem(tempBuffer + bytesRead, (size_t)(numThisTime / bytesPerFrame - bytesRead));
            }

            decodeMuLaw(tempMuBuffer, tempBuffer, numThisTime);

            copySampleData(bitsPerSample, usesFloatingPointData,
                destSamples, startOffsetInDestBuffer, numDestChannels,
                tempMuBuffer, (int)numChannels, numThisTime);

            startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;

            delete[] tempMuBuffer;
            tempMuBuffer = nullptr;
        }

        return true;
    }

    static void copySampleData(unsigned int /*numBitsPerSample*/, const bool /*floatingPointData*/,
        int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
        const void* sourceData, int numberOfChannels, int numSamples) noexcept
    {
        ReadHelper<juce::AudioData::Int32, juce::AudioData::Int16, juce::AudioData::LittleEndian>::read(destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples);
    }

    juce::int64 dataLength = 0;
    int bytesPerFrame = 0;


private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuLawFormatReader)
};

//==============================================================================

class MuLawFormatWriter : public juce::AudioFormatWriter
{
public:
    MuLawFormatWriter(juce::OutputStream* const out, const double rate,
                        const juce::AudioChannelSet& channelLayoutToUse, const unsigned int bits)
        : AudioFormatWriter(out, muFormatName, rate, channelLayoutToUse, bits)
    {
        usesFloatingPointData = false;
    }

    ~MuLawFormatWriter() override
    {
    }

    void encodeMuLaw(int* destBuffer, const int* sourceBuffer, int numSamples)
    {
        const double ratio1 = (double)UINT8_MAX / INT32_MAX;
        const double ratio2 = (double)INT32_MAX / log(1 + UINT8_MAX); 

        while(--numSamples >= 0)
        {
            int temp_samp = *sourceBuffer++;

            const double encoding = log(1.0 + ratio1 * fabs(temp_samp)) * ratio2;

            uint32_t encodedSample = (uint32_t)round(encoding);
            if(temp_samp < 0) encodedSample ^= (uint32_t) ~INT32_MAX;

            *destBuffer++ = (int)encodedSample;
        }
    }

    bool write(const int** data, const int numSamples) override
    {
        int* decodedBuffer = new int[numSamples];

        jassert(numSamples >= 0);
        jassert(data != nullptr && *data != nullptr); // the input must contain at least one channel!
            
        if (writeFailed)
            return false;

        auto bytes = numChannels * (size_t)numSamples * bitsPerSample / 8;
        tempBlock.ensureSize(bytes, false);

        encodeMuLaw(decodedBuffer, *data, numSamples);

        WriteHelper<juce::AudioData::UInt8, juce::AudioData::Int32, juce::AudioData::LittleEndian>::write(tempBlock.getData(), (int)numChannels, &decodedBuffer, numSamples);

        if (!output->write(tempBlock.getData(), bytes))
        {
            // failed to write to disk, so let's try writing the header.
            // If it's just run out of disk space, then if it does manage
            // to write the header, we'll still have a usable file..
            delete[] decodedBuffer;
            decodedBuffer = nullptr; 
            writeFailed = true;
            return false;
        }

        delete[] decodedBuffer;
        decodedBuffer = nullptr; 
        bytesWritten += bytes;
        lengthInSamples += (juce::uint64)numSamples;
        return true;
    }

    bool flush() override
    {
        auto lastWritePos = output->getPosition();

        if (output->setPosition(lastWritePos))
            return true;

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassertfalse;
        return false;
    }

private:
    juce::MemoryBlock tempBlock;
    juce::uint64 lengthInSamples = 0, bytesWritten = 0;
    bool writeFailed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuLawFormatWriter)
};

//==============================================================================
    
MuLawFormat::MuLawFormat() : juce::AudioFormat(muFormatName, ".bin") {}
MuLawFormat::~MuLawFormat() {}

juce::Array<int> MuLawFormat::getPossibleSampleRates() { return 8363; }
juce::Array<int> MuLawFormat::getPossibleBitDepths() { return 8; }

bool MuLawFormat::canDoStereo() { return false; }
bool MuLawFormat::canDoMono() { return true; }

bool MuLawFormat::isChannelLayoutSupported(const juce::AudioChannelSet& channelSet)
{ 
    return channelSet == juce::AudioChannelSet::mono() ? true : false;
}

juce::AudioFormatReader* MuLawFormat::createReaderFor(juce::InputStream* sourceStream, bool deleteStreamIfOpeningFails)
{
    std::unique_ptr<MuLawFormatReader> r(new MuLawFormatReader(sourceStream));

    if (r->lengthInSamples > 0)
        return r.release();

    if (!deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

//==============================================================================

juce::AudioFormatWriter* MuLawFormat::createWriterFor(juce::OutputStream* out)
{
    if (out != nullptr)
        return new MuLawFormatWriter(out, 22050.0, juce::AudioChannelSet::mono(), 8);

    return nullptr;
}

juce::AudioFormatWriter* MuLawFormat::createWriterFor(juce::OutputStream* out, double /* sampleRateToUse */,
                                                      unsigned int /* numberOfChannels */, int /* bitsPerSample */,
                                                      const juce::StringPairArray& /* metadataValues */, int /* qualityOptionIndex */)
{
    if (out != nullptr)
        return new MuLawFormatWriter(out, 22050.0, juce::AudioChannelSet::mono(), 8);

    return nullptr;
}
