/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the interface to a loader for 24/32-bpp uncompressed TGA.
/// It is assumed that both the host system and input data are little-endian.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef FF_TGA_HPP
#define FF_TGA_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Specifies the size of the tga_file_header_t structure, in bytes.
#define TGA_HEADER_SIZE               18U

/// @summary Specifies the size of the tga_file_footer_t structure, in bytes.
#define TGA_FOOTER_SIZE               26U

/// @summary Defines the color map types supported by the image format. This
/// library only supports loading of TGA_COLORMAPTYPE_NONE images.
enum tga_colormaptype_e
{
    TGA_COLORMAPTYPE_NONE             = 0,
    TGA_COLORMAPTYPE_INCLUDED         = 1,
    TGA_COLORMAPTYPE_FORCE_32BIT      = 0x7FFFFFFFL,
};

/// @summary Defines the image types supported by the image format. This library
/// only supports loading of TGA_IMAGETYPE_UNCOMPRESSED_TRUE images.
enum tga_imagetype_e
{
    TGA_IMAGETYPE_NO_IMAGE_DATA       = 0,
    TGA_IMAGETYPE_UNCOMPRESSED_PAL    = 1,
    TGA_IMAGETYPE_UNCOMPRESSED_TRUE   = 2,
    TGA_IMAGETYPE_UNCOMPRESSED_BW     = 3,
    TGA_IMAGETYPE_RLE_PAL             = 9,
    TGA_IMAGETYPE_RLE_TRUE            = 10,
    TGA_IMAGETYPE_RLE_BW              = 11,
    TGA_IMAGETYPE_FORCE_32BIT         = 0x7FFFFFFFL,
};

#pragma pack (push, 1)
/// @summary Describes the TGA file header. Review the TGA file format
/// specification for details on the individual fields.
struct tga_file_header_t
{
    uint8_t  ImageIdLength;
    uint8_t  ColormapType;
    uint8_t  ImageType;
    uint16_t ColormapFirstEntryIndex;
    uint16_t ColormapLength;
    uint8_t  ColormapEntrySize;
    uint16_t ImageXOrigin;
    uint16_t ImageYOrigin;
    uint16_t ImageWidth;
    uint16_t ImageHeight;
    uint8_t  ImageBitDepth;
    uint8_t  ImageFlags;
};

/// @summary Describes the TGA file footer. Review the TGA file format
/// specification for details on the individual fields.
struct tga_file_footer_t
{
    uint32_t ExtensionAreaOffset;
    uint32_t DeveloperDirectoryOffset;
    char     Signature[16];
    char     PeriodChar;
    char     ZeroByte;
};
#pragma pack (pop)

/*///////////////
//  Functions  //
///////////////*/
/// @summary Inspect the contents of a buffer representing a well-formed TGA
/// image and report various attributes about the image.
/// @param data The buffer containing the TGA image.
/// @param data_size The size of the TGA image data, in bytes.
/// @param out_width On return, this location is updated with the width of the image, in pixels.
/// @param out_height On return, this location is updated with the height of the image, in pixels.
/// @param out_required_size On return, this location is updated with the number of bytes required
/// to store the image data when converted to 32-bpp as returned by tga_pixels().
/// @param out_header On return, this structure is populated with the file header.
/// @param out_footer On return, this structure is populated with the file footer.
/// @return true if the TGA is a supported format.
bool tga_describe(
    void const        *data,
    size_t             data_size,
    size_t            *out_width,
    size_t            *out_height,
    size_t            *out_required_size,
    tga_file_header_t *out_header,
    tga_file_footer_t *out_footer);

/// @summary Extracts the 32-bpp RGBA pixel data from a TGA file. If necessary,
/// the data is converted to 32-bpp RGBA from its source format.
/// @param rgba32 The output buffer. 32-bpp RGBA pixel data is written here.
/// @param data The buffer containing the TGA image.
/// @param data_size The size of the TGA image data, in bytes.
/// @return true if the TGA is a supported format.
bool tga_pixels(void *rgba32, void const *data, size_t data_size);

#endif /* !defined(FF_TGA_HPP) */
