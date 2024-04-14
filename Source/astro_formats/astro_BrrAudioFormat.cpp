/*
  ==============================================================================

    BrrAudioFormat.cpp
    Created: 11 Feb 2023 10:44:13am
    Author:  _astriid_

  ==============================================================================
*/

#include "astro_BrrAudioFormat.h"

using StringMap = std::unordered_map<juce::String, juce::String>;

static auto getValueWithDefault(const StringMap& m, const juce::String& key, const juce::String& fallback = {})
{
    const auto iter = m.find(key);
    return iter != m.cend() ? iter->second : fallback;
}

static const char* const brrFormatName = "BRR file";

namespace BrrFileHelpers
{
        
    typedef struct BrrFilter
    {
        juce::int16 a, b, tmp[2];
    } brrfilter_t;

    constexpr inline size_t roundUpSize(size_t sz) noexcept { return (sz + 3) & ~3u; }

    /* S-DSP chip uses 16.16 fixed math for its filter coefficients during decoding */
    inline static juce::int16 fixedMath16_16(const juce::int32 in, const juce::int64 numerator, const juce::int32 denominator)
    {
        /* equivalent to (int16) round(numerator / denominator *  in) */
        return (juce::int16) ((in * (numerator << 16) / denominator) >> 16);
    }

    /* converts number of bytes to number of samples as per BRR's 16 sample to 9 byte ratio*/
    inline static juce::int32 bytePos2sampPos(const juce::int64 in)
    {
        /* equivalent to (int32) round(16.0 / 9.0 * (double) in) */
        return (juce::int32) ((in * ((16 << 16) / 9) + 0x8000) >> 16);
    }

    static void clearBrrFilter(brrfilter_t* f)
    {
        f->a = f->b = f->tmp[0] = f->tmp[1] = 0;
    }

    static void setBrrCoeff(brrfilter_t* f, char type)
    {
        switch (type)
        {
            case 1:
                f->a = fixedMath16_16(f->tmp[0], 15, 16);
                f->b = 0;
                break; 
                             
            case 2:      
                f->a = fixedMath16_16(f->tmp[0], 61, 32);
                f->b = fixedMath16_16(f->tmp[1], 15, 16);
                break;
                          
            case 3:
                f->a = fixedMath16_16(f->tmp[0], 115, 64);
                f->b = fixedMath16_16(f->tmp[1],  13, 16);
                break;

            default:
                f->a = f->b = 0;
                break;
        }
    }

    static void filterBrr(brrfilter_t* f, const juce::int16 shifted_samp, juce::int16 *sample)
    {
        const juce::int16 out = shifted_samp + f->a - f->b;

        f->tmp[1] = f->tmp[0];
        f->tmp[0] = out;
        *sample = out;
    }

    struct SampleLoopHeader
    {
        juce::int32 loopStart;
        juce::int32 loopEnd;

        template <typename NameType>
        static void setValue(StringMap& values, NameType name, juce::int32 val)
        {
            values[name] = juce::String(juce::ByteOrder::swapIfBigEndian(val));
        }

        static void setValue(StringMap& values, int prefix, const char* name, juce::int32 val)
        {
            setValue(values, "Loop" + juce::String(prefix) + name, val);
        }

        void copyTo(StringMap& values, const int /* totalSize */) const
        {
            setValue(values, 0, "Start",  loopStart);
            setValue(values, 0, "End", loopEnd - 1);
        }

        template <typename NameType>
        static juce::int32 getValue(const StringMap& values, NameType name, const char* def)
        {
            return (juce::int32) getValueWithDefault(values, name, def).getIntValue();
        }

        static juce::int32 getValue(const StringMap& values, int prefix, const char* name, const char* def)
        {
            return getValue(values, "Loop" + juce::String(prefix) + name, def);
        }

        static juce::MemoryBlock createFrom(const StringMap& values)
        {
            juce::MemoryBlock data;
            data.setSize(roundUpSize(sizeof(SampleLoopHeader)), true);

            auto s = static_cast<SampleLoopHeader*> (data.getData());

            s->loopStart = getValue(values, 0, "Start", "0");
            s->loopEnd = getValue(values, 0, "End", "0");

            return data;
        }
    } JUCE_PACKED;
}

class BrrAudioFormatReader : public juce::AudioFormatReader
{
public:
    BrrAudioFormatReader(juce::InputStream* in) : juce::AudioFormatReader(in, brrFormatName)
    {
        fileLength = in->getTotalLength();
        numChannels = 1;
        sampleRate = 16744;
        bitsPerSample = 16;
        bytesPerFrame = 2;
        dataLength = getBrrLength(fileLength, in);

        lengthInSamples = (bytesPerFrame > 0) ? (dataLength * 2 / bytesPerFrame) : 0;

        usesFloatingPointData = false;
    }

