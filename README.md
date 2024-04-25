<pre>                               
                
                                               ______     ____      ____     ____
                                              /      |_  |    \ _  /    |_  |    |_
                                             /       | | |     \ \/     | | |    | |
                                            /        | | |      \/      | | |    | |
                                           /   /|    | | |              | | |    | |
                                          /   /_|    | | |    |\  /|    | | |    | |
                                         /    __     | | |    | \/ |    | | |    | |
                                        /    / _|    | | |    | | \|    | | |    | |
                                       /    / / |    | | |    | |  |    | | |    | |
                                      /____/ /  |____| | |____| |  |____| | |____| |
                                       /____/     |____|   |____|    |____|   |____|
        
                ___________       ______     ____      ____     _________      ____            _________     _________
               /           /_    /      |_  |    \ _  /    |_  |     _   \    |    |_         |         |_  |     _   \__
              /     ______/ /   /       | | |     \ \/     | | |    | \   \   |    | |        |     ____| | |    | \   \ \
             /     /__ ____/   /        | | |      \/      | | |    |  |   |  |    | |        |    |  ____| |    |  |   | \
             \         \      /   /|    | | |              | | |    |_/   / | |    | |        |    |_|      |    |_/   /   |
              \____     \    /   /_|    | | |    |\  /|    | | |     ____/ /  |    | |        |     __|_    |        _/   /
                  /     /\  /    __     | | |    | \/ |    | | |    |  ___/   |    | |        |    |  __|   |        \  _/
          _______/     / / /    / _|    | | |    | | \|    | | |    | |       |    |_|____    |    |_|__    |    |\   \ \
         /            / / /    / / |    | | |    | |  |    | | |    | |       |           |_  |         |_  |    | \   \ \
        /____________/ / /____/ /  |____| | |____| |  |____| | |____| |       |___________| | |_________| | |____|  \___\ \
          /___________/   /____/     |____|   |____|    |____|   |____|         |___________|   |_________|   |____|   \___\
  
</pre>

