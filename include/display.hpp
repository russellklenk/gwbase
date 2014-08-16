/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_DISPLAY_HPP
#define GW_DISPLAY_HPP

/*////////////////
//   Includes   //
////////////////*/
#include <string>
#include <vector>
#include "common.hpp"
#include "platform.hpp"
#include "glshader.hpp" /// low-level shader interface
#include "glsprite.hpp" /// low-level sprite rendering
#include "glimage.hpp"  /// low-level pixel transfers

/*////////////////
//  Data Types  //
////////////////*/
struct rect_t
{
    float X;
    float Y;
    float Width;
    float Height;
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
    /// @summary Retrieves the OpenGL texture object identifier.
    /// @return The OpenGL texture object ID.
    GLuint GetId(void) const { return Id; }

    /// @summary Retrieves the texture coordinate wrapping mode.
    /// @return The OpenGL texture wrapping mode for all axes.
    GLenum GetWrapMode(void) const { return Wrap; }

    /// @summary Retrieves the texture magnification filter.
    /// @return The OpenGL texture magnification filter.
    GLenum GetMagnifyFilter(void) const { return Filter; }

    /// @summary Retrieves the width of the texture, in pixels.
    /// @return The width of the texture object, in pixels.
    size_t GetWidth(void) const { return Width; }

    /// @summary Retrieves the height of the texture, in pixels.
    /// @return The height of the texture object, in pixels.
    size_t GetHeight(void) const { return Height; }

public:
    /// @summary Sets the wrapping mode to use for tecture coordinates outside
    /// the [0, 1] range on both the horizontal and vertical axes.
    /// @param mode One of GL_CLAMP, GL_CLAMP_TO_EDGE, GL_REPEAT, etc.
    void SetWrapMode(GLenum mode) { Wrap = mode; }

    /// @summary Sets the texture magnification filter to use.
    /// @param filter One of GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, etc.
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

/// @summary Queues sprites for later rendering. Sprites support rotation and
/// scaling about an origin point.
class SpriteBatch
{
protected:
    std::vector<sprite_t>  SpriteData; /// A buffer for sprite definitions.
    GLuint                 Program;    /// The OpenGL program object ID.
    shader_desc_t          ShaderDesc; /// Metadata about the shader program.
    attribute_desc_t      *AttribPTX;  /// Information about the Position-Texture attribute.
    attribute_desc_t      *AttribCLR;  /// Information about the ARGB color attribute.
    sampler_desc_t        *SamplerTEX; /// Information about the texture sampler.
    uniform_desc_t        *UniformMSS; /// Information about the screenspace -> clipspace matrix.
    sprite_effect_t        EffectData; /// Low-level sprite renderer state.
    sprite_batch_t         BatchData;  /// Low-level sprite batch state.

public:
    /// @summary Constructs a new SpriteBatch and creates GPU resources.
    /// @param The initial capacity of the sprite batch, in number of sprites.
    SpriteBatch(size_t initial_capacity);
    virtual ~SpriteBatch(void);

public:
    /// @summary Retrieves the information about the texture sampler.
    /// @return Information about the default texture sampler.
    sampler_desc_t* GetSampler(void) const
    {
        return SamplerTEX;
    }

public:
    /// @summary Queues a sprite for rendering.
    /// @param z The layer depth of the sprite, increasing into the screen.
    /// @param t The texture containing the sprite image.
    /// @param dst A rectangle defining the position and size of the sprite on the screen.
    /// @param src A rectangle defining the position and size of the image on the source texture.
    /// @param rgba An array of four float values in [0, 1] defining the RGBA tint color.
    void Add(uint32_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba);

    /// @summary Queues a sprite for rendering.
    /// @param z The layer depth of the sprite, increasing into the screen.
    /// @param t The texture containing the sprite image.
    /// @param dst A rectangle defining the position and size of the sprite on the screen.
    /// @param src A rectangle defining the position and size of the image on the source texture.
    /// @param rgba An array of four float values in [0, 1] defining the RGBA tint color.
    /// @param rot The sprite orientation, in radians. Rotation is performed clockwise.
    /// @param ox The x-coordinate of the sprite origin, relative to the upper-left corner of the sprite.
    /// @param oy The y-coordinate of the sprite origin, relative to the upper-left corner of the sprite.
    void Add(uint32_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba, float rot, float ox, float oy);

    /// @summary Queues a sprite for rendering.
    /// @param z The layer depth of the sprite, increasing into the screen.
    /// @param t The texture containing the sprite image.
    /// @param x The x-coordinate of the sprite, in pixels.
    /// @param y The y-coordinate of the sprite, in pixels.
    /// @param src A rectangle defining the position and size of the image on the source texture.
    /// @param rgba An array of four float values in [0, 1] defining the RGBA tint color.
    void Add(uint32_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba);