    juce::int64 getBrrLength(const juce::int64 length, juce::InputStream* in)
    {
        using namespace BrrFileHelpers;

        if (length % 9 == 0)
        {
            dataStart = 0;

            hasLoop = false;
            loopStart = bytePos2sampPos(length);
            loopEnd = loopStart;

            return (juce::int64) loopStart;
        }
        else if ((length - 2) % 9 == 0)
        {
            in->setPosition(0);

            dataStart = 2;
            hasLoop = true;
            loopStart = bytePos2sampPos(input->readShort());

            return (juce::int64) bytePos2sampPos(length - 2);
        }
        else
        {
            loopStart = 0;
            loopEnd   = 0;
            dataStart = 0;

            return 0;
        }
    }

    void decodeBrr(juce::int16* destBuffer, const char* sourceBuffer, int* start, int bytesToDecode)
    {
        using namespace BrrFileHelpers;

        bool loopFlag   = false, 
             endFlag    = false;

        int endFlagPos = 0, i = *start;

        juce::int8  filterFlag = 0,
                    shifter    = 0;

        juce::int16 temp_samp  = 0;

        while (--bytesToDecode >= 0)
        {
            juce::uint8 inSample = (juce::uint8) *sourceBuffer++;

            if ((i - dataStart) % 9 == 0)
            {
                shifter = (inSample & ~0x0F) >> 4;
                if (shifter > 12) shifter = 12;

                filterFlag = (inSample & ~0xF3) >> 2;
                if (filterFlag > 3) filterFlag = 3;

                loopFlag = (inSample & 0x02) ? true : false;

                if (inSample & 0x01)
                {
                    endFlag = true;
                    endFlagPos = i;

                    if (loopFlag && endFlag)
                    {
                        hasLoop = true;
                        loopEnd = bytePos2sampPos(i + 7);
                    }
                    else
                    {
                        hasLoop = false;
                        loopStart = 16;
                        loopEnd = (juce::int32) dataLength;
                    }
                }
                else
                {
                    endFlag = false;
                    endFlagPos = (int) fileLength;
                }
            }

            else if ((i - dataStart) % 9 != 0 && shifter <= 12)
            {
                if ((i - dataStart) > 0)
                {
                    char nibble = 0;

                    nibble = (char) ((inSample & (juce::int8) ~0x0F) >> 4);
                    if (nibble > 7) nibble ^= (juce::uint8) 0xF0;

                    setBrrCoeff(&brrfilter, filterFlag);
                    filterBrr(&brrfilter, nibble << shifter, &temp_samp);

                    *destBuffer++ = i <= endFlagPos + 8 ? temp_samp : 0;

                    nibble = (char) (inSample & (juce::uint8) ~0xF0);
                    if (nibble > 7) nibble ^= (juce::uint8) 0xF0;

                    setBrrCoeff(&brrfilter, filterFlag);
                    filterBrr(&brrfilter, nibble << shifter, &temp_samp);

                    *destBuffer++ = i <= endFlagPos + 8 ? temp_samp : 0;
                }
            }

            i++;
        }

        *start = i;
    }

