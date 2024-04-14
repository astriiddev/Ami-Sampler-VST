/*
  ==============================================================================

    PixelBuffecpp
    Created: 16 Dec 2023 5:59:02pm
    Author:  _astriid_

  ==============================================================================
*/

#include <JuceHeader.h>

#include "ami_palette.h"
#include "ami_font.h"
#include "PixelBuffer.h"

constexpr int64_t ROUND32 = 0x80000000;

#define SETPIX(x, y, c)  *(pixel_pointer + (x) + (y) * PIXEL_WIDTH) = c

#define ABS(a)	 ((a) >  0 ? (a) : -(a))
#define SGN(a,b) ((a) < (b) ? 1 : -1)

//==============================================================================
PixelBuffer::PixelBuffer()
{
	setInterceptsMouseClicks(false, false);
	setRepaintsOnMouseActivity(false);
	setWantsKeyboardFocus(false);

	zoom_timer = juce::Time::getMillisecondCounter();
}

PixelBuffer::~PixelBuffer() {}

void PixelBuffer::paint (juce::Graphics& g)
{
	if (pixel_buffer.isEmpty()) return;
	if (screenBuffer.getWidth() <= 1) return;

	if(isShowing() && widthRatio <= 1) 
		widthRatio = (float)getWidth() / PIXEL_WIDTH;

	g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
	g.drawImageWithin(screenBuffer, 0, 0, getWidth(), getHeight(),
		juce::RectanglePlacement::stretchToFit, false);

	if (samp_len <= 0) { cursor = 0; return; }

	if (point_x.isEmpty()) { cursor = 0; return; }
	if (cursor > samp_len || cursor < 0) cursor = 0;

	if (isShowing() && point_x.operator[](samp_len) < PIXEL_WIDTH / 2)
	{
		updateWaveform();
		copyPixelBuffer();
		repaint();
		return;
	}

	g.setColour(JPAL(AMI_WHT));
	if (sample_pos > 0 && sample_pos < samp_len)
	{
		g.fillRect(posLine, 0, 2, getHeight());
	}

	const int samp_cursor = (cursor > 0) ? cursor <= samp_len  ? (int)(point_x.operator[](cursor) * widthRatio) : 0 : 0;
	g.fillRect(samp_cursor, 0, 1, getHeight());

	posLine = (int) (point_x.operator[](sample_pos) * widthRatio);

	if (loopEnable)
	{
		g.setColour(JPAL(AMI_ORG));
		
		if(loopStart >= line_start && loopStart <= line_end)
		{
			loopStartX = (int)(point_x.operator[](loopStart) * widthRatio);
			g.fillRect(loopStartX, 0, 2, getHeight());
			g.fillRect(loopStartX, 3, 13, 13);
		}

		if(loopEnd >= line_start && loopEnd <= line_end)
		{
			loopEndX = (int)(point_x.operator[](loopEnd) * widthRatio);
			g.fillRect(loopEndX, 0, 2, getHeight());
			g.fillRect(loopEndX - 13, getHeight() - 16, 13, 13);
		}
	}	
}

void PixelBuffer::visibilityChanged()
{
	if (!isVisible()) return;

	widthRatio = (float)getWidth() / PIXEL_WIDTH;

	updateWaveform();
	copyPixelBuffer();
	repaint();
}

void PixelBuffer::resized()
{
}

void PixelBuffer::mouseDown(const juce::MouseEvent&)
{
	const int mouse_x = getMouseXYRelative().x,
			  mouse_y = getMouseXYRelative().y;

	pixel_x = (int)(mouse_x / widthRatio);
	pixel_y = (int)(mouse_y / widthRatio);

	cursor = mouse_focus = scr2samp(pixel_x);

	if (loopHitBox(loopStartX, mouse_x, mouse_y, true)) loopStartEdit = true;
	if (loopHitBox(loopEndX, mouse_x, mouse_y, false)) loopEndEdit = true;

	repaint();
}

void PixelBuffer::mouseUp(const juce::MouseEvent&)
{
	if (loopStartEdit) 
	{
		mouse_focus = loopStart;
		loopStartEdit = false;
	}
	if (loopEndEdit) 
	{
		mouse_focus = loopEnd;
		loopEndEdit = false;
	}
}

void PixelBuffer::mouseDrag(const juce::MouseEvent&)
{
	const int mouse_x = getMouseXYRelative().x;
	
	if (loopStartEdit)
	{
		pixel_x = (int)(mouse_x / widthRatio);
		loopStart = scr2samp(pixel_x);

		if (loopStart <= 0) loopStart = 0;
		if (loopStart >= loopEnd) loopStart = loopEnd - 1;
	}
	else if (loopEndEdit)
	{
		pixel_x = (int)(mouse_x / widthRatio);
		loopEnd = scr2samp(pixel_x);

		if (loopEnd <= loopStart) loopEnd = loopStart + 1;
		if (loopEnd >= samp_len - 1) loopEnd = samp_len - 1;
	}
}

