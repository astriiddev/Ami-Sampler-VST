

          ╓────────────────────────────╖
          ║                            ║
          ║    BUILDING AMI SAMPLER    ║
          ║  Finley Baker (_astriid_)  ║
          ║                            ║
          ╙────────────────────────────╜

These instructions are for building Ami Sampler from the source
files. For instructions on installing the already-made application 
and VST files, see INSTALL.

═══════════════════╕
    DEPENDENCIES:  │
═══════════════════╛

1. Download or build JUCE framework
2. Download juce_patch.zip from AmiSampler repo
3. Install build tools for your platform
   - For Windows: Visual Studio 2022
   - For MacOS: Xcode 15
   - For Linux: GCC/G++ (sudo apt install build-essential 
                            or sudo pacman -Sy base-devel)
4. [OPTIONAL] Download the Samples directory and its
   subdirectories

═══════════════════╕
   PATCHING JUCE:  │
═══════════════════╛

    The Ami Sampler makes a few changes to the JUCE 
source code and therefore requires a handful of files for
patching JUCE to build it. One patch fixes a bug in Windows
for detecting the key state for the \ and = keys which is
needed for having the computer-key to MIDI note map function
properly. The other patch adds support for IFF/raw, BRR, and
BIN files. This can be patched manually or with either the
.bat or .ps1 scripts

To manually patch:

1. Unzip the juce_patch.zip file.
2. Move the following files within the juce_patch directory
   into the respective directories.

in (JUCE_DIR)\modules\juce_gui_basics\native :
     juce_patch\juce_Windowing_windows.cpp

in (JUCE_DIR)\modules\juce_audio_formats :

     juce_patch\astro_fmt\juce_audio_formats.cpp
     juce_patch\astro_fmt\juce_audio_formats.h

in (JUCE_DIR)\modules\juce_audio_formats\format :

    juce_patch\astro_fmt\format\juce_AudioFormatManager.cpp
    juce_patch\astro_fmt\format\juce_AudioFormatManager.h

in (JUCE_DIR)\modules\juce_audio_formats\codecs :

    juce_patch\astro_fmt\codecs\astro_BrrAudioFormat.cpp
    juce_patch\astro_fmt\codecs\astro_BrrAudioFormat.h
    juce_patch\astro_fmt\codecs\astro_IffAudioFormat.cpp
    juce_patch\astro_fmt\codecs\astro_IffAudioFormat.h
    juce_patch\astro_fmt\codecs\astro_MuLawFormat.cpp
    juce_patch\astro_fmt\codecs\astro_MuLawFormat.h

where (JUCE_DIR) is the location of your JUCE directory.

To patch with a script:

   On any platform with Powershell support, run:
      juce_patch\astro_fmt.ps1 [JUCE_DIR]

   Windows typically requires extra permissions for .ps1 
   files, so instead run:
      juce_patch\astro_fmt.bat [JUCE_DIR]

where [JUCE_DIR\ is the location of your JUCE directory.


═══════════════════╕
    BUILDING AMI:  │
═══════════════════╛

Once the above has been done, open the ami.jucer file with 
JUCE's Projucer. Select the exporter you'll be using (Visual
Studio, Xcode, or LinuxMakefile); it is okay to delete the
other two that you won't be using. Save the ami.jucer file 
to generate the necessary files for your selected exporter.

On Windows:

   Click the Visual Studio logo at the top of the Projucer 
   window to open the Visual Studio project. Select "Release" 
   for your build and build the project. Once complete, 
   navigate to the Release subdirectory in your ami directory. 
   Move the VST file (typically found at 
       VST3/ami.vst3/Contents/win_x86_64/ami.vst3) 
   into your computer's VST3 folder (typically found in 
       C:\Program Files\Common Files\VST3). 
   Move the .exe found at Releases\Standalone\ami.exe to the 
   directory of your choice.

On MacOS:

   OPTIONAL: select AU as a type while in the Projucer window
   and save the ami.jucer file.

   Click the Xcode logo at the top of the Projucer window to 
   open  the Xcode project. Under the Project drop menu, in 
   Schemes, select Ami Sampler - All. Select Edit Scheme and 
   change the build to Release. Build the project. The VST3 
   should automatically save in /Library/Audio/Plug-Ins/VST3 
   and the AU should automatically save in 
   /Library/Audio/Plug-Ins/Component. If not, they can be 
   found  in Builds/MacOS/Release/build from the project 
   directory and then moved to their respective directories. 
   Feel free to move Ami Sampler.app to /Applications/ or a
   directory of your choosing

On Linux:

   OPTIONAL: select LV2 as a type while in the Projucer 
   window and save the ami.jucer file.

   In the terminal, navigate to Builds/LinuxMakeFile in the
   project directory and type 
          make CONFIG=Release 
   into the command line. The VST3 and LV2 files should 
   automatically save in ~/.vst3 and ~/.lv2 respectively. 
   If not found there, navigate to Builds/LinuxMakefile/build
   and use "cp -rf" for "Ami Sampler.vst3" and 
   "Ami Sampler.lv2" to move them into the respective 
   directories. You can also use the command
          sudo cp 'Ami Sampler' /usr/bin/ami
   to move the standalone app to your applications directory
   for faster access (typing just "ami" into the command line
   will then open the standalone app.

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
