/*
  ==============================================================================

    RCFilters.h
    Created: 3 Jun 2023 12:09:35am
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/*
  ==============================================================================


  //// Amiga RC and LED Filters ////

  ///// C++/JUCE rewrite of 8bitbubsy's Amiga filters :
        https://github.com/8bitbubsy/pt2-clone/blob/master/src/pt2_rcfilters.h \\\\\\

  ==============================================================================
*/

class RCFilter 
{

public:

    RCFilter(AmiSamplerAudioProcessor&);
    ~RCFilter();

    typedef struct OnePoleFilter_t
    {
    public:

        double tmpL, tmpR, a1, a2;

    } OnePoleFilter_t;

    typedef struct  TwoPoleFilter_t
    {
    public:

        double tmpL[4], tmpR[4], a1, a2, b1, b2;

    } TwoPoleFilter_t;

    void clearOnePoleFilterState(OnePoleFilter_t *f);
    void clearTwoPoleFilterState(TwoPoleFilter_t* f);

    void setupOnePoleFilter(double audioRate, double cutOff, OnePoleFilter_t* f);
    void setupTwoPoleFilter(double audioRate, double cutOff, double qFactor, TwoPoleFilter_t* f);

    float onePoleLPFilter(OnePoleFilter_t *f, const float in);
    float onePoleHPFilter(OnePoleFilter_t *f, const float in);

    float twoPoleLPFilter(TwoPoleFilter_t *f, const float in);

private:
    
    const double smallNumber{ 1E-4 };
    const float twoPi = juce::MathConstants<double>::twoPi;
    const float pi = juce::MathConstants<double>::pi;

    AmiSamplerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RCFilter)

};
