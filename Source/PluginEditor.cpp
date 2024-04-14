/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

<<<<<<< HEAD
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
=======
#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "ami_palette.h"

//==============================================================================
AmiAudioProcessorEditor::AmiAudioProcessorEditor (AmiAudioProcessor& p)
    : AudioProcessorEditor (&p), handleGui(p), keyboardComponent(p.getKeyState(), juce::MidiKeyboardComponent::horizontalKeyboard),
       audioProcessor (p)
{
    setLookAndFeel(&lookNFeel);
    setScaleFactor(audioProcessor.getCurrentScaleFactor());
    lookNFeel.setMouseCursorSize(audioProcessor.getCurrentScaleFactor());

    addAndMakeVisible(handleGui);
    handleGui.setInterceptsMouseClicks(false, true);
    handleGui.toBack();

    waveMenu.setPixelArea(135, 160);
    waveMenu.setInterceptsMouseClicks(false, false);
    drawWaveMenu();

    for (int i = 0; i < numWaveforms; i++)
    {
        currentSample = i;
        const juce::String lpAtch = "LOOP ENABLE" + juce::String(currentSample);

        audioProcessor.getAPVTS().addParameterListener(lpAtch, this);
        initWaveforms(&waveform[i]);
    }

    currentSample = 0;
    juce::String sampName = audioProcessor.getSampleName(currentSample);

    addAndMakeVisible(logoImage);
    addAndMakeVisible(keyboardComponent);

    addAndMakeVisible(waveMenu);

    initButton(&loadButton);
    initButton(&saveButton);

    juce::Image clearOff = juce::ImageCache::getFromMemory(BinaryData::amiTrashOff_png, BinaryData::amiTrashOff_pngSize);
    juce::Image clearClicked = juce::ImageCache::getFromMemory(BinaryData::amiTrashOn_png, BinaryData::amiTrashOn_pngSize);

    initImgButton(&clearSampleButton, clearOff.rescaled(60, 66, juce::Graphics::lowResamplingQuality),
                  clearClicked.rescaled(60, 66, juce::Graphics::lowResamplingQuality));
    
    juce::Image amiwin1_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin1_1_png, BinaryData::amiwin1_1_pngSize);
    juce::Image amiwin1_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin1_2_png, BinaryData::amiwin1_2_pngSize);
    
    initImgButton(&resetScale, amiwin1_up.rescaled(41, 34, juce::Graphics::lowResamplingQuality),
                  amiwin1_dwn.rescaled(41, 34, juce::Graphics::lowResamplingQuality));
    
    juce::Image amiwin2_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin2_1_png, BinaryData::amiwin2_1_pngSize);
    juce::Image amiwin2_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin2_2_png, BinaryData::amiwin2_2_pngSize);
    
    initImgButton(&downScale, amiwin2_up.rescaled(41, 34, juce::Graphics::lowResamplingQuality),
                  amiwin2_dwn.rescaled(41, 34, juce::Graphics::lowResamplingQuality));
    
    juce::Image amiwin3_up  = juce::ImageCache::getFromMemory(BinaryData::amiwin3_1_png, BinaryData::amiwin3_1_pngSize);
    juce::Image amiwin3_dwn = juce::ImageCache::getFromMemory(BinaryData::amiwin3_2_png, BinaryData::amiwin3_2_pngSize);
    
    initImgButton(&upScale, amiwin3_up.rescaled(44, 34, juce::Graphics::lowResamplingQuality),
                  amiwin3_dwn.rescaled(44, 34, juce::Graphics::lowResamplingQuality));
    
    juce::Image logo = juce::ImageCache::getFromMemory(BinaryData::astriid_amiga_png, BinaryData::astriid_amiga_pngSize);

    if (!logo.isNull())
        logoImage.setImage(logo, juce::RectanglePlacement::stretchToFit);
    else
        jassert(!logo.isNull());

    keyboardComponent.clearKeyMappings();
    keyboardComponent.setAvailableRange(36, 108);
    keyboardComponent.setFont(lookNFeel.getCustomFont());

    keyboardComponent.setKeyWidth(21);
    keyboardComponent.setBlackNoteLengthProportion(0.66f);

    keyboardComponent.setColour(juce::MidiKeyboardComponent::keySeparatorLineColourId, JPAL(AMI_BLK));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::ColourIds::whiteNoteColourId, JPAL(AMI_BLK));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::shadowColourId, JPAL(AMI_BLK));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, JPAL(AMI_WHT));

    setWantsKeyboardFocus(true);
    setRepaintsOnMouseActivity(false);

    audioProcessor.getMidiCollector().reset(audioProcessor.getDevSampleRate());

    juce::zeromem(keysPressed, 50);
  
    startTimer(80);
    setSize (1080, 640);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

