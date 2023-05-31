# Ami-Sampler
VSTi Sampler inspired by the sound of the famous Commodore Amiga, early development release

I've been obsessed with the sound of the Commodore Amiga, and 90s trackers in general, ever since I discovered 
the demoscene two years ago. I spent hours learning Protracker and Fasttracker and messing around with them in
emulators and in their modernday clones. I went so far as to buy an Amiga 500 and a Pentium II desktop (with a
Soundblaster 16, of course) in order to have the real deal. Hell, I even designed my own 8-bit LPT sampling
module for my Amiga.

So, naturally, the first VST that I set out to make would of course be a VST that allows me to get that same sound
but in the ease of my DAW. I call it: the Ami Sampler.

![Ami_Sampler_FYee3W471g](https://github.com/astriiddev/Ami-Sampler/assets/98296288/d1cfa2bc-cb25-4aa8-b42f-892287b48d68)

This still has a long way to go until it's fully developed, but it's at least at a point in its development that
an alpha release can be usable in a session.

Right now, you can load in .WAV, .AIF, .MP3, and .RAW/.SMP PCM files. .RAW/.SMP files will be read as 8-bit signed 
PCM; therefore, raw PCM files must be formatted as such ( or if you wanna make some REALLY weird sounds, don't
format them as such, I'm not the boss of you ;] ).

Protracker's ASCII note map was one of the most important features that I wanted to implement. The Ami Sampler
has an ASCII-to-MIDI map that is mapped out just as Protracker's is with the added benefit of Fasstracker's
octave-switching range. Notes are mapped out from 'Z' to '/' for the first octave and 'Q' to '\' for the 
second octave ('=' and '\' can be a little buggy as JUCE doesn't seem to like these key presses for some reason).

Octave switching is done with the function keys, mapped out like Fasttracker II with F1 being the lowest octave,
F7 being the highest octave, and F5 being the default octave upon initialization.

In order to really get that Amiga sound, I recommend using 8-bit samples at a sample rate of 16726hz or below
without any type of interpolation. SoX is a great command line program for downsampling and decimating audio
without any added interpolation and 8bitbubsy's Fasttracker II clone is a great GUI program for that as well.

I wouldn't recommend downsampling with Audacity as Audacity adds unwanted interpolation. If your heart is 
really set out on using a program similar to Audacity to downsample your files, Wavosaur is great nine times
outta ten; although I still find that its "non-interpolated" downsampling is still too smooth when compared 
to Fasttracker's or SoX's downsampling or compared to audio recorded on an actual Amiga.

https://github.com/astriiddev/Ami-Sampler/assets/98296288/f3b99aa5-8ebc-4199-a978-0265354fa05a

# TODO

As I said, this has a long way to go and I sure have my work cut out to me but here's what I have in mind for
future releases.

First and foremost, for the next release, the first feature I plan to implement is reading 8SVX-formatted
.IFF files (Amiga audio files). Additionally I plan to implement the Amiga 500's RC filter as well as the
Amiga's LED Butterworth filter. There will be a toggleable button to switch between the Amiga 500 (with RC
filter) and Amiga 1200 (no RC filter) and to turn the LED filter on or off. At the moment the audio is
such as an Amiga 1200 with the LED off.

I also want to include a built in directory to the side of the plugin with the ST-01 and ST-02 sample packs
included for easy loading as well as some of my personal favorite samples from various other ST packs. This
will hopefully be added into the next version but may have to be pushed to the version after.

Additional functions I need to work on or plan to add:
- Panning: will initally be just a standard left and right panning but I would like to add a "step" panning 
  option. This will alternate between Left and Right panning as the sample voices are cycled through, creating
  an emulation of the Amiga's hard left and hard right panning.
- File saving/export: export samples with user's set loop points saved to metadata.
- Sample editing: basic crop, cut, copy, paste, and gain adjustment. Won't be implemented until file saving is.
- Undo/redo: will probably be added in the next update or two, but definitely before sample editing.
- Mono/Poly three-way toggle: will toggle through monophonic voicing, four-voice polyphony (a la Amiga) and
  eight voice polyphony. As of now it's set to eight voice polyphony.
- C-note sample rate adjusting: it's set to the amiga's default 16726 sample rate for Middle C but will add
  increment buttons to change base note as well as fine tuning the sample rate. Would be cool to have an
  auto-pitch detection to do this at file load but not sure on the likelihood of that.
- Multitimbral: right now, only one sample can be used at a time. This means to use several samples, such as
  for drums, one must have several instances of the plugin open. Making the synth multitimbral would allow 
  all 8 voices to have their own separate sample, making it easier to use the Ami Sampler things like drums.
  This will probably take quite a bit of time until I can implement this. If anything, I'll probably initially
  make a lightweight version of this plugin with multi-sample functionality, dedicated to Amiga drum sounds
  and then go back and add said functionality into this synth.
  
If there are any features you would like me to add, feel free to recommend them!

# Resources Used
1) The JUCE framework, I literally couldn't make this plugin without it https://juce.com/
2) The Audio Programmer youtube channel, the initial engine of this plugin was springboarded off of their
   sampler tutorial https://youtube.com/playlist?list=PLLgJJsrdwhPyTmhbSVVa5tPmJXJE-OMYm
3) 8bitbubsy's Protracker and Fasttracker II clones https://github.com/8bitbubsy/pt2-clone https://github.com/8bitbubsy/ft2-clone 
4) WinUAE, to help with designing the graphics without having to boot up my amiga https://www.winuae.net/
5) off1k youtube channel, the best channel for amiga demoscene playlists, an ongoing inspiration to me https://www.youtube.com/@off1k
6) Amiga ST-XX Sample library https://archive.org/details/AmigaSTXX
