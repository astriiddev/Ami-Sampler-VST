/*
  ==============================================================================

    RCFilters.cpp
    Created: 3 Jun 2023 12:09:35am
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "RCFilters.h"

/*
  ==============================================================================


  //// Amiga RC and LED Filters ////

  ///// C++/JUCE rewrite of 8bitbubsy's Amiga filters :
		https://github.com/8bitbubsy/pt2-clone/blob/master/src/pt2_rcfilters.c \\\\\\

  ==============================================================================
*/

/* 1-pole RC low-pass/high-pass filter, based on:
** https://www.musicdsp.org/en/latest/Filters/116-one-pole-lp-and-hp.html
*/

RCFilter::RCFilter(AmiSamplerAudioProcessor& p) : audioProcessor(p) {}

RCFilter::~RCFilter() {}

void RCFilter::setupOnePoleFilter(double audioRate, double cutOff, OnePoleFilter_t *f)
{
	if (cutOff >= audioRate/2.0)
		cutOff = (audioRate/2.0) - smallNumber;

	const double a = 2.0 - std::cos((twoPi * cutOff) / audioRate);
	const double b = a - std::sqrt((a * a) - 1.0);

	f->a1 = 1.0 - b;
	f->a2 = b;
}

void RCFilter::clearOnePoleFilterState(OnePoleFilter_t *f)
{
	f->tmpL = f->tmpR = 0.0;
}

float RCFilter::onePoleLPFilter(OnePoleFilter_t *f, const float in)
{
	f->tmpL = (f->a1 * in) + (f->a2 * f->tmpL);
	return f->tmpL;
}

float RCFilter::onePoleHPFilter(OnePoleFilter_t *f, const float in)
{
	f->tmpL = (f->a1 * in) + (f->a2 * f->tmpL);
	return (in - f->tmpL);
}


/* 2-pole RC low-pass filter with Q factor, based on:
** https://www.musicdsp.org/en/latest/Filters/38-lp-and-hp-filter.html
*/

void RCFilter::setupTwoPoleFilter(double audioRate, double cutOff, double qFactor, TwoPoleFilter_t *f)
{
	if (cutOff >= audioRate/2.0)
		cutOff = (audioRate/2.0) - smallNumber;

	const double a = 1.0 / std::tan((pi * cutOff) / audioRate);
	const double b = 1.0 / qFactor;

	f->a1 = 1.0 / (1.0 + b * a + a * a);
	f->a2 = 2.0 * f->a1;
	f->b1 = 2.0 * (1.0 - a*a) * f->a1;
	f->b2 = (1.0 - b * a + a * a) * f->a1;
}

void RCFilter::clearTwoPoleFilterState(TwoPoleFilter_t *f)
{
	f->tmpL[0] = f->tmpL[1] = f->tmpL[2] = f->tmpL[3] = 0.0;
	f->tmpR[0] = f->tmpR[1] = f->tmpR[2] = f->tmpR[3] = 0.0;
}

float RCFilter::twoPoleLPFilter(TwoPoleFilter_t *f, const float in)
{
	const double out = (f->a1 * in) + (f->a2 * f->tmpL[0]) + (f->a1 * f->tmpL[1]) - (f->b1 * f->tmpL[2]) - (f->b2 * f->tmpL[3]);

	// shift states

	f->tmpL[1] = f->tmpL[0];
	f->tmpL[0] = in;
	f->tmpL[3] = f->tmpL[2];
	f->tmpL[2] = out;

	// set output

	return out;
}