AmiAudioProcessorEditor::~AmiAudioProcessorEditor()
{
<<<<<<< HEAD
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
=======
    for (int i = 0; i < numWaveforms; i++)
    {
        const juce::String lpAtch = "LOOP ENABLE" + juce::String(i);
        audioProcessor.getAPVTS().removeParameterListener(lpAtch, this);
    }

    setLookAndFeel(nullptr);
}

//==============================================================================
void AmiAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(JPAL(AMI_BLU));
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    currentSample = audioProcessor.getCurrentSample();

    const int midiChannel = audioProcessor.getMidiChannel(currentSample);

    const int samp_len = audioProcessor.getWaveForm(currentSample).getNumSamples();

    if (!waveform[currentSample].isVisible() || (isOnDesktop() && samp_len > 0 && waveform[currentSample].getPointX(samp_len) < 404))
    {
        if(midiChannel > 0) keyboardComponent.setMidiChannel(audioProcessor.getMidiChannel(currentSample));

        waveform[currentSample].setLoopStart(audioProcessor.getLoopStart(currentSample));
        waveform[currentSample].setLoopEnd(audioProcessor.getLoopEnd(currentSample));

        waveform[currentSample].updateWaveform();

        waveform[currentSample].copyPixelBuffer();
        waveform[currentSample].setVisible(true);
        waveform[currentSample].setEnabled(true);
        waveform[currentSample].toBack();

        drawWaveMenu();

        repaint(waveBox);
    }

    if (midiChannel > 0 && currMidiChannel != midiChannel)
        currMidiChannel = midiChannel;

    if (waveform[currentSample].getNewLoopStart() != audioProcessor.getLoopStart(currentSample))
    {
        waveform[currentSample].setLoopStart(audioProcessor.getLoopStart(currentSample));
        repaint(waveform[currentSample].getBounds());
    }

    if(waveform[currentSample].getNewLoopEnd() != audioProcessor.getLoopEnd(currentSample))
    {
        waveform[currentSample].setLoopEnd(audioProcessor.getLoopEnd(currentSample));
        repaint(waveBox);
    }

    g.setColour(JPAL(AMI_GRY - 0x00606060));
    g.fillRect(proportionOfWidth(0.75f), 0, proportionOfWidth(0.25f), 2);

    g.setColour(JPAL(AMI_BLD));
    g.fillRect(proportionOfWidth(0.88f), getHeight() - 2, proportionOfWidth(0.12f), 2);
    
    g.setColour(JPAL(AMI_BLL));

    g.fillRect(0, 0, 2, getHeight());
    g.fillRect(0, proportionOfHeight(0.5f), getWidth(), 2);
    g.fillRect(0, proportionOfHeight(0.72f) + 3, getWidth(), 1);

    g.fillRect(proportionOfWidth(0.88f) + 4, proportionOfHeight(0.72f) + 2, 1, proportionOfHeight(0.3f) - 2);
    g.fillRect(proportionOfWidth(0.71f) + 3, proportionOfHeight(0.72f) + 2, 1, proportionOfHeight(0.125f) - 2);
    g.fillRect(proportionOfWidth(0.17f) + 2, proportionOfHeight(0.72f) + 2, 2, proportionOfHeight(0.125f) - 2);

    g.fillRect(proportionOfWidth(0.23f) + 3, proportionOfHeight(0.5f), 1, proportionOfHeight(0.22f));
    g.fillRect(proportionOfWidth(0.46f) + 2, proportionOfHeight(0.5f), 2, proportionOfHeight(0.345f));

    g.setColour(JPAL(AMI_BLD));

    g.fillRect(getWidth() - 2, 0, 2, getHeight());
    g.fillRect(0, proportionOfHeight(0.72f), getWidth(), 2);

    g.fillRect(proportionOfWidth(0.88f) + 1, proportionOfHeight(0.72f) + 2, 2, proportionOfHeight(0.3f) - 2);
    g.fillRect(proportionOfWidth(0.71f), proportionOfHeight(0.72f) + 2, 2, proportionOfHeight(0.125f) - 2);
    g.fillRect(proportionOfWidth(0.17f), proportionOfHeight(0.72f) + 2, 2, proportionOfHeight(0.125f) - 2);

    g.fillRect(proportionOfWidth(0.23f), proportionOfHeight(0.5f), 2, proportionOfHeight(0.22f));
    g.fillRect(proportionOfWidth(0.46f), proportionOfHeight(0.5f), 2, proportionOfHeight(0.345f));

    g.fillRect(0, proportionOfHeight(0.85f) - 3, proportionOfWidth(0.88f) + 1, 2);
}

void AmiAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    const int lowNote = audioProcessor.getLowNote(currentSample);
    const int hiNote = audioProcessor.getHighNote(currentSample);
    g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);

    g.setColour(JPAL(AMI_BLU));
    g.setFont(lookNFeel.getCustomFont().withHeight(28.f));
    g.drawText("Ami Sampler 1.3", waveBox.withX(waveBox.getX() + 50).withY(waveBox.getY() + 3), 
                juce::Justification::topLeft, false);

    g.fillRect(scrollBack);
    const float widthRatio = waveform[currentSample].getWidthRatio();
    const juce::Rectangle scroll_bar = { (int)(waveform[currentSample].getScrollBar().getX() * widthRatio),
                                         (int)(waveform[currentSample].getScrollBar().getY() * widthRatio),
                                         (int)(waveform[currentSample].getScrollBar().getWidth() * widthRatio),
                                         (int)(waveform[currentSample].getScrollBar().getHeight() * widthRatio) };
    
    g.setColour(JPAL(scrollcolor));
    g.fillRect(scroll_bar);

    g.setColour(JPAL(0xA0121212));
    
    if(lowNote > 36)
    {
        juce::Rectangle lowkeyWidth = keyboardComponent.getRectangleForKey(lowNote);
        if (lowkeyWidth.getHeight() < keyboardComponent.getWhiteNoteLength()) lowkeyWidth = keyboardComponent.getRectangleForKey(lowNote + 1);

        g.fillRect((float)keyboardComponent.getBounds().getX(), (float)keyboardComponent.getBoundsInParent().getY(),
            lowkeyWidth.getX(), (float)keyboardComponent.getBoundsInParent().getHeight() + 1.f);
    }

    if (lastLowKey != lowNote)
    {
        repaint(keyboardComponent.getBounds());
        lastLowKey = lowNote;
    }

    if (hiNote < 108)
    {
        juce::Rectangle hikeyWidth = keyboardComponent.getRectangleForKey(hiNote + 1);
        if (hikeyWidth.getHeight() < keyboardComponent.getWhiteNoteLength()) hikeyWidth = keyboardComponent.getRectangleForKey(hiNote + 2);

        g.fillRect(hikeyWidth.getRight() + 1.f, (float)keyboardComponent.getBoundsInParent().getY() - 1.f,
            (float)keyboardComponent.getBoundsInParent().getWidth() - (float)hikeyWidth.getX() + 1.f, 
            (float)keyboardComponent.getBoundsInParent().getHeight() + 1.f);
    }

    if (lastHighkey != hiNote)
    {
        repaint(keyboardComponent.getBounds());
        lastHighkey = hiNote;
    }

    const juce::Rectangle keyRect  = keyboardComponent.getBounds();
    const juce::Rectangle keyEdge1 = { keyRect.getX() - proportionOfWidth(0.015f),  keyRect.getY() - 2, proportionOfWidth(0.015f), keyRect.getHeight() + 4};
    const juce::Rectangle keyEdge2 = { keyRect.getRight(), keyRect.getY() - 2, proportionOfWidth(0.015f), keyRect.getHeight() + 4};

    g.setColour(JPAL(AMI_BLL));

    g.fillRect(keyEdge2.getRight(), keyEdge2.getY() - 2, 2, keyEdge2.getHeight() + 4);
    g.fillRect(keyEdge1.getX() - 2, keyEdge2.getBottom(), (keyEdge1.getWidth() * 2) + keyRect.getWidth() + 4, 2);

    g.setColour(JPAL(AMI_BLD));

    g.fillRect(0, getBottom() - 2, getWidth(), 2);

    g.fillRect(keyEdge1.getX() - 2, keyEdge1.getY() - 2, 2, keyEdge1.getHeight() + 4);
    g.fillRect(keyEdge1.getX() - 2, keyEdge1.getY() - 2, (keyEdge1.getWidth() * 2) + keyRect.getWidth() + 4, 2);

    g.setColour(JPAL(AMI_BLK));
    g.drawRect(keyRect.expanded(2, 2), 2);
    g.fillRect(keyEdge1);
    g.fillRect(keyEdge2);

}

