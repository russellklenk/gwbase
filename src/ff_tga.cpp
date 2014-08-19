/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a loader for 24/32-bpp uncompressed TGA. It is assumed
/// that both the host system and input data are little-endian.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <stdlib.h>
#include "ff_tga.hpp"

/*//////////////////////
//   Implementation   //
//////////////////////*/
bool tga_describe(
    void const        *data,
    size_t             data_size,
    size_t            *out_width,
    size_t            *out_height,
    size_t            *out_required_size,
    tga_file_header_t *out_header,
    tga_file_footer_t *out_footer)
{
    tga_file_header_t  header;
    tga_file_footer_t  footer;
    uint8_t const     *base_ptr   = (uint8_t const*) data;
    uint8_t const     *header_ptr = (uint8_t const*) data;
    uint8_t const     *footer_ptr = (base_ptr + data_size)    - sizeof(tga_file_footer_t);
    size_t             min_size   = sizeof(tga_file_header_t) + sizeof(tga_file_footer_t);

    if (data == NULL || data_size < min_size)
        goto tga_error;

    header = *(tga_file_header_t const*) header_ptr;
    footer = *(tga_file_footer_t const*) footer_ptr;
    if (header.ImageType != TGA_IMAGETYPE_UNCOMPRESSED_TRUE)
        goto tga_unsupported;
    if (header.ImageBitDepth != 24 && header.ImageBitDepth != 32)
        goto tga_unsupported;

    if (out_width)  *out_width  = header.ImageWidth;
    if (out_height) *out_height = header.ImageHeight;
    if (out_required_size) *out_required_size = header.ImageWidth * header.ImageHeight * 4;
    if (out_header) *out_header = header;
    if (out_footer) *out_footer = footer;
    return true;

tga_unsupported:
    if (out_width)  *out_width  = header.ImageWidth;
    if (out_height) *out_height = header.ImageHeight;
    if (out_required_size) *out_required_size = header.ImageWidth * header.ImageHeight * 4;
    if (out_header) *out_header = header;
    if (out_footer) *out_footer = footer;
    return false;

tga_error:
    if (out_width)  *out_width  = 0;
    if (out_height) *out_height = 0;
    if (out_required_size) *out_required_size = 0;
    return false;
}

bool tga_pixels(void *rgba32, void const *data, size_t data_size)
{
    tga_file_header_t header;
    uint8_t const    *base_ptr = (uint8_t const*) data;
    size_t            width    =  0;
    size_t            height   =  0;

    if (tga_describe(data, data_size, &width, &height, NULL, &header, NULL))
    {
        size_t pixel_offset = sizeof(tga_file_header_t) + header.ImageIdLength +
            (header.ColormapLength * (header.ColormapEntrySize / 8));

        uint8_t const *src  = base_ptr + pixel_offset;
        uint8_t       *dst  = (uint8_t*) rgba32;
        size_t         srcb = 0;

        if (header.ImageBitDepth == 32)
        {
            for (size_t row = 0; row < height; ++row)
            {
                for (size_t col = 0; col < width; ++col)
                {
                    *dst++ = src[srcb + 2];
                    *dst++ = src[srcb + 1];
                    *dst++ = src[srcb + 0];
                    *dst++ = src[srcb + 3];
                    srcb  += 4;
                }
            }
        }
        else // 24bpp
        {
            for (size_t row = 0; row < height; ++row)
            {
                for (size_t col = 0; col < width; ++col)
                {
                    *dst++ = src[srcb + 2];
                    *dst++ = src[srcb + 1];
                    *dst++ = src[srcb + 0];
                    *dst++ = 0xFF;
                    srcb  += 3;
                }
            }
        }
        return true;
    }
    else return false;
}