![image](https://github.com/astriiddev/Ami-Sampler-VST/assets/98296288/04bf0afe-4dd8-4c98-94b7-9626dd64a960)

# Ami Sampler
The Commodore Amiga revolutionized the home computer when it hit the shelves in 1985. Not only sporting colourful 16-bit graphics but also boasting four channels of 8-bit sampled audio. With sample sequencing software like ProTracker and OctaMED, it was the first computer that allowed its users to record and sequence instruments into a full song; inspiring musicians, programmers, graphic designers, and hackers to show what they could really do with the new technology that the Amiga gave them. And while its specs may seem like they pale in comparison to more modern computers, its sound and legacy continue to inspire people to this day.

The Ami Sampler brings that feel and that sound into the modern world of Digital Audio Workstations. Obviously heavily inspired by the Amiga, as well as Protracker and Fasttracker, it also draws inspiration from last trackers like Triton's Fasttracker II and Milkytracker in addition to some modern sampler VST plugins. While in no way an emulator nor a recreation of the limitations that the late 80s software had, the Ami Sampler strives to take the classic sound of the Amiga but still allow it to stand up with a modern DAW setting.

https://github.com/astriiddev/Ami-Sampler-VST/assets/98296288/fe12af70-d8be-4d9a-93b7-d94a14513dc8

https://github.com/astriiddev/Ami-Sampler-VST/assets/98296288/cca21d16-3dfb-4881-8c29-4aa84a049bbf

<a href="https://www.paypal.com/donate/?hosted_button_id=HXGUCB6MBN43Y">
  <img src="https://raw.githubusercontent.com/stefan-niedermann/paypal-donate-button/master/paypal-donate-button.png" alt="Donate with PayPal" />
</a>

# Controls / Parameters
For the most part, the Ami Sampler has pretty standard controls that one would expect to find in a sampler VST. There are twelve individual samplers. Each sampler has its own volume, pan, and ADSR controls as well as solo/mute. There's also the option for each channel to be monophonic, four-voice polyphonic (PT poly), or eight-voice polyphonic (Octa poly). There's also a sample rate decimator control and when the channel is in monophonic mode, there's a control for pitch glide. Master output has controls for master volume, master pan, a switch to enable a 4.4khz 6db/octave low pass filter (modeled after the RC filter in the Amiga 500), a switch to enable a 3.1khz 12db/octave low pass filter (modeled after the LED filter in the Amiga 500 and Amiga 1200), and vibrato speed and depth.

By far my favorite control that I put into the Ami Sampler is the "Paula" switch, taking inspiration from the Amiga's audio processor's (known as Paula) limitation to only be able to pan hard left or hard right. This limitation in the Amiga could allow the user to create uniquely wide sounding pads when multiple channels play the same sample as a chord. The Paula switch creates a similar affect by alternating the pan for each  note played by the sample; almost like an autopan except the notes are sustained instead of shifting back and forth. When in "Paula" mode, the channel pan control becomes a stereo width from -100 wide to +100 wide (when in Mono mode, "Paula" is disabled since the effect is for chords).  

Other controls include a computer-key virtual midi keyboard mapped to Protracker's/Fasttracke's note map, plugin window scaling via the window buttons above the waveform graphic (window scaling is disable in Linux plugins as Linux DAWs don't handle this feature well), sample looping, and MIDI channel/transposition controls.

# Sample loading/saving
Sample loading can be done via the LOAD button or by dragging and dropping samples into Ami Sampler. Supported file formats include: .WAV, .AIF, 8SVX .IFF (Amiga audio file), 8-bit signed raw PCM, mu-law encoded .BIN (Linn drum and Drumulator samples), and .BRR files (Super Nintendo samples). While Ami Sampler does play back any audio at 8-bit PCM and does have a sample rate decimator available, to fully get the Amiga feel, it's recommended to use 8-bit samples with a sample rate below 28khz (typical sample rates for the Amiga were usually either 8363hz or 16726hz). 

A handleful of samples are included in this repository under the Samples folder. Included are the famous ST-01 and ST-02 sample packs converted to WAV, a random collection of some of my favorite samples in the remaining ST-XX sample packs (under ST-FV), and a handful of samples that I recorded on my Amiga 500 using my Arduinosound Turbo sampler module (https://github.com/astriiddev/Arduinosound-Turbo) under the folder ST-ASTRX. Some of the ST-ASTRX samples are in the original IFF format, some have been converted to BRR using my SBC700 Super BRR Converter program (still in development).

Additional Amiga samples from the ST-XX sample packs can be found at https://archive.org/details/AmigaSTXX

Sample saving is done with the SAVE button; currently only WAV, AIF, IFF, raw, and BIN are supported. You can also clear out a sample from its slot with the trashcan icon.

# Things to add, bugs to fix
The two most features that I still need to add are sample presets (hence the samples included externally in the repo) and BLEP synthesis for a more accurate recreation of the Amiga's sound. These are both a bit far out and may take some time to implement.

The main bug that I'm currently aware of is the mouse cursor not changing to the Amiga cursor sometimes. This seems to only be present in Ableton when the playhead is moving.
(Previous versions had a bug with Auto-Scale PLugin Window in Ableton. This has since been fixed in the current JUCE version)

Ami Sampler has been tested and working on the following.

**Windows 11:**
- As a standalone app
- As a VST3 in Ableton
- As a VST3 in Studio One
- As a VST3 in Tracktion Waveform

**MacOS Sonoma (Apple Silicon):**
- As a standalone app
- As a VST3 in Ableton
- As an AU in Ableton
- As an AU in Garageband

**Linux Ubuntu 20.04**
- As a standalone app
- As a VST3 in Ardour 8
- As an LV2 in Ardour 8
- As an LV2 in MusE

# Special thanks to

 - Olav Sørensen (8bitbubsy)
 - The folks over at WinUAE
 - The folks over at Cloanto/Amiga Forever
 - And, of course, to Commodore themselves
