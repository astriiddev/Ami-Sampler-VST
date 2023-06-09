/*
  ==============================================================================

    WaveWindow.cpp
    Created: 19 May 2023 10:08:47pm
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveWindow.h"

/*
  ==============================================================================


  //// Workbench 1.3-inspired window graphics ////


  ==============================================================================
*/

//==============================================================================
WaveWindow::WaveWindow(AmiSamplerAudioProcessor& p) : audioProcessor(p)
{
    //!!!! TODO: Clean this up and make it reusable for window graphics of other components !!!!//
}

WaveWindow::~WaveWindow()
{
}

void WaveWindow::paint (juce::Graphics& g)
{
    juce::Rectangle<int> sampleArea(getWidth(), getHeight());
    g.setColour(juce::Colours::white);
    g.drawRect(sampleArea, proportionOfWidth(0.03125f));
    
    /* Right and bottom blue rectangles */
    g.setColour(juce::Colour(0xff0054aa));
    g.drawRect(juce::Rectangle<int>(proportionOfWidth(0.973f), proportionOfHeight(0.11f), proportionOfWidth(0.022f), proportionOfHeight(0.78f)), proportionOfHeight(0.014f));
    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.033f), proportionOfHeight(0.91f), proportionOfWidth(0.934f), proportionOfHeight(0.08f)));

    /*!!!!TODO Workbench 1.3 style "close," "to front," and "to back" graphic icons // 
      these probably won't have the actual Workbench functionality but they'll make the ~aesthetic~ more accurate and prettier :) !!!!*/
    /* Sampler title and blue lines running next to title */
    auto theFont = g.getCurrentFont();
    theFont.setHeight(proportionOfWidth(0.03f));
    g.setFont(theFont);
    g.drawFittedText("Ami Sampler 1.3", (getLocalBounds().reduced(proportionOfWidth(0.03925f), 0)), juce::Justification::topLeft, 1);
    juce::Line<float> winLine1(juce::Point<float>(proportionOfWidth(0.255f), proportionOfHeight(0.025f)), juce::Point<float>(proportionOfWidth(0.967f), proportionOfHeight(0.025f)));
    g.drawLine(winLine1, proportionOfWidth(0.00625f));
    juce::Line<float> winLine2(juce::Point<float>(proportionOfWidth(0.255), proportionOfHeight(0.075f)), juce::Point<float>(proportionOfWidth(0.967f), proportionOfHeight(0.075f)));
    g.drawLine(winLine2, proportionOfWidth(0.00625f));

    /* Left side orange rectangle */
    g.setColour(juce::Colour(0xfffc8a00));
    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.0045f), proportionOfHeight(0.11f), proportionOfWidth(0.022f), proportionOfHeight(0.78f)));

    /* Black box around F and E */
    g.setColour(juce::Colours::black);
    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.0045f), proportionOfHeight(0.11f), proportionOfWidth(0.022f), proportionOfHeight(0.0975f)));
    g.fillRect(juce::Rectangle<int>(proportionOfWidth(0.0045f), proportionOfHeight(0.7925f), proportionOfWidth(0.022f), proportionOfHeight(0.0975f)));

    /* Workbench "First" and "End" scroll snap buttons */
    /*???? Possible TODO: these are just for aesthet at the moment //
      but as I type this it occured to me I could make these snap the waveform cursor to start and end loop slider positions ????*/
    auto feFont = juce::Font((juce::Font::bold, proportionOfWidth(0.03f)));
    g.setColour(juce::Colours::white);
    g.setFont(feFont);
    g.drawText("F", juce::Rectangle<int>(proportionOfWidth(0.0053f), proportionOfHeight(0.11f), proportionOfWidth(0.022f), proportionOfHeight(0.0975f)), juce::Justification::centred, false);
    g.drawText("E", juce::Rectangle<int>(proportionOfWidth(0.0053f), proportionOfHeight(0.791f), proportionOfWidth(0.022f), proportionOfHeight(0.1f)), juce::Justification::centred, false);
}

void WaveWindow::resized()
{
}