    bool readSamples(int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
         juce::int64 startSampleInFile, int numSamples) override
    {
        int blockPos = (int) dataStart;
        using namespace BrrFileHelpers;

        StringMap dict;

        juce::HeapBlock<SampleLoopHeader> loop;
        loop.calloc(juce::jmax((size_t)8, sizeof(SampleLoopHeader)), 1);

        clearSamplesBeyondAvailableLength(destSamples, numDestChannels, startOffsetInDestBuffer,
            startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        input->setPosition(blockPos);
        clearBrrFilter(&brrfilter);

        while (numSamples > 0)
        {
            const int tempBufSize = 480 * 3 * 4;
            const int tempBrrBuffSize = bytePos2sampPos(tempBufSize);

            char tempBuffer[tempBufSize];
            juce::int16 *tempBrrBuffer = new juce::int16[tempBrrBuffSize];

            auto numBlockThisTime = juce::jmin(tempBufSize, (int) fileLength);
            auto bytesRead = input->read(tempBuffer, numBlockThisTime);

            auto numSampThisTime  = juce::jmin(tempBrrBuffSize, numSamples);

            juce::zeromem(tempBrrBuffer, (size_t) tempBrrBuffSize);

            if (bytesRead < numBlockThisTime)
            {
                jassert(bytesRead >= 0);
                juce::zeromem(tempBuffer + bytesRead, (size_t) (tempBufSize - bytesRead));
            }

            decodeBrr(tempBrrBuffer, tempBuffer, &blockPos, numBlockThisTime);

            copySampleData(bitsPerSample, usesFloatingPointData,
                destSamples, startOffsetInDestBuffer, numDestChannels,
                tempBrrBuffer, (int)numChannels, numSampThisTime);

            startOffsetInDestBuffer += numSampThisTime;
            numSamples -= numSampThisTime;

            delete[] tempBrrBuffer;
            tempBrrBuffer = nullptr;
        }

        loop.get()->loopStart = loopStart;
        loop.get()->loopEnd = loopEnd;

        loop->copyTo(dict, 8);

        if (dict.size() > 0)            dict["MetaDataSource"] = "BRR";

        metadataValues.addUnorderedMap(dict);

        return true;
    }

    static void copySampleData(unsigned int /*numBitsPerSample*/, const bool /*floatingPointData*/,
        int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
        const void* sourceData, int numberOfChannels, int numSamples) noexcept
    {
        ReadHelper<juce::AudioData::Int32, juce::AudioData::Int16, juce::AudioData::LittleEndian>::read(destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples);
    }

    juce::int64 dataStart = 0, dataLength = 0;
    int bytesPerFrame = 0;

private:
        
    BrrFileHelpers::brrfilter_t brrfilter;

    juce::int64  fileLength = 0;
    juce::int32 loopStart = 0, loopEnd = 0;
    bool hasLoop = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrrAudioFormatReader)
};

//==============================================================================

/*
*
* TODO!!!! Port BRR encoder into JUCE format writer!
*
*/


class BrrAudioFormatWriter : public juce::AudioFormatWriter
{
public:
    BrrAudioFormatWriter(juce::OutputStream* const out, const double rate,
                        const juce::AudioChannelSet& channelLayoutToUse, const unsigned int bits)
        : AudioFormatWriter(out, brrFormatName, rate, channelLayoutToUse, bits)
    {
        usesFloatingPointData = false;
    }

    ~BrrAudioFormatWriter() override
    {
    }


    bool write(const int** /* data */, const int /* numSamples */) override
    {
            
        /*
        *
        * TODO!!!! Port BRR encoder into JUCE format writer!
        *
        */

        return false;
        /*int* decodedBuffer = new int[numSamples];
        //int decodedBuffer[numSamples];

        jassert(numSamples >= 0);
        jassert(data != nullptr && *data != nullptr); // the input must contain at least one channel!
            
        if (writeFailed)
            return false;

        auto bytes = numChannels * (size_t)numSamples * bitsPerSample / 8;
        tempBlock.ensureSize(bytes, false);

        WriteHelper<AudioData::UInt8, AudioData::Int32, AudioData::LittleEndian>::write(tempBlock.getData(), (int)numChannels, &decodedBuffer, numSamples);

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
        lengthInSamples += (uint64)numSamples;
        return true;*/
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BrrAudioFormatWriter)
};

//==============================================================================
    
BrrAudioFormat::BrrAudioFormat() : AudioFormat(brrFormatName, ".brr") {}
BrrAudioFormat::~BrrAudioFormat() {}

juce::Array<int> BrrAudioFormat::getPossibleSampleRates() { return 8363; }
juce::Array<int> BrrAudioFormat::getPossibleBitDepths() { return 8; }

bool BrrAudioFormat::canDoStereo() { return false; }
bool BrrAudioFormat::canDoMono() { return true; }

bool BrrAudioFormat::isChannelLayoutSupported(const juce::AudioChannelSet& channelSet)
{ 
    return channelSet == juce::AudioChannelSet::mono() ? true : false;
}

juce::AudioFormatReader* BrrAudioFormat::createReaderFor(juce::InputStream* sourceStream, bool deleteStreamIfOpeningFails)
{
    std::unique_ptr<BrrAudioFormatReader> r(new BrrAudioFormatReader(sourceStream));

    if (r->lengthInSamples > 0)
        return r.release();

    if (!deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

//==============================================================================

juce::AudioFormatWriter* BrrAudioFormat::createWriterFor(juce::OutputStream* /* out */)
{
    return nullptr;
}

juce::AudioFormatWriter* BrrAudioFormat::createWriterFor(juce::OutputStream* /* out */, double /* sampleRateToUse */,
                                                unsigned int /* numberOfChannels */, int /* bitsPerSample */,
                                                const juce::StringPairArray& /* metadataValues */, int /* qualityOptionIndex */)
{
    return nullptr;
}
