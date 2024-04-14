/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   AmiLogo_png;
    const int            AmiLogo_pngSize = 36300;

    extern const char*   amidos_ttf;
    const int            amidos_ttfSize = 15824;

    extern const char*   amiMouseCursor_png;
    const int            amiMouseCursor_pngSize = 266;

    extern const char*   amiwin1_1_png;
    const int            amiwin1_1_pngSize = 547;

    extern const char*   amiwin1_2_png;
    const int            amiwin1_2_pngSize = 544;

    extern const char*   amiwin2_1_png;
    const int            amiwin2_1_pngSize = 546;

    extern const char*   amiwin2_2_png;
    const int            amiwin2_2_pngSize = 542;

    extern const char*   amiwin3_1_png;
    const int            amiwin3_1_pngSize = 551;

    extern const char*   amiwin3_2_png;
    const int            amiwin3_2_pngSize = 548;

    extern const char*   pixelkey_black_png;
    const int            pixelkey_black_pngSize = 587;

    extern const char*   amiTrashOff_png;
    const int            amiTrashOff_pngSize = 248;

    extern const char*   amiTrashOn_png;
    const int            amiTrashOn_pngSize = 242;

    extern const char*   astriid_amiga_png;
    const int            astriid_amiga_pngSize = 38845;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 13;

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