void PixelBuffer::mouseMove(const juce::MouseEvent&)
{
	if (samp_len < 0) return;
	int mouse_x = getMouseXYRelative().x;

	if (mouse_x < last_x + 1 && mouse_x > last_x - 1) return;
	
	mouse_focus = scr2samp((int64_t)((float) mouse_x / widthRatio));
	last_x = mouse_x;
}

void PixelBuffer::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
	const bool shiftMod = juce::ModifierKeys::currentModifiers.isShiftDown();

	if (samp_len < 0) return;

	if (juce::Time::getMillisecondCounter() - zoom_timer < 85) return;

	if (wheel.deltaY > 0 && !shiftMod) handleZoom(true);
	else if (wheel.deltaY < 0 && !shiftMod) handleZoom(false);

	else if (wheel.deltaX > 0 || (wheel.deltaY > 0 && shiftMod))
	{
		handleScroll(true);
		mouse_focus = scr2samp((int64_t) std::floor((float) getMouseXYRelative().x / widthRatio));
	}
	else if (wheel.deltaX < 0 || (wheel.deltaY < 0 && shiftMod))
	{
		handleScroll(false);
		mouse_focus = scr2samp((int64_t) std::floor((float) getMouseXYRelative().x / widthRatio));
	}

	updateWaveform();
	copyPixelBuffer();
}

void PixelBuffer::setPixelArea(const int w, const int h)
{
	if (!pixel_buffer.isEmpty()) pixel_buffer.clear();

    PIXEL_WIDTH  = w;
    PIXEL_HEIGHT = h;
	PIXEL_AREA = PIXEL_WIDTH * PIXEL_HEIGHT;

    pixel_buffer.resize(PIXEL_AREA);

	pixel_pointer = pixel_buffer.getRawDataPointer();
	screenBuffer = { juce::Image::PixelFormat::ARGB, PIXEL_WIDTH, PIXEL_HEIGHT, false };

	clearScreen();
}

void PixelBuffer::copyPixelBuffer()
{
    juce::Image::BitmapData pixelMap(screenBuffer, juce::Image::BitmapData::readWrite);

	for (int y = 0; y < pixelMap.height; y++)
		memcpy(pixelMap.getLinePointer(y), pixel_pointer + (y * pixelMap.width), (size_t) pixelMap.lineStride);
}

void PixelBuffer::clearScreen()
{
	juce::zeromem(pixel_pointer, (size_t) PIXEL_AREA * sizeof *pixel_pointer);
}

void PixelBuffer::draw_line(const int x1, const int y1, const int x2, const int y2, uint32_t c)
{
	int x, y, dx, dy, sx, sy, error, e2;

	if (pixel_buffer.isEmpty()) return;

    x = x1;
    y = y1;

	dx = ABS(x2 - x1);
	sx = SGN(x1, x2);
	dy = ABS(y2 - y1);
	sy = SGN(y1, y2);

	error = dx - dy;

	while (1)
	{
		if (x == x2 && y == y2) break;
		e2 = error << 1;
		if (e2 >= -dy)
		{
			if (x == x2) break;
			error -= dy;
			x += sx;
		}
		if (e2 <= dx)
		{
			if (y == y2) break;
			error += dx;
			y += sy;
		}

		if (x < 0) continue;
		if (y < 0) continue;
		if (x >= PIXEL_WIDTH)  break;
		if (y >= PIXEL_HEIGHT) break;

		SETPIX(x, y, c);
	}
}

void PixelBuffer::draw_Hline(const int x, const int y, const int w, const uint32_t c)
{
	if (pixel_buffer.isEmpty()) return;
	if (y < 0 || y >= PIXEL_HEIGHT) return;
	int draw_x = x, draw_w = x + w;

	if (w < 0)
	{
		draw_x = draw_w;
		draw_w = draw_x - w;
	}

	if (draw_x < 0) { draw_x = 0; }

	if (draw_w >= PIXEL_WIDTH) draw_w = PIXEL_WIDTH - 1;

	if (draw_x >= PIXEL_WIDTH) return;

	for (int i = draw_x; i < draw_w; i++)
		SETPIX(i, y, c);
}