    /// @summary Queues a sprite for rendering.
    /// @param z The layer depth of the sprite, increasing into the screen.
    /// @param t The texture containing the sprite image.
    /// @param x The x-coordinate of the sprite, in pixels.
    /// @param y The y-coordinate of the sprite, in pixels.
    /// @param src A rectangle defining the position and size of the image on the source texture.
    /// @param rgba An array of four float values in [0, 1] defining the RGBA tint color.
    /// @param rot The sprite orientation, in radians. Rotation is performed clockwise.
    /// @param ox The x-coordinate of the sprite origin, relative to the upper-left corner of the sprite.
    /// @param oy The y-coordinate of the sprite origin, relative to the upper-left corner of the sprite.
    /// @param sx The scale factor to apply along the horizontal axis, 1.0 = no scaling.
    /// @param sy The scale factor to apply along the vertical axis, 1.0 = no scaling.
    void Add(uint32_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba, float rot, float ox, float oy, float sx, float sy);

    /// @summary Disables alpha blending. Changing the blend mode flushes the
    /// current contents of the sprite batch.
    void SetBlendModeNone(void);

    /// @summary Enables standard alpha blending. Changing the blend mode
    /// flushes the current contents of the sprite batch.
    void SetBlendModeAlpha(void);

    /// @summary Enables additive alpha blending. Changing the blend mode
    /// flushes the current contents of the sprite batch.
    void SetBlendModeAdditive(void);

    /// @summary Enables premultiplied alpha blending. Changing the blend mode
    /// flushes the current contents of the sprite batch.
    void SetBlendModePremultiplied(void);

public:
    /// @summary Sets the width and height of the viewport. The viewport
    /// dimensions should be set prior to calling SpriteBatch::Flush().
    /// @param width The viewport width, in pixels.
    /// @param height The viewport height, in pixels.
    virtual void SetViewport(int width, int height);

    /// @summary FLushes the current contents of the sprite batch to the GPU.
    virtual void Flush(void);

    /// @summary Disposes of resources associated with the sprite batch.
    virtual void Dispose(void);
};

/// @summary Measures and renders text using a monospace bitmap font.
class SpriteFont
{
protected:
    Texture *GlyphTexture;  /// The texture defining the glyph bitmaps.
    rect_t   SourceRect;    /// The bounding rectangle of the glyph texture.
    float    CharWidth;     /// The width of a single character, in pixels.
    float    CharHeight;    /// The height of a single character, in pixels.
    float    SpacingX;      /// The horizontal spacing between characters.
    float    SpacingY;      /// The vertical spacing between lines.
    char     FirstChar;     /// The codepoint of the first glyph in the font.
    char     LastChar;      /// The codepoint of the last glyph in the font.

public:
    SpriteFont(void);
    virtual ~SpriteFont(void);

public:
    /// @summary Retrieve the texture containing the glyph data.
    /// @return The texture containing the glyph bitmap data.
    Texture* GetTexture(void)    const { return GlyphTexture; }

    /// @summary Retrieve the glyph texture rectangle.
    /// @return The rectangle describing the glyph texture.
    rect_t   GetSourceRect(void) const { return SourceRect;   }

    /// @summary Retrieve the width of a single character, in pixels.
    /// @return The character width, in pixels.
    float    GetCharWidth(void)  const { return CharWidth;    }

    /// @summary Retrieve the height of a single character, in pixels.
    /// @return The character height, in pixels.
    float    GetCharHeight(void) const { return CharHeight;   }

    /// @summary Retrieve the horizontal spacing between characters.
    /// @return The horizontal spacing between characters, in pixels.
    float    GetSpacingX(void)   const { return SpacingX;     }

    /// @summary Retrieve the vertical spacing between lines.
    /// @return The vertical spacing between lines, in pixels.
    float    GetSpacingY(void)   const { return SpacingY;     }

    /// @summary Retrieve the codepoint of the first character in the font.
    /// @return The codepoint of the first character defined in the font.
    char     GetFirstChar(void)  const { return FirstChar;    }

    /// @summary Retrieve the codepoint of the last character in the font.
    /// @return The codepoint of the last character defined in the font.
    char     GetLastChar(void)   const { return LastChar;     }

public:
    /// @summary Sets the fonts attributes.
    /// @param t The texture defining the glyph bitmap data.
    /// @param chw The width of a single character, in pixels.
    /// @param chh The height of a single character, in pixels.
    /// @param sx The horizontal spacing between characters, in pixels.
    /// @param sy The vertical spacing between characters, in pixels.
    /// @param first The codepoint of the first character defined in the font.
    /// @param last The codepoint of the last character defined in the font.
    void SetSource(Texture *t, float chw, float chh, float sx, float sy, char first, char last);

    /// @summary Measures the dimensions of a string when rendered with the font.
    /// @param str The string to measure.
    /// @param bounds On return, this rectangle is updated with the width and
    /// height of the bounding box of the string when rendered with the font.
    void Measure(std::string const &str, rect_t *bounds);

    /// @summary Generates sprites for the characters in a string.
    /// @param str The string for which sprites will be generated.
    /// @param x The x-coordinate of the upper-left corner of the first character.
    /// @param y The y-coordinate of the upper-left corner of the first character.
    /// @param rgba The RGBA tint color for the glyph sprites.
    /// @param batch The target sprite batch.
    void Draw(std::string const &str, float x, float y, uint32_t z, float const *rgba, SpriteBatch *batch);
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
