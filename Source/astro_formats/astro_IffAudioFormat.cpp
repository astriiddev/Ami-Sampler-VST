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

#include "astro_IffAudioFormat.h"

using StringMap = std::unordered_map<juce::String, juce::String>;

static auto toMap(const juce::StringPairArray& array)
{
    StringMap result;

    for (auto i = 0; i < array.size(); ++i)
        result[array.getAllKeys()[i]] = array.getAllValues()[i];

    return result;
}

static auto getValueWithDefault(const StringMap& m, const juce::String& key, const juce::String& fallback = {})
{
    const auto iter = m.find(key);
    return iter != m.cend() ? iter->second : fallback;
}

static const char* const iffFormatName = "IFF file";
    
namespace IffFileHelpers
{
using Fixed = juce::int32;                //68000 processor 16.16 fixed-point format

using UBYTE = juce::uint8;                //IFF/AmigaOS typedef for unsigned 8bit integer
using UWORD = juce::uint16;               //IFF/AmigaOS typedef for unsigned 16bit integer
using ULONG = juce::uint32;               //IFF/AmigaOS typedef for unsigned 32bit integer, !!! NOT !!! 64bit integer

constexpr Fixed Unity = 0x10000;    //Unity volume, or 1.0f in floating point format

constexpr inline int chunkName (const char* name) noexcept         { return (int) juce::ByteOrder::littleEndianInt (name); }
constexpr inline size_t roundUpSize (size_t sz) noexcept           { return (sz + 3) & ~3u; }

//==============================================================================
    
inline juce::AudioChannelSet canonicalWavChannelSet (int numChannels)
{
    if (numChannels == 1)  return juce::AudioChannelSet::mono();
    if (numChannels == 2)  return juce::AudioChannelSet::stereo();
    if (numChannels == 3)  return juce::AudioChannelSet::createLCR();
    if (numChannels == 4)  return juce::AudioChannelSet::quadraphonic();
    if (numChannels == 5)  return juce::AudioChannelSet::create5point0();
    if (numChannels == 6)  return juce::AudioChannelSet::create5point1();
    if (numChannels == 7)  return juce::AudioChannelSet::create7point0SDDS();
    if (numChannels == 8)  return juce::AudioChannelSet::create7point1SDDS();

    return juce::AudioChannelSet::discreteChannels (numChannels);
}
//==============================================================================

struct Voice8Header
{
    struct SampleLoop
    {
        ULONG oneShotHiSamples;     //loop start
        ULONG repeatHiSamples;      //length of loop
        ULONG samplesPerHiCycle;    //number of samples in cycle
    } JUCE_PACKED;

    SampleLoop loop;
    UWORD samplesPerSec;            //sample rate, typically 16726 or 8363
    UBYTE ctOctave;                 //number of octaves, typically 1
    UBYTE sCompression;             //compression type, typically 0
    Fixed volume;                   //16.16 fixed-point, typically unity volume: 0x10000 (1.00f in floating-point)

    template <typename NameType>
    static void setValue32 (StringMap& values, NameType name, ULONG val)
    {
        values[name] = juce::String (juce::ByteOrder::swapIfLittleEndian (val));
    }

    template <typename NameType>
    static void setValue16(StringMap& values, NameType name, UWORD val)
    {
        values[name] = juce::String(juce::ByteOrder::swapIfLittleEndian(val));
    }

    template <typename NameType>
    static void setValue8(StringMap& values, NameType name, UBYTE val)
    {
        values[name] = juce::String(val);
    }

    static void setValue32 (StringMap& values, int prefix, const char* name, ULONG val)
    {
        setValue32 (values, "Loop" + juce::String (prefix) + name, val);
    }

    static void setValue16(StringMap& values, int prefix, const char* name, UWORD val)
    {
        setValue16(values, "Loop" + juce::String(prefix) + name, val);
    }

    static void setValue8(StringMap& values, int prefix, const char* name, UBYTE val)
    {
        setValue8(values, "Loop" + juce::String(prefix) + name, val);
    }