void PixelBuffer::draw_Vline(const int x, const int y, const int h, const uint32_t c)
{
	if (pixel_buffer.isEmpty()) return;
	if (x < 0 || x >= PIXEL_WIDTH || y >= PIXEL_HEIGHT) return;

	int draw_y = y, draw_h = y + h;

	if (h < 0)
	{
		draw_y = draw_h;
		draw_h = draw_y - h;
	}

	if (draw_y < 0) { draw_y = 0; }

	if (draw_h >= PIXEL_HEIGHT) draw_h = PIXEL_HEIGHT - 1;

	if (draw_y >= PIXEL_HEIGHT) return;

	for (int i = draw_y; i < draw_h; i++)
		SETPIX(x, i, c);
}

void PixelBuffer::draw_rect(const int x, const int y, const int w, const int h, const uint32_t c)
{
	int draw_x = x, draw_w = x + w,
		draw_y = y, draw_h = y + h;

	if (pixel_buffer.isEmpty()) return;

	if (w < 0)
	{
		draw_x = draw_w;
		draw_w = draw_x - w;
	}

	if (h < 0)
	{
		draw_y = draw_h;
		draw_h = draw_y - h;
	}

	draw_w--;
	draw_h--;

	if (draw_x < 0) { draw_x = 0; }
	if (draw_y < 0) { draw_y = 0; }

	if (draw_w >= PIXEL_WIDTH) draw_w = PIXEL_WIDTH - 1;
	if (draw_h >= PIXEL_HEIGHT) draw_h = PIXEL_HEIGHT - 1;

	if (draw_x >= PIXEL_WIDTH) return;
	if (draw_y >= PIXEL_HEIGHT) return;

	for (int i = draw_x; i <= draw_w; i++)
	{
		SETPIX(i, draw_y, c);
		SETPIX(i, draw_h, c);
	}

	for (int n = draw_y; n <= draw_h; n++)
	{
		SETPIX(draw_x, n, c);
		SETPIX(draw_w, n, c);
	}
}

void PixelBuffer::fill_rect(const int x, const int y, const int w, const int h, const uint32_t c)
{
	int draw_x = x, draw_w = x + w,
		draw_y = y, draw_h = y + h;

	if (pixel_buffer.isEmpty()) return;

	if (w < 0)
	{
		draw_x = draw_w;
		draw_w = draw_x - w;
	}

	if (h < 0)
	{
		draw_y = draw_h;
		draw_h = draw_y - h;
	}

	if (draw_x < 0) { draw_x = 0; }
	if (draw_y < 0) { draw_y = 0; }

	if (draw_w > PIXEL_WIDTH) draw_w = PIXEL_WIDTH;
	if (draw_h > PIXEL_HEIGHT) draw_h = PIXEL_HEIGHT;

	if (draw_x > PIXEL_WIDTH) return;
	if (draw_y > PIXEL_HEIGHT) return;

	for (int n = draw_y; n < draw_h; n++)
	{
		for (int i = draw_x; i < draw_w; i++)
			SETPIX(i, n, c);
	}
}

void PixelBuffer::print_font(char text, int x, int y, uint32_t color, int size)
{
	int i, n, t, draw_x, draw_y;
	int snh = size << 3;

	if (text < '!' || text > '~') return;

	t = text - '!';

	for (n = 0; n < snh; n++)
	{
		int shift = 7;
		int l = n / size;

		for (i = 0; i < snh; i++)
		{
			draw_x = x + i;
			draw_y = y + n;

			if (draw_x >= PIXEL_WIDTH)
				continue;

			if (draw_y >= PIXEL_HEIGHT)
				break;

			if (getAmiFont(t, l) & (1 << shift))
				SETPIX(draw_x, draw_y, color);

			if ((i + 1) % size == 0)
				shift--;
		}
	}
}

void PixelBuffer::print_string(const char* text, int x, int y, uint32_t c, int size)
{
	int text_x = x, text_y = y;
	const int spacing = size << 3;
	char letter = *text++;

	while (letter != '\0')
	{
		if (letter == '\n')
		{
			text_x = x;
			text_y += spacing;
			letter = *text++;
		}

		if (text_x >= PIXEL_WIDTH - 7) break;

		if (letter >= '!' && letter <= '~')
			print_font(letter, text_x, text_y, c, size);

		text_x += spacing;

		letter = *text++;
	}
}

void PixelBuffer::resetZoom()
{
	scroll_factor = 0;
	zoom_divider = 0;

	line_start = 0;
	line_end = samp_len;
}

bool PixelBuffer::allocatePoints(const int len)
{
	bool success = true;

	if (!point_x.isEmpty()) point_x.clear();

	if (!point_y.isEmpty()) point_y.clear();

	if (len == 0) return (success = true);

	point_x.resize(len);
	point_y.resize(len);

	if (point_x.isEmpty()) success = false;
	if (point_y.isEmpty()) success = false;

	point_x.fill(0);
	point_y.fill(PIXEL_HEIGHT / 2);

	return success;
}

