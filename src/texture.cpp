/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a thin wrapper around an OpenGL texture object.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <stdio.h>
#include "texture.hpp"
#include "glimage.hpp"
#include "imtga.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
Texture::Texture(void)
    :
    Id(0),
    Wrap(GL_CLAMP_TO_EDGE),
    Filter(GL_NEAREST),
    Width(0),
    Height(0)
{
    /* empty */
}

Texture::~Texture(void)
{
    Dispose();
}

bool Texture::LoadFromFile(char const *path)
{
    FILE *fp = fopen(path, "rb");
    if   (fp)
    {
        // determine the file size, in bytes.
        size_t file_size = 0;
        fseek(fp, 0, SEEK_END);
        file_size = (size_t) ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // allocate a buffer for the TGA file data, read it, and close the file.
        uint8_t  *tga = new uint8_t[file_size];
        if (fread(tga, 1, file_size, fp) != file_size)
        {
            delete[] tga;
            fclose(fp);
            return false;
        }
        fclose(fp);

        // extract the pixel data from the TGA and upload it to a new texture.
        size_t tga_w = 0;
        size_t tga_h = 0;
        size_t pix_n = 0;
        if (tga_describe(tga, file_size, &tga_w, &tga_h, &pix_n, NULL, NULL))
        {
            // extract the 32-bpp RGBA data from the file.
            uint8_t   *pix = new uint8_t[pix_n];
            tga_pixels(pix, tga, file_size);
            delete[]   tga;

            // generate and configure the texture object.
            GLuint id = 0;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            texture_storage(GL_TEXTURE_2D, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, Filter, tga_w, tga_h, 1, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap);

            // transfer the pixel data to the device.
            pixel_transfer_h2d_t px;
            px.Target          = GL_TEXTURE_2D;
            px.Format          = GL_RGBA;
            px.DataType        = GL_UNSIGNED_BYTE;
            px.UnpackBuffer    = 0;
            px.TargetIndex     = 0;
            px.TargetX         = 0;
            px.TargetY         = 0;
            px.TargetZ         = 0;
            px.SourceX         = 0;
            px.SourceY         = 0;
            px.SourceZ         = 0;
            px.SourceWidth     = tga_w;
            px.SourceHeight    = tga_h;
            px.TransferWidth   = tga_w;
            px.TransferHeight  = tga_h;
            px.TransferSlices  = 0;
            px.TransferSize    = pix_n;
            px.TransferBuffer  = pix;
            transfer_pixels_h2d(&px);

            delete[] pix;
            Id     = id;
            Width  = tga_w;
            Height = tga_h;
            return true;
        }
        else
        {
            // not a valid TGA file.
            delete[] tga;
            return false;
        }
    }
    else return false;
}

void Texture::Dispose(void)
{
    if (Id != 0)
    {
        glDeleteTextures(1, &Id);
        Id = 0;
    }
}