    void copyTo (StringMap& values, const int /* totalSize */) const
    {
        ULONG loop_end = juce::ByteOrder::swapIfLittleEndian(loop.oneShotHiSamples) + 
                            juce::ByteOrder::swapIfLittleEndian(loop.repeatHiSamples) - 1;

        setValue32 (values, 0, "Start",             loop.oneShotHiSamples);
        setValue32 (values, 0, "Repeat",            loop.repeatHiSamples);
        setValue32 (values, 0, "End",               juce::ByteOrder::swapIfLittleEndian(loop_end));
        setValue32 (values, 0, "SampsPerCycle",     loop.samplesPerHiCycle);

        setValue16 (values, "SamplesPerSec",     samplesPerSec);
        setValue8  (values, "OctaveCount",       ctOctave);
        setValue8  (values, "SampCompress",      sCompression);
        setValue32 (values, "sampleVolume",      (ULONG)volume);
    }

    template <typename NameType>
    static ULONG getValue32(const StringMap& values, NameType name, const char* def)
    {
        return (ULONG)juce::ByteOrder::swapIfLittleEndian ((ULONG) getValueWithDefault (values, name, def).getIntValue());
    }

    template <typename NameType>
    static UWORD getValue16(const StringMap& values, NameType name, const char* def)
    {
        return (UWORD)juce::ByteOrder::swapIfLittleEndian ((UWORD) getValueWithDefault(values, name, def).getIntValue());
    }

    template <typename NameType>
    static UBYTE getValue8 (const StringMap& values, NameType name, const char* def)
    {
        return (UBYTE) getValueWithDefault(values, name, def).getIntValue();
    }

    static ULONG getValue32 (const StringMap& values, int prefix, const char* name, const char* def)
    {
        return getValue32 (values, "Loop" + juce::String (prefix) + name, def);
    }

    static UWORD getValue16 (const StringMap& values, int prefix, const char* name, const char* def)
    {
        return getValue16 (values, "Loop" + juce::String(prefix) + name, def);
    }

    static UBYTE getValue8 (const StringMap& values, int prefix, const char* name, const char* def)
    {
        return getValue8 (values, "Loop" + juce::String(prefix) + name, def);
    }

    static juce::MemoryBlock createFrom (const StringMap& values)
    {
        juce::MemoryBlock data;
        data.setSize (roundUpSize (sizeof (Voice8Header)), true);

        auto s = static_cast<Voice8Header*> (data.getData());
            
        auto& loop = s->loop;

        const ULONG loop_replen = juce::ByteOrder::swapIfLittleEndian(getValue32 (values, 0, "End", "0")) - 
                                    juce::ByteOrder::swapIfLittleEndian(getValue32 (values, 0, "Start", "0")) + 1;
            
        loop.oneShotHiSamples   = getValue32 (values, 0, "Start", "0");
            
        loop.repeatHiSamples    = values.find("Loop0Repeat") != values.end() ? getValue32(values, 0, "Repeat", "0") :
                                    juce::ByteOrder::swapIfLittleEndian(loop_replen);

        loop.samplesPerHiCycle  = getValue32 (values, 0, "SampsPerCycle", "32");

        s->samplesPerSec     = getValue16 (values, "SamplesPerSec", "16726");
        s->ctOctave          = getValue8  (values, "OctaveCount", "1");
        s->sCompression      = getValue8  (values, "SampCompress", "0");
        s->volume            = (Fixed) getValue32 (values, "sampleVolume", std::to_string(Unity).c_str());

        return data;
    }
} JUCE_PACKED;
}
    //==============================================================================