void PixelBuffer::setPointX(const int index, const int x)
{
	if (!point_x.isEmpty())
		point_x.set(index, x);
}

void PixelBuffer::setPointY(const int index, const int y)
{
	if (!point_y.isEmpty())
		point_y.set(index, y < PIXEL_HEIGHT ? y : PIXEL_HEIGHT - 1);
}

int PixelBuffer::vmap(const int x) const { return (PIXEL_HEIGHT / 2) - ((x * PIXEL_HEIGHT) >> 16); }

int PixelBuffer::scr2samp(const int64_t x) const { return (int)(line_start + ((ROUND32 + (x * samp2wave_scale)) >> 32)); }
int PixelBuffer::samp2scr(const int64_t x) const { return (int)((ROUND32 + ((x - line_start) * wave2samp_scale)) >> 32); }

void PixelBuffer::setPixelWave(const int16_t val, const int i) 
{ 
	if (i > samp_len) return;

	pixelWave.set(i, val); 
}

void PixelBuffer::setSampLen(const int len)
{
	samp_len = len;

	if (!pixelWave.isEmpty()) pixelWave.clear();

	if (samp_len == 0)
	{
		cursor = mouse_focus = line_start = line_end = 0;
		scroll_bar.setX(20);
		scroll_bar.setWidth(398);
		return;
	}

	pixelWave.resize(samp_len);

	samp2wave_scale = ((int64_t)samp_len << 32) / PIXEL_WIDTH;
	wave2samp_scale = ((int64_t)PIXEL_WIDTH << 32) / samp_len;

	line_start = 0;
	line_end = wave_width = samp_len;

	mouse_focus = samp_len / 2;
}

int PixelBuffer::draw_new_wave()
{
	if (!pixelWave.isEmpty())
	{
		allocatePoints(samp_len + 1);

		for (int i = 0; i < samp_len; i++)
			setPointY(i, vmap(pixelWave.operator[](i)));

		setPointY(samp_len, point_y.operator[](samp_len - 1));
	}
	else
		return 0;

	return 1;
}

void PixelBuffer::handleZoom(const bool zoom_in)
{
	if (samp_len <= 1) return;

	if (zoom_in)
	{
		if(wave_width <= 2) return; 

		zoom_divider += (0.5 - zoom_divider) / 2;
	}
	else
	{
		if(wave_width >= samp_len || zoom_divider < 0.0) 
		{
			zoom_divider = scroll_factor = 0;
			return;
		}

		zoom_divider -= (0.5 - zoom_divider) / 2;
		mouse_focus = (line_end + line_start) >> 1;
	}

	if (mouse_focus > 0) scroll_factor = mouse_focus - (samp_len >> 1);
}

void PixelBuffer::setScrollFactor(const int mouse_x)
{
	const int64_t x = (int64_t)(mouse_x / widthRatio);
	const int scroll_length = (int)((x * ((int64_t)samp_len << 32) / (int64_t)PIXEL_WIDTH + ROUND32) >> 32);
	scroll_factor = scroll_length - (samp_len / 2);

	updateWaveform();
	copyPixelBuffer();
}

void PixelBuffer::handleScroll(const bool scroll_left)
{
	if (samp_len < 0) return;

	if (zoom_divider <= 0) return;

	if (scroll_left)
	{
		if (line_end <= line_start + 10) scroll_factor--;
		else scroll_factor -= wave_width / 8;

		if (scroll_factor <= -(samp_len / 2)) scroll_factor = -(samp_len / 2) + 1;
	}
	else
	{
		if (line_end <= line_start + 10) scroll_factor++;
		else scroll_factor += wave_width / 8;

		if (scroll_factor >= samp_len / 2) scroll_factor = samp_len / 2 - 1;
	}

	mouse_focus = (line_end + line_start) / 2;
}

