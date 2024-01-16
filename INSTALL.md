<pre>

             ╓────────────────────────────╖
             ║                            ║
             ║   INSTALLING AMI SAMPLER   ║
             ║  Finley Baker (_astriid_)  ║
             ║                            ║
             ╙────────────────────────────╜

These instructions are for installing Ami Sampler from the
already-made application and VST files. For instructions 
on building from the source, see BUILD.

═══════════════════╕
    ON WINDOWS:    │
═══════════════════╛

1. Navigate to the Releases page on the Ami Sampler github
   repository at https://github.com/astriiddev/Ami-Sampler/releases
2. Download AmiSampler_win_x86_64.7z and extract the contents
3. Move ami.vst3 to your VST3 folder; typically 
   C:\Program Files\Common Files\VST3
4. Move ami.exe to the folder of your choice

═══════════════════╕
    ON MACOS:      │
═══════════════════╛

1. Navigate to the Releases page on the Ami Sampler github
   repository at https://github.com/astriiddev/Ami-Sampler/releases

For automatic installation:

2. Download AmiSampler_macOS.pkg and navigate to the folder where it
   downloaded to.
3. Control + Click on the file and click Open
4. Ignore Mac's warning (it shows that whenever a developer hasn't had
   their program signed by Apple) and click open.
5. Follow the instructions in the installer.

For manual installation:

2. Download AmiSampler_macOS.7z and extract the contents.
3. Move Ami Sampler.vst3 to /Library/Audio/Plug-Ins/VST3
4. Move Ami Sampler.component to /Library/Audio/Plug-Ins/Component
5. Move Ami Sampler.app to /Applications

═══════════════════╕
    ON LINUX:      │
═══════════════════╛

1. Navigate to the Releases page on the Ami Sampler github
   repository at https://github.com/astriiddev/Ami-Sampler/releases
2. Download AmiSampler_linux.7z and extract its contents

For automatic installation:

3. run sudo chmod +x install.sh
4. run ./install.sh
5. type in root user password when prompted 

For manual installation:

3. move Ami Sampler.vst3 to ~/.vst3
4. move Ami Sampler.lv2 to ~/.lv2
5. move ami to /usr/local/bin
6. move ami.png to ~/.local/share/icons
7. move ami.desktop to ~/.local/share/applications

═══════════════════════════════════════════════════════════════════

Additonally, there are four directories of samples available
for optional download. In addition to the famous ST-01 and
ST-02 sample packs, I included a handful of my favorite
samples from all the other ST-XX sample packs, found in the
ST-FV directory. All ST-XX samples have been converted to WAV.
Finally, I included a handful of samples recorded on my own
Amiga 500 with my Arduinosound Turbo sampler module (available
at  https://github.com/astriiddev/Arduinosound-Turbo for 
building). There are found in ST-ASTRX; some kept in their
original IFF format, some coverted to BRR with my SBC700
Super BRR Converter program (still in development). WAV,
IFF, and BRR are all supported by Ami Sampler.

To download the complete ST-XX sample packs in their original
format, go to:
https://archive.org/details/AmigaSTXX

Special thanks to:

Olav Sørensen (8bitbubsy)
The folks over at WinUAE
The folks over at Cloanto/Amiga Forever
And, of course, to Commodore themselves
  
</pre>
