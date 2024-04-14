/*
  ==============================================================================

    ami_palette.h
    Created: 16 Dec 2023 5:39:12pm
    Author:  _astriid_

  ==============================================================================
*/

<<<<<<< HEAD
#include <JuceHeader.h>

#ifndef __AMI_PALETTE_H
#define __AMI_PALETTE_H

#define JPAL(c)   juce::Colour((juce::uint32) (c))
=======
#ifndef __AMI_PALETTE_H
#define __AMI_PALETTE_H

#define JPAL(c)   juce::Colour(c)
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

constexpr uint32_t AMI_BLU = 0xFF0054AA;
constexpr uint32_t AMI_BLL = 0xFF006ADD;
constexpr uint32_t AMI_BLD = 0xFF002049;

constexpr uint32_t AMI_ORG = 0xFFFC8A00;
constexpr uint32_t AMI_WHT = 0xFFFFFFFF;
constexpr uint32_t AMI_BLK = 0xFF000000;

constexpr uint32_t AMI_GRY = 0xFF9E9E9E;
constexpr uint32_t AMI_RED = 0xFFC92D28;
constexpr uint32_t AMI_YLW = 0xFFF9D800;

#endif /* __AMI_PALETTE_H */
