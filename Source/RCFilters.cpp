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

RCFilter::RCFilter() {}

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

void RCFilter::onePoleLPFilter(OnePoleFilter_t *f, const float* inL, const float* inR, float* outL, float* outR)
{
	f->tmpL = (*inL * f->a1) + (f->tmpL * f->a2);
	*outL = (float) f->tmpL;

	f->tmpR = (*inR * f->a1) + (f->tmpR * f->a2);
	*outR = (float) f->tmpR;
}

void RCFilter::onePoleHPFilter(OnePoleFilter_t *f, const float* inL, const float* inR, float* outL, float* outR)
{
	f->tmpL = (*inL * f->a1) + (f->tmpL * f->a2);
	*outL = (float) (*inL - f->tmpL);

	f->tmpR = (*inR * f->a1) + (f->tmpR * f->a2);
	*outR = (float) (*inR - f->tmpR);
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

void RCFilter::twoPoleLPFilter(TwoPoleFilter_t *f, const float* inL, const float* inR, float* outL, float* outR)
{
	const double LOut = (*inL * f->a1) + (f->tmpL[0] * f->a2) + (f->tmpL[1] * f->a1) - (f->tmpL[2] * f->b1) - (f->tmpL[3] * f->b2);
	const double ROut = (*inR * f->a1) + (f->tmpR[0] * f->a2) + (f->tmpR[1] * f->a1) - (f->tmpR[2] * f->b1) - (f->tmpR[3] * f->b2);

	// shift states

	f->tmpL[1] = f->tmpL[0];
	f->tmpL[0] = *inL;
	f->tmpL[3] = f->tmpL[2];
	f->tmpL[2] = LOut;

	f->tmpR[1] = f->tmpR[0];
	f->tmpR[0] = *inR;
	f->tmpR[3] = f->tmpR[2];
	f->tmpR[2] = ROut;

	// set output

	*outL = (float) LOut;
	*outR = (float) ROut;
}
