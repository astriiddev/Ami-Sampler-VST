/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   AmiMED_Icon_png;
    const int            AmiMED_Icon_pngSize = 60556;

    extern const char*   amiTrashOff_png;
    const int            amiTrashOff_pngSize = 248;

    extern const char*   amiTrashOn_png;
    const int            amiTrashOn_pngSize = 242;

    extern const char*   amiBoxBacking_png;
    const int            amiBoxBacking_pngSize = 946;

    extern const char*   astriid_amiga_png;
    const int            astriid_amiga_pngSize = 38845;

    extern const char*   amiSaveOff_png;
    const int            amiSaveOff_pngSize = 2195;

    extern const char*   amiSaveOn_png;
    const int            amiSaveOn_pngSize = 2280;

    extern const char*   amiMouseCursor_png;
    const int            amiMouseCursor_pngSize = 266;

    extern const char*   amiLoadOff_png;
    const int            amiLoadOff_pngSize = 2110;

    extern const char*   amiLoadOn_png;
    const int            amiLoadOn_pngSize = 2151;

    extern const char*   amiLoopOff_png;
    const int            amiLoopOff_pngSize = 1956;

    extern const char*   amiLoopOn_png;
    const int            amiLoopOn_pngSize = 2187;

    extern const char*   amiClearOff_png;
    const int            amiClearOff_pngSize = 2394;

    extern const char*   amiClearOn_png;
    const int            amiClearOn_pngSize = 2480;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 14;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
