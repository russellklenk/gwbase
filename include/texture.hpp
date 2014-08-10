/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines a thin wrapper around an OpenGL texture object.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_TEXTURE_HPP
#define GW_TEXTURE_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Wraps an OpenGL texture object and provides an interface for
/// loading image data from a file into the texture. Supports 2D textures only.
class Texture
{
public:
    GLuint Id;      /// The OpenGL object ID.
    GLenum Wrap;    /// One of GL_CLAMP_TO_EDGE, GL_REPEAT, etc.
    GLenum Filter;  /// One of GL_NEAREST, GL_LINEAR, etc. for magnification.
    size_t Width;   /// The width of level 0 of the texture, in pixels.
    size_t Height;  /// The height of level 0 of the texture, in pixels.

public:
    Texture(void);
    ~Texture(void);

public:
    /// @summary Creates a texture object and uploads data from a TGA file into it.
    /// @param path The path of the TGA file to load into the texture.
    /// @return true if the texture was loaded.
    bool LoadFromFile(char const *path);

    /// @summary Disposes of the texture object.
    void Dispose(void);
};

#endif /* !defined(GW_TEXTURE_HPP) */