//==============================================================================
class IffAudioFormatReader  : public juce::AudioFormatReader
{
public:
    IffAudioFormatReader (juce::InputStream* in)  : AudioFormatReader (in, iffFormatName)
    {
        using namespace IffFileHelpers;
        juce::uint64 len = 0, end = 0;

        numChannels = 1;
        bitsPerSample = 8;
        bytesPerFrame = 1;
        usesFloatingPointData = false;
            
        StringMap dict;
        auto firstChunkType = input->readInt();

        if (firstChunkType == chunkName ("FORM"))
        {
            len = (juce::uint64) (juce::uint16) input->readIntBigEndian();
            end = len + (juce::uint64) input->getPosition();
        }
        else
        {
            if(firstChunkType == chunkName("RIFF")) return;

            dataLength = lengthInSamples = in->getTotalLength();

            bodyChunkStart = 0;
            sampleRate = 8363;
                
            juce::HeapBlock<Voice8Header> vhdr;
            vhdr.calloc (juce::jmax ((size_t)21, sizeof (Voice8Header)), 1);

            vhdr.get()->loop.oneShotHiSamples = (ULONG) dataLength;
            vhdr.get()->loop.repeatHiSamples = 0;
            vhdr.get()->loop.samplesPerHiCycle = 32;
                
            vhdr.get()->samplesPerSec = 8363;
            vhdr.get()->ctOctave = 1;
            vhdr.get()->sCompression = 0;
            vhdr.get()->volume = 0x10000;

            return;
        }

        if (input->readInt() == chunkName ("8SVX"))
        {
            while ((juce::uint64) input->getPosition() < end && ! input->isExhausted())
            {
                auto chunkType = input->readInt();
                auto length = (juce::uint32) input->readIntBigEndian();
                auto chunkEnd = input->getPosition() + length + (length & 1); 

                if (chunkType == chunkName ("VHDR"))
                {
                    // read the voice header chunk
                    juce::HeapBlock<Voice8Header> vhdr;
                    vhdr.calloc (juce::jmax ((size_t) length + 1, sizeof (Voice8Header)), 1);
                    input->read (vhdr, (int) length);
                    vhdr->copyTo (dict, (int) length);

                    sampleRate = juce::ByteOrder::swapIfLittleEndian(vhdr.get()->samplesPerSec);
                }
                else if (chunkType == chunkName ("BODY"))
                {
                    lengthInSamples = dataLength = length;
                    
                    bodyChunkStart = input->getPosition();
                }
                    else if (chunkEnd <= input->getPosition())
                {
                    break;
                } 

                input->setPosition (chunkEnd);
            }

            if (dict.size() > 0)            dict["MetaDataSource"] = "IFF";

            metadataValues.addUnorderedMap(dict);
        }
        
    }

    //==============================================================================
    bool readSamples (int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
                        juce::int64 startSampleInFile, int numSamples) override
    {
        clearSamplesBeyondAvailableLength (destSamples, numDestChannels, startOffsetInDestBuffer,
                                            startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        input->setPosition (bodyChunkStart + startSampleInFile * bytesPerFrame);

        while (numSamples > 0)
        {
            const int tempBufSize = 480 * 3 * 4; // (keep this a multiple of 3)
            char tempBuffer[tempBufSize];

            auto numThisTime = juce::jmin (tempBufSize / bytesPerFrame, numSamples);
            auto bytesRead = input->read (tempBuffer, numThisTime * bytesPerFrame);

            if (bytesRead < numThisTime * bytesPerFrame)
            {
                jassert (bytesRead >= 0);
                juce::zeromem (tempBuffer + bytesRead, (size_t) (numThisTime * bytesPerFrame - bytesRead));
            }

            copySampleData (bitsPerSample, usesFloatingPointData,
                            destSamples, startOffsetInDestBuffer, numDestChannels,
                            tempBuffer, (int) numChannels, numThisTime);

            startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;
        }

        return true;
    }

    static void copySampleData (unsigned int numBitsPerSample, const bool /* floatingPointData */,
                                int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numberOfChannels, int numSamples) noexcept
    {
        if(numBitsPerSample > 8 || numberOfChannels > 1 || numDestChannels > 1) return;

        ReadHelper<juce::AudioData::Int32, juce::AudioData::Int8, juce::AudioData::BigEndian>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples);
    }
        
    juce::int64 bodyChunkStart = 0, dataLength = 0;
    int bytesPerFrame = 0;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IffAudioFormatReader)
};