void PixelBuffer::updateZoom(void)
{
	int scroll_x = 0, scroll_w = 0;
	if (point_x.isEmpty()) return;

	wave_adjust = (int)round((double)samp_len * zoom_divider);

	if (line_end < 0) line_end = samp_len;

	if (mouse_focus == 0)
		mouse_focus = (line_end + line_start) / 2;

	if (scroll_factor < -wave_adjust)
		scroll_factor = -wave_adjust;

	if (scroll_factor > wave_adjust)
		scroll_factor = wave_adjust;

	line_start = wave_adjust + scroll_factor;

	line_end = (samp_len - wave_adjust + scroll_factor);

	if (line_end > samp_len) line_end = samp_len;

	if (line_start < 0) line_start = 0;

	if (line_end <= line_start)
	{
		line_end = line_start + 1;
		line_start -= 1;
	}

	wave_width = line_end - line_start;

	samp2wave_scale = ((int64_t) wave_width << 32) / PIXEL_WIDTH;
	wave2samp_scale = ((int64_t)PIXEL_WIDTH << 32) / wave_width;

	for (int i = 0; i < samp_len; i++)
		setPointX(i, samp2scr(i));

	if (line_end >= samp_len) setPointX(samp_len, PIXEL_WIDTH - 1);

	scroll_w = (int)((PIXEL_WIDTH - 7) * (int)wave_width / samp_len);

	if (scroll_w <= 7)
	{
		scroll_w = 7;
		scroll_x = (int)((-point_x.operator[](0) * (int)wave_width / samp_len) +
						(20 - ((14 * line_start) / (samp_len - 7))));
	}
	else
	{
		scroll_x = (int)((-point_x.operator[](0) * (int)wave_width / samp_len) +
						(20 - ((14 * line_start) / samp_len)));
	}

	if (line_start <= 0) scroll_x = 20;

	if (line_end == samp_len) scroll_x = PIXEL_WIDTH - scroll_w + 13;

	scroll_bar.setX(scroll_x);
	scroll_bar.setWidth(scroll_w);
}

bool PixelBuffer::loopHitBox(const int loopPosX, const int x, const int y, bool startPos) const
{
	const int flagX = startPos ? loopPosX + 10 : loopPosX - 10,
			  flagY = startPos ? 13 : (getHeight() - 16);

	if (!loopEnable) return false;

	if (x < loopPosX - 3)
	{
		if (startPos) return false;
		else if (x < flagX || y < flagY) return false;
	}

	if (x > loopPosX + 3)
	{
		if (!startPos) return false;
		else if (x > flagX || y > flagY) return false;
	}

	return true;
}


void PixelBuffer::drawWaveLines()
{
    for (int i = 0; i < PIXEL_WIDTH; i++)
    {
        int currSampPos = scr2samp((int64_t)(i + 0));
        int nextSampPos = scr2samp((int64_t)(i + 1));
		int x1, y1, x2, y2;

        if (currSampPos >= samp_len) currSampPos = samp_len - 1;
        if (nextSampPos >= samp_len) nextSampPos = samp_len;

		x1 = point_x.operator[](currSampPos);
		y1 = point_y.operator[](currSampPos);
		x2 = point_x.operator[](nextSampPos);
		y2 = point_y.operator[](nextSampPos);

        draw_line(x1, y1, x2, y2, AMI_WHT);
    }
}

void PixelBuffer::drawWavePolygons()
{
    int last_ymin = point_y.getFirst(), last_ymax = point_y.getFirst();

    for (int i = 0; i < PIXEL_WIDTH; i++)
    {
		int ymin = 0, ymax = 0;
		int samp_min = INT16_MAX, samp_max = INT16_MIN;

        int currSampPos = scr2samp((int64_t)(i + 0));
        int nextSampPos = scr2samp((int64_t)(i + 1));
		
        if (currSampPos >= samp_len) currSampPos = samp_len - 1;
        if (nextSampPos >= samp_len) nextSampPos = samp_len - 1;

        for (int n = currSampPos; n <= nextSampPos; n++)
        {
            const int currSampVal = pixelWave.operator[](n);

            if (currSampVal < samp_min)
            {
                samp_min = currSampVal;
                ymax = point_y.operator[](n);
            }

            if (currSampVal > samp_max)
            {
                samp_max = currSampVal;
                ymin = point_y.operator[](n);
            }
        }

        if (i > 0)
        {
            if (ymin >= last_ymax) draw_line(i - 1, last_ymax, i, ymin, AMI_WHT);
            if (ymax <= last_ymin) draw_line(i - 1, last_ymin, i, ymax, AMI_WHT);
        }

        draw_line(i, ymin, i, ymax, AMI_WHT);

        last_ymin = ymin;
        last_ymax = ymax;
    }
}

void PixelBuffer::updateWaveform()
{
	if (samp_len <= 0) return;
	if (point_x.isEmpty() || point_y.isEmpty()) return;

	if (juce::Time::getMillisecondCounter() - zoom_timer < 100) return;

	clearScreen();

	updateZoom();

	if (wave_width <= PIXEL_WIDTH)
        drawWaveLines();
	else
        drawWavePolygons();

	zoom_timer = juce::Time::getMillisecondCounter();
}
