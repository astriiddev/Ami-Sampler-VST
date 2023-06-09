# Ami-Sampler
VSTi Sampler inspired by the sound of the famous Commodore Amiga, early development release

I've been obsessed with the sound of the Commodore Amiga, and 90s trackers in general, ever since I discovered 
the demoscene two years ago. I spent hours learning Protracker and Fasttracker and messing around with them in
emulators and in their modernday clones. I went so far as to buy an Amiga 500 and a Pentium II desktop (with a
Soundblaster 16, of course) in order to have the real deal. Hell, I even designed my own 8-bit LPT sampling
module for my Amiga.

So, naturally, the first VST that I set out to make would of course be a VST that allows me to get that same sound
but in the ease of my DAW. I call it: the Ami Sampler.

![amiGUI](https://github.com/astriiddev/Ami-Sampler/assets/98296288/654c9643-c2f1-4f22-8597-c7908c504f19)

This still has a long way to go until it's fully developed, but it's at least at a point in its development that
an alpha release can be usable in a session.

Right now, you can load and save .WAV, .AIF, and .RAW/.SMP PCM files. .RAW/.SMP files will be read as 8-bit signed 
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

# New For v0.5

The first thing I made sure to implement in this new version were the filters. The A1200/A500 switch implements
the first of the filters with the A500 mode enabling a 4.4khz 6db/oct low pass filter and the A1200 bypassing 
said filter. The LED button activates the LED Butterworth filter, a 12db/oct low pass filter at 3khz. To 
paraphrase the Fasttracker 2.08 manual, turn on the A500 and LED to make your sample sound terrible ;)

Both of these filters were rewritten from 8bitbubsy's Protracker 2 clone to work in JUCE/C++
https://github.com/8bitbubsy/pt2-clone/blob/master/src/pt2_rcfilters.c

I also added in the panning and saving controls. Saving can export WAV, AIF, and RAW/SMP files. Saving the sample
as RAW or SMP will save your sample as raw 8-bit signed PCM, as will saving your file without an extension. Exporting
as WAV will also export the loop point settings, if loop points are enabled. Unfortunately I couldn't get AIF to
also export the loop point metadata, though I also couldn't get the AIF metadata to save loop points even with SoX
and Audacity so I'm unsure if it's an issue with the actual format or not. RAW/SMP will not save loop points as these
are just headerless PCM data.

Additionally, there is now a three way switch for one monophonic and two polyphonic modes. "MONO" is simply one voice
monophonic. "PT POLY" is a four voice polyphonic mode to simulate the four channels of Protracker. MEDPOLY is an
eight voice polyphonic mode for simulating the eight channels of OctaMED.

My most interesting addition, in my opinion, is the "PAULA STEREO" button. This is an "note auto-panning" effect;
when enabled, the first note played will be panned left, second note played will be panned right, and then it 
continues to switch back and forth. Rather than autopanning the summed output of the synth, the panned note will remain
panned until the note is released or the voice is replaced. This is to not only emulate the hard-left and hard-right 
panning of the Paula sound chip on the Amiga, but to also emulate the ability in trackers to pan individual notes of 
chords left and right, creating a really unique stereo image.

I'm not aware of this sort of effect being used on any other VSTi but it was an effect I missed when jumping between
using trackers and using a DAW, so it was very important that I add something to simulate it. I really recommend it
for slow moving pads and for ambient piano/plucked string sounds.

Smaller additons/changes include loop point flags to make the loop points easier to see/move around, a loop length 
("REPLEN") text box, minor graphic changes, and an improved ASCII note map.

# TODO

As I said, this has a long way to go and I sure have my work cut out to me but here's what I have in mind for
future releases.

~~First and foremost, for the next release, the first feature I plan to implement is reading 8SVX-formatted .IFF files 
(Amiga audio files). Additionally I plan to implement the Amiga 500's RC filter as well as the Amiga's LED Butterworth 
filter. There will be a toggleable button to switch between the Amiga 500 (with RC filter) and Amiga 1200 (no RC filter) 
and to turn the LED filter on or off. At the moment the audio is such as an Amiga 1200 with the LED off.~~

Unfortunately, I did not get around to adding support for 8SVX-formatted .IFF audio files. I did start on writing
the parser for them, however, so I do hope to add support for them in the next release or two.

My next release will focus on sample importing and exporting. The file browser currently has no selectable
formats in the dialog, making the user have to type out the format for their file; so fixing this will be my
first order of business. I would also like to have the sample's loaded sample name automatically filled into
the file browser's text box when saving.

The larger side of the next update will be a built-in directory with a library of Amiga samples. This will be a 
mixture of the ST-01/ST-02 samples, random samples that I find useful or enjoyable from the remaining ST-XX 
folders, and samples that I have recorded off of my own amiga. External loading/sample drop will still be available,
but a built in directory will make sample loading a lot quicker for users. 

Additional functions I need to work on or plan to add:
- ~~Panning: will initally be just a standard left and right panning but I would like to add a "step" panning 
  option. This will alternate between Left and Right panning as the sample voices are cycled through, creating
  an emulation of the Amiga's hard left and hard right panning.~~
- Stereo width control maybe?
- ~~File saving/export: export samples with user's set loop points saved to metadata.~~
- Sample editing: basic crop, cut, copy, paste, and gain adjustment. ~~Won't be implemented until file saving is.~~
- Undo/redo: will probably be added in the next update or two, but definitely before sample editing.
- ~~Mono/Poly three-way toggle: will toggle through monophonic voicing, four-voice polyphony (a la Amiga) and
  eight voice polyphony. As of now it's set to eight voice polyphony.~~
- C-note sample rate adjusting: it's set to the amiga's default 16726 sample rate for Middle C but will add
  increment buttons to change base note as well as fine tuning the sample rate. Would be cool to have an
  auto-pitch detection to do this at file load but not sure on the likelihood of that.
- Resampling: convert higher quality files to have an 8-bit bit depth and a sample rate of 16726hz using nearest
  neighbor resampling.
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
