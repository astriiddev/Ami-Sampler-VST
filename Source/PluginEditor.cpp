/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginEditor.h"

//==============================================================================
AmiAudioProcessorEditor::AmiAudioProcessorEditor(AmiAudioProcessor& p) : AudioProcessorEditor (&p), audioProcessor(p)
{
    const double screen_scale = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale;
    const int screen_width = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->totalArea.getWidth();
    
    const int res_width = (int) std::floor(screen_scale * (double) screen_width);
    juce::Image btn_up, btn_dwn;
    
    setLookAndFeel(&lookNFeel);
    setRepaintsOnMouseActivity(false);

    setScaleFactor(audioProcessor.getCurrentScaleFactor());
    setOpaque(false);

    amiWindow = std::make_unique<AmiWindowEditor>(audioProcessor);
    amiWindow->setFont(lookNFeel.getCustomFont());

    addAndMakeVisible(amiWindow.get());

    btn_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin1_1_png, BinaryData::amiwin1_1_pngSize);
    btn_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin1_2_png, BinaryData::amiwin1_2_pngSize);
    
    initImgButton(&resetScale, btn_up.rescaled(41, 34, juce::Graphics::lowResamplingQuality),
                  btn_dwn.rescaled(41, 34, juce::Graphics::lowResamplingQuality));
    
    btn_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin2_1_png, BinaryData::amiwin2_1_pngSize);
    btn_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin2_2_png, BinaryData::amiwin2_2_pngSize);
    
    initImgButton(&downScale, btn_up.rescaled(41, 34, juce::Graphics::lowResamplingQuality),
                  btn_dwn.rescaled(41, 34, juce::Graphics::lowResamplingQuality));
    
    btn_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin3_1_png, BinaryData::amiwin3_1_pngSize);
    btn_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin3_2_png, BinaryData::amiwin3_2_pngSize);
    
    initImgButton(&upScale, btn_up.rescaled(44, 34, juce::Graphics::lowResamplingQuality),
                  btn_dwn.rescaled(44, 34, juce::Graphics::lowResamplingQuality));
    
    setSize (1080, 640);

    startTimer(100);
    amiWindow->setInterceptsMouseClicks(false, true);

    if(res_width >= 3840)
        lookNFeel.setMouseCursorSize(44);
    else if(res_width >= 1920)
        lookNFeel.setMouseCursorSize(33);
    else if(res_width >= 1280)
        lookNFeel.setMouseCursorSize(22);
    else
        lookNFeel.setMouseCursorSize(11);
}

AmiAudioProcessorEditor::~AmiAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void AmiAudioProcessorEditor::paint (juce::Graphics&) {}

void AmiAudioProcessorEditor::resized()
{
    amiWindow->setBounds(getBounds());
    
    resetScale.toFront(false);
    resetScale.setBounds(7, 0, 41, 34);
    downScale.setBounds(722, 0, 41, 34);
    upScale.setBounds(763, 0, 44, 34);
}

void AmiAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
    amiWindow->mouseDown(e);
}

void AmiAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e)
{
    amiWindow->mouseDrag(e);
}

void AmiAudioProcessorEditor::mouseUp(const juce::MouseEvent& e)
{
    amiWindow->mouseUp(e);
}

void AmiAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    amiWindow->mouseWheelMove(e, wheel);
}

bool AmiAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    if (files.size() != 1) return false;

    for (const juce::String& fileName : files)
    {
        const juce::String file = fileName.toLowerCase();

        if (file.contains(".wav"))  return true;
        if (file.contains(".aif"))  return true;
        if (file.contains(".aiff")) return true;
        if (file.contains(".iff"))  return true;
        if (file.contains(".8svx")) return true;
        if (file.contains(".brr"))  return true;
        if (file.contains(".raw"))  return true;
        if (file.contains(".bin"))  return true;
        if (!file.contains("."))    return true;    // Amiga samples don't typically have a file extension
    }

    return false;
}

void AmiAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if(!isInterestedInFileDrag(files)) return;

    amiWindow->handleLoad(juce::File(files.strings.getFirst()));
}

void AmiAudioProcessorEditor::mouseMove(const juce::MouseEvent& e)
{
    amiWindow->mouseMove(e);
}

bool AmiAudioProcessorEditor::keyPressed(const juce::KeyPress &k)
{
    if(alertWin.get() == nullptr)
        amiWindow->grabKeyboardFocus();
    else
    {
        amiWindow->giveAwayKeyboardFocus();
        alertWin->grabKeyboardFocus();
    }
    
    if (juce::JUCEApplicationBase::isStandaloneApp())
    {
        if (k.getTextDescription().compareIgnoreCase("alt + F4") == 0)
            juce::JUCEApplicationBase::quit();

        if ((juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) == 0)
        {
            if (k.getTextDescription().compareIgnoreCase("ctrl + Q") == 0)
                juce::JUCEApplicationBase::quit();
        }

        else if ((juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0)
        {
            if (k.getTextDescription().compareIgnoreCase("command + Q") == 0)
                juce::JUCEApplicationBase::quit();
        }

        return true;
    }

    return false;
}

void AmiAudioProcessorEditor::initImgButton(juce::ImageButton* b, const juce::Image& up, const juce::Image& down)
{
    const juce::Colour c = juce::Colours::transparentBlack;

    b->addListener(this);
    addAndMakeVisible(*b);

    b->setImages(true, true, true, up, 1.0f, c, up, 1.0, c, down, 1.0f, c, 0.0f);
}

void AmiAudioProcessorEditor::buttonClicked(juce::Button *button)
{
    /* Disables plugin rescaling in Linux as Linux doesn't handle plugin rescaling well */
    if(!juce::JUCEApplicationBase::isStandaloneApp() && (juce::SystemStats::getOperatingSystemType() & juce::SystemStats::Linux)) return;

    if(button == &resetScale) audioProcessor.resetScaleFactor();
    
    if(button == &downScale) audioProcessor.decScaleFactor();
    
    if(button == &upScale) audioProcessor.incScaleFactor();
    
    setScaleFactor(audioProcessor.getCurrentScaleFactor());
}

void AmiAudioProcessorEditor::visibilityChanged()
{
    setScaleFactor(audioProcessor.getCurrentScaleFactor());
}

void AmiAudioProcessorEditor::timerCallback()
{
    if(alertWin.get() == nullptr)
    {
        if(!amiWindow->alertWinShowing()) return;

        createAlertWin(amiWindow->getAlertWinTitle(), amiWindow->getAlertWinMessage());
    }

    if (!alertWin->wasOkPressed()) return;
 
    amiWindow->alertWindowCleared();
    amiWindow->setInterceptsMouseClicks(true, true);
    amiWindow->grabKeyboardFocus();

    alertWin.reset();
}

void AmiAudioProcessorEditor::createAlertWin(const juce::String &title, const juce::String &message)
{
    int width = 600, height = 200;
    juce::String line_1, line_2;

    if((line_1 = message.upToLastOccurrenceOf("\n", false, false)).length() * 16 > width)
    {
        width = line_1.length() * 16 + 100;
        width -= width % 100;
    }
    
    if((line_2 = message.upToLastOccurrenceOf("\n", false, false)).length() * 16 > width)
    {
        width = line_2.length() * 16 + 100;
        width -= width % 100;
    }

    alertWin = std::make_unique<AmiAlertWindow>(width, height, title, message);
    alertWin->setBounds((getWidth() - width) / 2, proportionOfHeight(0.52f), width, height);

    addAndMakeVisible(alertWin.get());
    alertWin->toFront(true);

    amiWindow->setInterceptsMouseClicks(false, false);
    amiWindow->giveAwayKeyboardFocus();
}
