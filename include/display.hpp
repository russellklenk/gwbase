/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_DISPLAY_HPP
#define GW_DISPLAY_HPP

/*////////////////
//   Includes   //
////////////////*/
#include <map>
#include <vector>
#include "common.hpp"
#include "platform.hpp"
#include "glshader.hpp"
#include "glimage.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary A simple structure representing a rectangle.
struct rect_t
{
    float X;
    float Y;
    float Width;
    float Height;
};

/// @summary A bucket of vertex and index data for a batch of sprites.
/// All sprites in the bucket share the same texture.
struct sprite_bucket_t
{
    std::vector<float>    Vertices;  /// Each vertex is 2 floats.
    std::vector<float>    TexCoords; /// Each texcoord is 2 floats.
    std::vector<float>    Colors;    /// Each color is 4 floats.
    std::vector<uint16_t> Indices;   /// Each sprite is 6 indices.
};

/// @summary Wraps an OpenGL texture object and provides an interface for
/// loading image data from a file into the texture. Supports 2D textures only.
class Texture
{
protected:
    GLuint Id;      /// The OpenGL object ID.
    GLenum Wrap;    /// One of GL_CLAMP_TO_EDGE, GL_REPEAT, etc.
    GLenum Filter;  /// One of GL_NEAREST, GL_LINEAR, etc. for magnification.
    size_t Width;   /// The width of level 0 of the texture, in pixels.
    size_t Height;  /// The height of level 0 of the texture, in pixels.

public:
    Texture(void);
    virtual ~Texture(void);

public:
    GLuint GetId(void) const { return Id; }
    GLenum GetWrapMode(void) const { return Wrap; }
    GLenum GetMagnifyFilter(void) const { return Filter; }
    size_t GetWidth(void) const { return Width; }
    size_t GetHeight(void) const { return Height; }

public:
    void SetWrapMode(GLenum mode) { Wrap = mode; }
    void SetMagnifyFilter(GLenum filter) { Filter = filter; }

public:
    /// @summary Creates a texture object and uploads data from a TGA file into it.
    /// @param path The path of the TGA file to load into the texture.
    /// @return true if the texture was loaded.
    bool LoadFromFile(char const *path);

public:
    /// @summary Disposes of the texture object.
    virtual void Dispose(void);
};

/// @summary
class SpriteBatch
{
protected:
    typedef std::map<std::pair<uint8_t,Texture*>,sprite_bucket_t> bucket_map_t;

protected:
    bucket_map_t      Buckets;
    attribute_desc_t *AttribPos;      /// Vertex position attribute
    attribute_desc_t *AttribTex;      /// Vertex texcoord attribute
    attribute_desc_t *AttribClr;      /// Vertex color attribute
    uniform_desc_t   *UniformMSS;     /// Matrix Screen Space -> Clip Space
    sampler_desc_t   *SamplerTex;     /// Diffuse texture sampler
    shader_desc_t     ShaderDesc;
    GLuint            ShaderProgram;
    GLuint            VAO;
    GLuint            VBOPos;
    GLuint            VBOTex;
    GLuint            VBOClr;
    size_t            BufferOffset;
    size_t            BufferCapacity;
    float             Matrix[16];

protected:
    void FourPoints(std::vector<float> &v, rect_t const &r);
    void SixIndices(std::vector<uint16_t> &v, uint16_t start);

public:
    SpriteBatch(void);
    virtual ~SpriteBatch(void);

public:
    bool CreateGPUResources(size_t capacity);
    void DeleteGPUResources(void);
    void SetProjection(float const *m16);
    void Draw(uint8_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba);
    void Draw(uint8_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba, float rot, float ox, float oy);
    void Draw(uint8_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba);
    void Draw(uint8_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba, float rot, float ox, float oy, float sx, float sy);
    void Flush(void);
};

/// @summary
class DisplayManager
{
public:
    GLFWwindow *main_window;

public:
    DisplayManager(void);
    ~DisplayManager(void);

public:
    bool init(GLFWwindow *win);
    void shutdown(void);

private:
    DisplayManager(DisplayManager const &other);
    DisplayManager& operator =(DisplayManager const &other);
};

#endif /* !defined(GW_DISPLAY_HPP) */
