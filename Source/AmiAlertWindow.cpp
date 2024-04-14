/*
  ==============================================================================

    AmiAlertWindow.cpp
    Created: 30 Mar 2024 10:58:10am
    Author:  astriid

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AmiAlertWindow.h"
#include "ami_palette.h"

//==============================================================================
AmiAlertWindow::AmiAlertWindow(const int width, const int height, const juce::String &title, const juce::String &message)
{
    setRepaintsOnMouseActivity(false);

    pixelBuffer = std::make_unique<PixelBuffer>();
    setInterceptsMouseClicks(true, true);

    pixelBuffer->setPixelArea(width, height);
    pixelBuffer->setInterceptsMouseClicks(false,false);
    pixelBuffer->setWidthRatio(1.f);

    pixelBuffer->clearScreen();
    pixelBuffer->fill_rect(0, 0, width, height, AMI_GRY);

    pixelBuffer->print_string(title.toStdString().c_str(), (width - (int) title.length() * 16) / 2 + 2, 22, AMI_GRY - 0x00707070, 2);
    pixelBuffer->print_string(title.toStdString().c_str(), (width - (int) title.length() * 16) / 2, 20, AMI_WHT, 2);

    pixelBuffer->fill_rect(4,  4, 2, height, AMI_GRY - 0x00707070);

    pixelBuffer->fill_rect(0, 52, width, 2, AMI_WHT - 0x00202020);
    pixelBuffer->fill_rect(0, 54, width, 2, AMI_GRY - 0x00707070);

    pixelBuffer->fill_rect(0,  4,  width, 2, AMI_GRY - 0x00707070);

    pixelBuffer->fill_rect(0, height - 6, width, 2, AMI_WHT - 0x00202020);
    pixelBuffer->fill_rect(width - 6, 0, 2, height, AMI_WHT - 0x00202020);

    if(message.containsChar('\n'))
    {
        const std::string line_1 = message.upToFirstOccurrenceOf("\n", false,false).toStdString();
        const std::string line_2 = message.fromFirstOccurrenceOf("\n", false,false).toStdString();

        const int text_x1 = (width - (int) line_1.length() * 16) / 2;
        const int text_x2 = (width - (int) line_2.length() * 16) / 2;
        
        const int text_y1 = (height - 34) / 2;
        const int text_y2 = (height) / 2;

        pixelBuffer->print_string(line_1.c_str(), text_x1 + 2, text_y1 + 2, AMI_GRY - 0x00707070, 2);
        pixelBuffer->print_string(line_2.c_str(), text_x2 + 2, text_y2 + 2, AMI_GRY - 0x00707070, 2);

        pixelBuffer->print_string(line_1.c_str(), text_x1, text_y1, AMI_WHT, 2);
        pixelBuffer->print_string(line_2.c_str(), text_x2, text_y2, AMI_WHT, 2);
    }
    else
    {
        const std::string line = message.upToFirstOccurrenceOf("\n", false,false).toStdString();

        const int text_x = (width - (int) line.length() * 16) / 2;
        const int text_y = (height - 16) / 2;

        pixelBuffer->print_string(line.c_str(), text_x + 2, text_y + 2, AMI_GRY - 0x00707070, 2);
        pixelBuffer->print_string(line.c_str(), text_x, text_y, AMI_WHT, 2);
    }

    okRect = Rect_t{(width - 100) / 2 , height - 44, 100, 22, AMI_GRY };

    pixelBuffer->fill_rect(0, 0, width, 4, AMI_WHT - 0x00202020);
    pixelBuffer->fill_rect(0, 0, 4, height, AMI_WHT - 0x00202020);

    pixelBuffer->fill_rect(0, height - 4, width, 4, AMI_GRY - 0x00707070);
    pixelBuffer->fill_rect(width - 4, 0, 4, height,  AMI_GRY - 0x00707070);

    drawOkButton(false);
    pixelBuffer->copyPixelBuffer();

    addAndMakeVisible(pixelBuffer.get());
}

AmiAlertWindow::~AmiAlertWindow() {}

void AmiAlertWindow::paint (juce::Graphics&)
{
    pixelBuffer->setBoundsRelative(0.f, 0.f, 1.f, 1.f);
}

void AmiAlertWindow::resized() {}

void AmiAlertWindow::mouseDown(const juce::MouseEvent &e)
{
    if(!hitbox(&okRect, e.x, e.y)) return;

    drawOkButton(true);
    pixelBuffer->copyPixelBuffer();
    repaint();
}

void AmiAlertWindow::mouseUp(const juce::MouseEvent &e)
{
    if (hitbox(&okRect, e.x, e.y))
    {
        okPressed = true;
        return;
    }

    drawOkButton(false);
    pixelBuffer->copyPixelBuffer();

    repaint();
}

bool AmiAlertWindow::hitbox(const Rect_t *b, const int x, const int y)
{
    if(x < b->x) return false;
    if(x > b->x + b->w) return false;
    if(y < b->y) return false;
    if(y > b->y + b->h) return false;
    
    return true;
}

void AmiAlertWindow::drawButton(const bool clicked, const Rect_t *b, const std::string txt)
{
    pixelBuffer->fill_rect(b->x - 1, b->y - 1, b->w + 2, b->h + 2, b->c);
    pixelBuffer->draw_rect(b->x - 1, b->y - 1, b->w + 2, b->h + 2, AMI_BLK);

    if(!clicked)
    {
        pixelBuffer->fill_rect(b->x, b->y, b->w, 2, AMI_WHT - 0x00202020);
        pixelBuffer->fill_rect(b->x, b->y, 2, b->h, AMI_WHT - 0x00202020);
        pixelBuffer->fill_rect(b->x, b->y + b->h - 2, b->w, 2, AMI_GRY - 0x00707070);
        pixelBuffer->fill_rect(b->x + b->w - 2, b->y, 2, b->h,  AMI_GRY - 0x00707070);

        pixelBuffer->print_string(txt.c_str(), b->x + (b->w - 16 * (int) txt.length()) / 2 + 1, b->y + (b->h - 16) / 2 + 1, AMI_BLK, 2);
    }
    else
    {
        pixelBuffer->fill_rect(b->x, b->y, b->w, 2, AMI_GRY - 0x00707070);
        pixelBuffer->fill_rect(b->x, b->y, 2, b->h, AMI_GRY - 0x00707070);

        pixelBuffer->print_string(txt.c_str(), b->x + (b->w - 16 * (int) txt.length()) / 2 + 2, b->y + (b->h - 16) / 2 + 2, AMI_BLK, 2);
    }
}