//==============================================================================
class IffAudioFormatWriter  : public juce::AudioFormatWriter
{
public:
    IffAudioFormatWriter (juce::OutputStream* const out, const bool isRawPCM, const double rate,
                            const juce::AudioChannelSet& channelLayoutToUse, const unsigned int bits,
                            const juce::StringPairArray& metadataValues)
        : AudioFormatWriter (out, iffFormatName, rate, channelLayoutToUse, bits)
    {
        using namespace IffFileHelpers;

        ignoreHeader = isRawPCM;
        if(isRawPCM) return;

        if (metadataValues.size() > 0)
        {
            // The meta data should have been sanitised for the WAV format.
            // If it was originally sourced from an AIFF or  WAV file the MetaDataSource
            // key should be removed (or set to "IFF") once this has been done
            jassert (metadataValues.getValue ("MetaDataSource", "None") != "WAV");
            jassert (metadataValues.getValue ("MetaDataSource", "None") != "AIFF");

            auto map = toMap (metadataValues);

            if(map.find("SamplesPerSec") == map.end()) map.merge(StringMap({ { "SamplesPerSec", juce::String(sampleRate) } }));

            vhdrChunk = Voice8Header::createFrom (map);
        }

        headerPosition = out->getPosition();
        writeHeader();
    }

    ~IffAudioFormatWriter() override
    {
        writeHeader();
    }

    //==============================================================================
    bool write (const int** data, int numSamples) override
    {
        jassert (numSamples >= 0);
        jassert (data != nullptr && *data != nullptr); // the input must contain at least one channel!

        if (writeFailed)
            return false;

        auto bytes = numChannels * (size_t) numSamples * bitsPerSample / 8;
        tempBlock.ensureSize (bytes, false);

        WriteHelper<juce::AudioData::Int8, juce::AudioData::Int32, juce::AudioData::BigEndian>::write (tempBlock.getData(), (int) numChannels, data, numSamples);

        if (! output->write (tempBlock.getData(), bytes) && !ignoreHeader)
        {
            // failed to write to disk, so let's try writing the header.
            // If it's just run out of disk space, then if it does manage
            // to write the header, we'll still have a usable file..
            writeHeader();
            writeFailed = true;
            return false;
        }

        bytesWritten += bytes;
        lengthInSamples += (juce::uint64) numSamples;
        return true;
    }

    bool flush() override
    {
        auto lastWritePos = output->getPosition();
        if(!ignoreHeader) writeHeader();

        if (output->setPosition (lastWritePos))
            return true;

        // if this fails, you've given it an output stream that can't seek! It needs
        // to be able to seek back to write the header
        jassertfalse;
        return false;
    }

private:
    juce::MemoryBlock tempBlock, vhdrChunk;
    juce::uint64 lengthInSamples = 0, bytesWritten = 0;
    juce::int64 headerPosition = 0;
    bool writeFailed = false, ignoreHeader = false;

    void writeHeader()
    {
        using namespace IffFileHelpers;
            
        if (ignoreHeader) return;

        if ((bytesWritten & 1) != 0) // pad to an even length
            output->writeByte (0);

        if (headerPosition != output->getPosition() && ! output->setPosition (headerPosition))
        {
            // if this fails, you've given it an output stream that can't seek! It needs to be
            // able to seek back to go back and write the header after the data has been written.
            jassertfalse;
            return;
        }

        const size_t bytesPerFrame = numChannels * bitsPerSample / 8;
        juce::uint64 audioDataSize = bytesPerFrame * lengthInSamples;

        juce::int64 iffChunkSize = (juce::int64) (8 + 20 /* 8SVXVHDR */ + 8 + audioDataSize + (audioDataSize & 1));

        iffChunkSize += (iffChunkSize & 1);

        writeChunkHeader (chunkName ("FORM"), (int) iffChunkSize);

        output->writeInt (chunkName ("8SVX"));
            
        if(vhdrChunk.isEmpty())
        {
            writeChunkHeader (chunkName ("VHDR"), 20);

            output->writeIntBigEndian ((juce::int32) lengthInSamples);
            output->writeIntBigEndian ((ULONG) 0);
            output->writeIntBigEndian ((ULONG) 32);

            output->writeShort(juce::ByteOrder::swapIfLittleEndian((juce::int16) sampleRate));
            output->writeByte ((juce::int8) 1);
            output->writeByte ((UBYTE) 0);

            output->writeIntBigEndian ((Fixed) 0x10000);
        }
        else
        {
            writeChunk (vhdrChunk, chunkName ("VHDR"));
        }

        writeChunkHeader (chunkName ("BODY"), (int) (lengthInSamples * bytesPerFrame));

        usesFloatingPointData = false;
    }