void AmiAudioProcessorEditor::resized()
{
    const float wave_x = 15.f / (float)proportionOfWidth(0.5), 
                wave_y = 15.f / (float)proportionOfHeight(0.5),
                wave_w = 0.75f - (wave_x * 2), wave_h = 0.5f - (wave_y * 2);

    handleGui.setBoundsRelative(0.f, 0.f, 1.f, 1.f);
    
    resetScale.toFront(false);
    resetScale.setBounds(7, 0, 41, 34);
    downScale.setBounds(waveBox.getRight() - 88, 0, 41, 34);
    upScale.setBounds(waveBox.getRight() - 47, 0, 44, 34);
    
    for(int i = 0; i < numWaveforms; i++)
        waveform[i].setBoundsRelative(wave_x, wave_y, wave_w, wave_h);

    waveMenu.setBoundsRelative(0.75f, 0.f, 0.25f, 0.5f);

    clearSampleButton.setBoundsRelative(0.875f, 0.85f, 0.13f, 0.14f);

    loadButton.setBoundsRelative(0.89f, 0.73f, 0.1f, 0.06f);
    saveButton.setBoundsRelative(0.89f, 0.785f, 0.1f, 0.06f);

    keyboardComponent.setBoundsRelative(0.021f, 0.86f, 0.838f, 0.13f);
    logoImage.setBoundsRelative(0.69f, 0.52f, 0.3f, 0.08f);
}

