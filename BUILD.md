<pre>

          ╓────────────────────────────╖
          ║                            ║
          ║    BUILDING AMI SAMPLER    ║
          ║  Finley Baker (_astriid_)  ║
          ║                            ║
          ╙────────────────────────────╜

These instructions are for building Ami Sampler from the 
source files. For instructions on installing the already 
made application and VST files, see INSTALL.

Version 0.6 required a juce_patch in order to build.
This is no longer needed as JUCE has fixed the issues
in juce_Windows_windowing.cpp and I've rewritten the
extra formats to no longer require a patch. The extra
formats are now located in Source/astro_formats. If you
built v0.6, it is recommended that you remove the patch
and build with the current JUCE source.

═══════════════════╕
    DEPENDENCIES:  │
═══════════════════╛

1. Download or build JUCE framework
2. Install build tools for your platform
   - For Windows: Visual Studio 2022
   - For MacOS: Xcode 15
   - For Linux: GCC/G++ (sudo apt install build-essential 
                            or sudo pacman -Sy base-devel)
3. [OPTIONAL] Download the Samples directory and its
   subdirectories

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
       VST3\ami.vst3\Contents\x86_64-win\ami.vst3) 
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

   OPTIONAL 1: select LV2 as a type while in the Projucer 
   window and save the ami.jucer file.

   OPTIONAL 2: the waveform in Ami Sampler can be scrolled
   left-to-right with a laptop trackpad when the waveform is
   zoomed in. However, JUCE in Linux does not have the
   necessary code to allow for this. Please see my write-up at:
          https://forum.juce.com/t/no-horizontal-scrolling-in-x11/60584
   to add this in.

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