    static size_t chunkSize (const juce::MemoryBlock& data) noexcept     { return data.isEmpty() ? 0 : (8 + data.getSize()); }

    void writeChunkHeader (int chunkType, int size) const
    {
        output->writeInt (chunkType);
        output->writeIntBigEndian(size);
    }

    void writeChunk (const juce::MemoryBlock& data, int chunkType, int size = 0) const
    {
        if (! data.isEmpty())
        {
            writeChunkHeader (chunkType, size != 0 ? size : (int) data.getSize());
            *output << data;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IffAudioFormatWriter)
};

//==============================================================================
IffAudioFormat::IffAudioFormat() : juce::AudioFormat(iffFormatName, ".iff .8svx .raw \0") {}
IffAudioFormat::~IffAudioFormat() {}

juce::Array<int> IffAudioFormat::getPossibleSampleRates()
{
    /* NTSC Amiga sample rates calculated from clock/period */
    return { 4181, 4430, 4697, 4971, 5279, 5593, 5926, 6279, 6653, 
                7046, 7457, 7901, 8363, 8860, 9395, 9943, 10559, 11186, 
                11852, 12559, 13306, 14092, 14914, 15838, 16726, 17720, 
                18839, 19886, 21056, 22372, 23705, 25031, 26515, 28185 };
}

juce::Array<int> IffAudioFormat::getPossibleBitDepths() { return 8; }

bool IffAudioFormat::canDoStereo()  { return false; }
bool IffAudioFormat::canDoMono()    { return true; }

bool IffAudioFormat::isChannelLayoutSupported(const juce::AudioChannelSet& channelSet)
{ 
    return channelSet == juce::AudioChannelSet::mono() ? true : false;
}

juce::AudioFormatReader* IffAudioFormat::createReaderFor (juce::InputStream* sourceStream, bool deleteStreamIfOpeningFails)
{
    std::unique_ptr<IffAudioFormatReader> r (new IffAudioFormatReader (sourceStream));

    if (r->sampleRate > 0 && r->numChannels > 0 && r->bytesPerFrame > 0 && r->bitsPerSample <= 32)
        return r.release();

    if (! deleteStreamIfOpeningFails)
        r->input = nullptr;

    return nullptr;
}

juce::AudioFormatWriter* IffAudioFormat::createWriterFor(juce::OutputStream* out)
{
    if (out != nullptr)
        return new IffAudioFormatWriter(out, true, 8363, juce::AudioChannelSet::mono(), 8, 0);

    return nullptr;
}

juce::AudioFormatWriter* IffAudioFormat::createWriterFor (juce::OutputStream* out, double sampleRate,
                                                          unsigned int numChannels, int bitsPerSample,
                                                          const juce::StringPairArray& metadataValues, int qualityOptionIndex)
{
    if (out != nullptr)
        return createWriterFor (out, sampleRate, IffFileHelpers::canonicalWavChannelSet (static_cast<int> (numChannels)), 
                                bitsPerSample, metadataValues, qualityOptionIndex);
            
    return nullptr;
}

juce::AudioFormatWriter* IffAudioFormat::createWriterFor (juce::OutputStream* out,
                                                          double sampleRate,
                                                          const juce::AudioChannelSet& channelLayout,
                                                          int bitsPerSample,
                                                          const juce::StringPairArray& metadataValues,
                                                          int /*qualityOptionIndex*/)
{
    if (out != nullptr && getPossibleBitDepths().contains (bitsPerSample) && isChannelLayoutSupported (channelLayout))
        return new IffAudioFormatWriter (out, false, sampleRate, channelLayout,
                                            (unsigned int) bitsPerSample, metadataValues);

    return nullptr;
}
//==============================================================================