void AmiAudioProcessorEditor::visibilityChanged()
{
    setScaleFactor(audioProcessor.getCurrentScaleFactor());
    lookNFeel.setMouseCursorSize(audioProcessor.getCurrentScaleFactor());
    baseOctave = audioProcessor.getBaseOctave();

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void AmiAudioProcessorEditor::mouseDown(const juce::MouseEvent& e)
{
<<<<<<< HEAD
    amiWindow->mouseDown(e);
=======
    if (waveform[currentSample].getBoundsInParent().contains(e.getPosition())) waveform[currentSample].mouseDown(e);
    if (handleGui.getBoundsInParent().contains(e.getPosition())) handleGui.mouseDown(e);
    if (waveMenu.getBoundsInParent().contains(e.getPosition())) waveMenu.mouseDown(e);

    const int pixelY = waveMenu.getPixelMouseY();
    const int lastWaveform = currentSample;

    if (waveform[currentSample].getScrollBar().getWidth() < 391)
    {

        if (scrollBack.contains(e.getPosition()))
        {
            scrollcolor = AMI_BLK;
            waveform[currentSample].setScrollFactor(waveform[currentSample].getMouseXYRelative().x);
            onScrollBar = true;
            repaint(waveBox);
            return;
        }
    }

    if (!waveMenu.getBounds().contains(e.getPosition())) return;

    for (int i = 0; i < numWaveforms; i++)
    {
        if (pixelY < i * 13) continue;
        if (pixelY > (i * 13) + 10) continue;
        currentSample = i;
    }

    if (currentSample == lastWaveform) return;

    waveform[lastWaveform].setVisible(false);
    waveform[lastWaveform].setInterceptsMouseClicks(false, false);

    audioProcessor.setCurrentSample(currentSample);

    repaint(getBounds().withHeight(proportionOfHeight(0.5f)));

>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void AmiAudioProcessorEditor::mouseDrag(const juce::MouseEvent& e)
{
<<<<<<< HEAD
    amiWindow->mouseDrag(e);
=======
    waveform[currentSample].mouseDrag(e);

    checkLoops();

    if (!onScrollBar) return;

    waveform[currentSample].setScrollFactor(waveform[currentSample].getMouseXYRelative().x);

    repaint(waveBox);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void AmiAudioProcessorEditor::mouseUp(const juce::MouseEvent& e)
{
<<<<<<< HEAD
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
=======
    waveform[currentSample].mouseUp(e);
    handleGui.mouseUp(e);
    
    if (onScrollBar)
    {
        scrollcolor = AMI_WHT;
        onScrollBar = false;
        repaint(waveBox);
    }
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void AmiAudioProcessorEditor::mouseMove(const juce::MouseEvent& e)
{
<<<<<<< HEAD
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
=======
    if (waveBox.contains(e.getPosition()))
        waveform[currentSample].mouseMove(e);
}

void AmiAudioProcessorEditor::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (waveBox.contains(e.getPosition()))
    {
        waveform[currentSample].mouseWheelMove(e, wheel);
        repaint(waveBox);
    } 
}

bool AmiAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (juce::String file : files)
    {
        if (file.contains(".wav") || file.contains(".aif") || file.contains(".aiff") || file.contains(".iff") || file.contains(".brr") ||
            file.contains(".raw") || file.contains(".smp") || file.contains(".bin") || !file.contains("."))
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895

        return true;
    }

    return false;
}

<<<<<<< HEAD
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
=======
void AmiAudioProcessorEditor::filesDropped(const juce::StringArray& files, int, int)     
{
    currentSample = audioProcessor.getCurrentSample();

    for (juce::String file : files)
    {
        if (isInterestedInFileDrag(file))
        {
            auto myFile = std::make_unique<juce::File>(file);

            if(!audioProcessor.loadFile(file)) return;

            waveform[currentSample].resetZoom();
            loadWaves();
            drawWaveMenu();

            juce::String sampName = audioProcessor.getSampleName(currentSample);

            repaint(0, 0, getWidth(), proportionOfHeight(0.5f));
        }
    }
}

void AmiAudioProcessorEditor::handleQuitKeyShortcut(const juce::String keyDesc, const juce::String descToCheck)
{
    if (!juce::JUCEApplicationBase::isStandaloneApp()) return;

    if(keyDesc.compareIgnoreCase(descToCheck) == 0) juce::JUCEApplicationBase::quit();
}

bool AmiAudioProcessorEditor::keyPressed(const juce::KeyPress& k)
{
    const wchar_t key = (wchar_t)toupper(k.getKeyCode());

    keyboardComponent.grabKeyboardFocus();

    if (juce::JUCEApplicationBase::isStandaloneApp())
    {
        if (k.getTextDescription().compareIgnoreCase("alt + F4") == 0)
            juce::JUCEApplicationBase::quit();

        if ((juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) == 0)
        {
            if (k.getTextDescription().compareIgnoreCase("ctrl + O") == 0)
            {
                loadButton.triggerClick();
                return true;
            }

            if (k.getTextDescription().compareIgnoreCase("ctrl + S") == 0)
            {
                saveButton.triggerClick();
                return true;
            }

            if (k.getTextDescription().compareIgnoreCase("ctrl + Q") == 0)
                juce::JUCEApplicationBase::quit();
        }

        if ((juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0)
        {
            if (k.getTextDescription().compareIgnoreCase("command + O") == 0)
            {
                loadButton.triggerClick();
                return true;
            }

            if (k.getTextDescription().compareIgnoreCase("command + S") == 0)
            {
                saveButton.triggerClick();
                return true;
            }

            if (k.getTextDescription().compareIgnoreCase("command + Q") == 0)
                juce::JUCEApplicationBase::quit();
        }
    }

    if (switchWaveforms(k)) return true;

    if (cycleWaveforms(k.getKeyCode())) return true;

    /* ASCII-to-MIDI note map a la Fasttracker II */

    if (k.getKeyCode() <= juce::KeyPress::F8Key && k.getKeyCode() >= juce::KeyPress::F1Key)
    {
        audioProcessor.setBaseOctave(k.getKeyCode() - juce::KeyPress::F1Key + 1);
        baseOctave = audioProcessor.getBaseOctave();
    }

    if (key < ',' || key > ']') return true;

    asciiNote = keyPress2Note[key - ','];

    if (asciiNote == 0)  return true;
        
    keysPressed[key - ','] = 1;
  
    handleExtraNoteOn(asciiNote - 1);

    return true;
}

bool AmiAudioProcessorEditor::keyStateChanged(bool isKeyDown)
{
    if (isKeyDown) return true;

    for (char c = ','; c <= ']'; c++)
    {
        const  char key = c - ',';

        asciiNote = keyPress2Note[key];
        if (!juce::KeyPress::isKeyCurrentlyDown(c) && keysPressed[key])
        {
            handleExtraNoteOff(asciiNote - 1);
            keysPressed[key] = 0;
        }
    }

    return true;
}

void AmiAudioProcessorEditor::handleExtraNoteOn(const int note)
{
    const int midiNote = note + (baseOctave * 12);
    if (midiNote > 127) return;
    if (audioProcessor.getKeyState().isNoteOn(currMidiChannel, midiNote)) return;

    audioProcessor.getKeyState().noteOn(currMidiChannel, midiNote, 1.0f);
}

static bool checkDuplicateNote(const int note, const int noteToCheck, const char key1, const char key2)
{
    if (note != noteToCheck) return false;
    if (juce::KeyPress::isKeyCurrentlyDown(key1)) return true;
    if (juce::KeyPress::isKeyCurrentlyDown(key2)) return true;

    return false;
}

void AmiAudioProcessorEditor::handleExtraNoteOff(const int note)
{
    const int midiNote = note + (baseOctave * 12);
    if (midiNote > 127) return;

    if (checkDuplicateNote(note, 12, ',', 'q')) return;
    if (checkDuplicateNote(note, 13, 'l', '2')) return;
    if (checkDuplicateNote(note, 14, '.', 'w')) return;
    if (checkDuplicateNote(note, 15, ';', '3')) return;
    if (checkDuplicateNote(note, 16, '/', 'e')) return;

    audioProcessor.getKeyState().noteOff(currMidiChannel, (note + ((baseOctave) * 12)), 0.0f);
}

void AmiAudioProcessorEditor::loadWaves()
{
    juce::AudioSampleBuffer sample = audioProcessor.getWaveForm(currentSample);
    const int numSamples = sample.getNumSamples();
    const float* sampBuf = sample.getReadPointer(0);

    waveform[currentSample].setSampLen(numSamples);

    for (int i = 0; i < numSamples; i++)
        waveform[currentSample].setPixelWave((int16_t)floor(sampBuf[i] * INT16_MAX), i);

    waveform[currentSample].setLoopEnable(audioProcessor.getLoopEnable(currentSample));
    waveform[currentSample].setLoopStart(audioProcessor.getLoopStart(currentSample));
    waveform[currentSample].setLoopEnd(audioProcessor.getLoopEnd(currentSample));

    waveform[currentSample].draw_new_wave();
    waveform[currentSample].updateWaveform();
    waveform[currentSample].copyPixelBuffer();
}

void AmiAudioProcessorEditor::drawWaveMenu()
{
    waveMenu.fill_rect(0, 0, 135, 160, AMI_BLK);
    waveMenu.setWidthRatio((float)waveMenu.getWidth() / 135.f);
    int n = 0;
    juce::String name;

    waveMenu.fill_rect(0, (currentSample * 13) + 1, 135, 13, AMI_YLW);

    for (int i = 14; i < 158; i += 13)
    {
        name = juce::String(n + 1).paddedLeft('0', 2) + "." + audioProcessor.getSampleName(n);

        int c = (n == currentSample) ? AMI_BLK : AMI_YLW;

        if(n < 12) waveMenu.draw_Hline(0, i, 135, AMI_GRY);

        waveMenu.print_string(name.toStdString().c_str(), 2, i - 10, c, 1);

        n++;
    }

    waveMenu.draw_rect(0, 0, 135, 2, AMI_GRY - 0x00606060);
    waveMenu.draw_rect(0, 0, 2, 160, AMI_GRY - 0x00606060);

    waveMenu.draw_rect(0, 158, 135, 2, AMI_GRY - 0x00202020);
    waveMenu.draw_rect(133, 0, 2, 160, AMI_GRY - 0x00202020);

    waveMenu.copyPixelBuffer();
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}

void AmiAudioProcessorEditor::timerCallback()
{
<<<<<<< HEAD
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
=======
    const int samplePos = audioProcessor.getSamplePos();

    if (lastPos != samplePos)
    {
        waveform[currentSample].setSamplePos(samplePos);

        const int posLine = waveform[currentSample].getPosLine();

        repaint((int)getLocalPoint(&waveform[currentSample],
            juce::Point<float>((float)lastPosLine, 0.f)).x,
            waveform[currentSample].getY(), 3,
            waveform[currentSample].getWidth());

        repaint((int)getLocalPoint(&waveform[currentSample],
            juce::Point<float>((float)posLine, 0.f)).x,
            waveform[currentSample].getY(), 3,
            waveform[currentSample].getWidth());

        lastPosLine = posLine;
        lastPos = samplePos;
    }
}

void AmiAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if(button == &clearSampleButton)
    {
        audioProcessor.getSampler(currentSample).clearSounds();
        audioProcessor.getWaveForm(currentSample).setSize(1, 0);
        audioProcessor.getWaveForm(currentSample).clear();

        juce::String pathAPTVS = "pathname";
        pathAPTVS.append(juce::String(currentSample), 2);
        audioProcessor.getAPVTS().state.setProperty(pathAPTVS, "", nullptr);

        waveform[currentSample].setSampLen(0);
        waveform[currentSample].allocatePoints(0);
        waveform[currentSample].clearScreen();
        waveform[currentSample].copyPixelBuffer();

        audioProcessor.setLoopEnable(currentSample, 0);
        audioProcessor.setLoopStart(currentSample, 0);
        audioProcessor.setLoopEnd(currentSample, 0);

        waveform[currentSample].setLoopEnable(audioProcessor.getLoopEnable(currentSample));
        waveform[currentSample].setLoopStart(audioProcessor.getLoopStart(currentSample));
        waveform[currentSample].setLoopEnd(audioProcessor.getLoopEnd(currentSample));

        audioProcessor.setSampleName(currentSample, "");
        drawWaveMenu();
        waveMenu.copyPixelBuffer();

        repaint(getBounds().withHeight(proportionOfHeight(0.5f)));
    }

    if (button == &loadButton)
    {
        loadButton.setColour(juce::TextButton::buttonColourId, JPAL(AMI_RED));
        loadButton.setColour(juce::TextButton::textColourOffId, JPAL(AMI_WHT));

        const bool load = audioProcessor.buttonLoadFile();

        loadButton.setColour(juce::TextButton::buttonColourId, JPAL(AMI_GRY));
        loadButton.setColour(juce::TextButton::textColourOffId, JPAL(AMI_BLK));

        repaint(getBounds().withHeight(proportionOfHeight(0.5f)));

        if(!load) return;
        
        waveform[currentSample].resetZoom();
        loadWaves();
        drawWaveMenu();

        juce::String sampName = audioProcessor.getSampleName(currentSample);
    }

    if (button == &saveButton)
    {
        saveButton.setColour(juce::TextButton::buttonColourId, JPAL(AMI_RED));
        saveButton.setColour(juce::TextButton::textColourOffId, JPAL(AMI_WHT));

        if(audioProcessor.getWaveForm(currentSample).getNumSamples() > 0)
            audioProcessor.saveFile(audioProcessor.getSampleName(currentSample));

        saveButton.setColour(juce::TextButton::buttonColourId, JPAL(AMI_GRY));
        saveButton.setColour(juce::TextButton::textColourOffId, JPAL(AMI_BLK));
    }
    
    /* Disables plugin rescaling in Linux as Linux doesn't handle plugin rescaling well*/
    if(juce::JUCEApplicationBase::isStandaloneApp() || !(juce::SystemStats::getOperatingSystemType() & juce::SystemStats::Linux))
    {
        if(button == &resetScale)
        {
            audioProcessor.resetScaleFactor();
            setScaleFactor(audioProcessor.getCurrentScaleFactor());
            lookNFeel.setMouseCursorSize(audioProcessor.getCurrentScaleFactor());
        }
        
        if(button == &downScale)
        {
            audioProcessor.decScaleFactor();
            setScaleFactor(audioProcessor.getCurrentScaleFactor());
            lookNFeel.setMouseCursorSize(audioProcessor.getCurrentScaleFactor());
        }
        
        if(button == &upScale)
        {
            audioProcessor.incScaleFactor();
            setScaleFactor(audioProcessor.getCurrentScaleFactor());
            lookNFeel.setMouseCursorSize(audioProcessor.getCurrentScaleFactor());
        }

    }

}

void AmiAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    const juce::String lpAtch = "LOOP ENABLE" + juce::String(currentSample);
    if (parameterID.containsIgnoreCase(lpAtch))
    {
        waveform[currentSample].setLoopEnable(newValue);
        repaint();
    }
}

bool AmiAudioProcessorEditor::cycleWaveforms(const int key)
{
    int sel_wave = audioProcessor.getCurrentSample();
    currentSample = sel_wave;

    if (key == juce::KeyPress::upKey) sel_wave--;
    else if (key == juce::KeyPress::downKey) sel_wave++;
    else return false;

    if (sel_wave < 0) sel_wave = 0;
    if (sel_wave >= numWaveforms) sel_wave = numWaveforms - 1;

    waveform[currentSample].setVisible(false);
    waveform[currentSample].setInterceptsMouseClicks(false, false);

    currentSample = sel_wave;
    audioProcessor.setCurrentSample(currentSample);

    repaint(getBounds().withHeight(proportionOfHeight(0.5f)));

    return true;
}

bool AmiAudioProcessorEditor::switchWaveforms(const juce::KeyPress& k)
{
    const int num = (k.getKeyCode() & ~'0') < 10 ? k.getKeyCode() & ~'0' :
                     k.getKeyCode() & ~juce::KeyPress::numberPad0;

    if (num < 0 || num > 15) return false;

    if (!juce::KeyPress::isKeyCurrentlyDown(juce::KeyPress::numberPad0 + num)) return false;

    waveform[currentSample].setVisible(false);
    waveform[currentSample].setInterceptsMouseClicks(false, false);

    currentSample = num < 10 ? num > 0 ? num - 1 : 10 - 1 : currentSample;
    audioProcessor.setCurrentSample(currentSample);

    repaint(getBounds().withHeight(proportionOfHeight(0.5f)));

    return true;
}

void AmiAudioProcessorEditor::checkLoops()
{
    int newLoopStart = 0, newLoopEnd = 0;

    if (!audioProcessor.getLoopEnable(currentSample)) return;

    if ((newLoopStart = waveform[currentSample].getNewLoopStart()) != audioProcessor.getLoopStart(currentSample))
    {
        audioProcessor.setLoopStart(currentSample, newLoopStart);
        repaint(waveBox);
    }

    if ((newLoopEnd = waveform[currentSample].getNewLoopEnd()) != audioProcessor.getLoopEnd(currentSample))
    {
        audioProcessor.setLoopEnd(currentSample, newLoopEnd);
        repaint(waveBox);
    }
}

void AmiAudioProcessorEditor::initWaveforms(PixelBuffer* w)
{
    w->setPixelArea(405, 160);
    w->resetZoom();

    if (audioProcessor.getWaveForm(currentSample).getNumSamples() > 0)
        loadWaves();

    addAndMakeVisible(*w);
    w->toFront(false);
    w->setVisible(false);
    w->setEnabled(false);
    w->setInterceptsMouseClicks(false, false);
}

void AmiAudioProcessorEditor::initButton(juce::Button* b)
{
    b->addListener(this);
    addAndMakeVisible(*b);
    b->setColour(juce::TextButton::buttonColourId, JPAL(AMI_GRY));
    b->setColour(juce::TextButton::textColourOffId, JPAL(AMI_BLK));
}

void AmiAudioProcessorEditor::initImgButton(juce::ImageButton* b, const juce::Image up, const juce::Image down)
{
    b->addListener(this);
    addAndMakeVisible(*b);

    b->setImages(true, true, true, up, 1.0f, JPAL(0),
                 up, 1.0, JPAL(0), down, 1.0f, JPAL(0), 0.0f);
>>>>>>> 1374dd9113eb167dae43dbc5650cd7c1ff690895
}
