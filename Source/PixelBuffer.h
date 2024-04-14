/*
  ==============================================================================

    PixelBuffer.h
    Created: 16 Dec 2023 5:59:02pm
    Author:  _astriid_

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
typedef struct Rect_t
{
    int x,
        y,
        w,
        h;

    uint32_t c;
} Rect_t;

class PixelBuffer  : public juce::Component
{
public:
    PixelBuffer();
    ~PixelBuffer() override;

    void paint (juce::Graphics&) override;
    void visibilityChanged() override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    
    void setPixelArea(const int w, const int h);
    bool allocatePoints(const int len);
    void clearScreen();

    int draw_new_wave();
    void updateWaveform();
    void copyPixelBuffer();

    void setPixelWave(const int16_t val, const int i);
    void setSampLen(const int len);
    void setScrollFactor(const int mouse_x);

    void resetZoom();

    void draw_line(const int x1, const int y1, const int x2, const int y2, const uint32_t c);
    void draw_Hline(const int x, const int y, const int w, const uint32_t c);
    void draw_Vline(const int x, const int y, const int h, const uint32_t c);

    void draw_rect(const int x, const int y, const int w, const int h, const uint32_t c);
    void fill_rect(const int x, const int y, const int w, const int h, const uint32_t c);

    void print_font(char text, int x, int y, uint32_t color, int size);
    void print_string(const char* text, int x, int y, uint32_t c, int size);

    inline void setSamplePos(const int pos) { sample_pos = pos > 0 ? pos < samp_len ? pos : 0 : 0; }
    inline int& getPosLine() { return posLine; }

    inline int& getPixelWidth() { return PIXEL_WIDTH; }
    inline int& getPixelHeight() { return PIXEL_HEIGHT; }
    inline int& getPointX(const int i) { return point_x.getReference(i); }

    inline void setLoopEnable(const bool enable) { loopEnable = enable; }
    inline void setLoopStart(const int pos) { loopStart = pos; }
    inline void setLoopEnd(const int pos) { loopEnd = pos; }
    
    inline int& getNewLoopStart() { return loopStart; }
    inline int& getNewLoopEnd() { return loopEnd; }

    inline int& getPixelMouseX() { return pixel_x; }
    inline int& getPixelMouseY() { return pixel_y; }

    inline float& getWidthRatio() { return widthRatio; }
    inline void setWidthRatio(const float ratio) { widthRatio = ratio; }

    inline juce::Rectangle<int>& getScrollBar() { return scroll_bar; }
 
private:
    void setPointX(const int index, const int x);
    void setPointY(const int index, const int y);

    int scr2samp(const int64_t x) const;
    int samp2scr(const int64_t x) const;
    int vmap(const int x) const;

    void handleZoom(const bool zoom_in);
    void handleScroll(const bool scroll_left);
    void updateZoom(void);

    void drawWaveLines();
    void drawWavePolygons();
    bool loopHitBox(const int loopPosX, const int x, const int y, bool startPos) const;

    juce::Image screenBuffer = { juce::Image::PixelFormat::ARGB, 1, 1, false };

    uint32_t* pixel_pointer = nullptr;
    juce::Array<uint32_t> pixel_buffer;
    juce::Array <int16_t> pixelWave;
    juce::Array<int> point_x, point_y;

    int64_t samp2wave_scale = 0, wave2samp_scale = 0;
    int PIXEL_WIDTH = 0, PIXEL_HEIGHT = 0, PIXEL_AREA = 0;

    int pixel_x = 0, pixel_y = 0, last_x = 0, posLine = 0, wave_width = 0;
    int	line_start = 0, line_end = 0, mouse_focus = 0, cursor = 0,
        scroll_factor = 0, scroll_divider = 1, wave_adjust = 0;
    
    double zoom_divider = 0.0;
    uint32_t zoom_timer = 0;

    int samp_len = 0, sample_pos = 0, 
        loopStart = 0, loopEnd = 0,
        loopStartX = 0, loopEndX = 0;

    bool loopEnable = false;

    float widthRatio = 0;

    bool loopStartEdit = false, loopEndEdit = false;

    juce::Rectangle<int> scroll_bar{ 20, 159, 398, 11 };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PixelBuffer)
};
