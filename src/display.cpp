/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.hpp"
#include "glimage.hpp"
#include "imtga.hpp"
#include "math.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
static char const *SpriteBatch_VSS =
    "#version 330\n"
    "uniform mat4 uMSS;\n"
    "layout (location = 0) in vec4 aPTX;\n"
    "layout (location = 1) in vec4 aCLR;\n"
    "out vec4 vCLR;\n"
    "out vec2 vTEX;\n"
    "void main() {\n"
    "    vCLR = aCLR;\n"
    "    vTEX = vec2(aPTX.z, aPTX.w);\n"
    "    gl_Position = uMSS * vec4(aPTX.x, aPTX.y, 0, 1);\n"
    "}\n";

static char const *SpriteBatch_FSS =
    "#version 330\n"
    "uniform sampler2D sTEX;\n"
    "in  vec2 vTEX;\n"
    "in  vec4 vCLR;\n"
    "out vec4 oCLR;\n"
    "void main() {\n"
    "    oCLR = texture(sTEX, vTEX) * vCLR;\n"
    "}\n";

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

SpriteBatch::SpriteBatch(size_t initial_capacity)
    :
    Program(0),
    AttribPTX(NULL),
    AttribCLR(NULL),
    SamplerTEX(NULL),
    UniformMSS(NULL)
{
    SpriteData.reserve(initial_capacity);

    shader_source_t sources;
    shader_source_init(&sources);
    shader_source_add(&sources, GL_VERTEX_SHADER, (char**) &SpriteBatch_VSS, 1);
    shader_source_add(&sources, GL_FRAGMENT_SHADER, (char**) &SpriteBatch_FSS, 1);
    build_shader(&sources, &ShaderDesc, &Program);
    AttribPTX  = find_attribute(&ShaderDesc, "aPTX");
    AttribCLR  = find_attribute(&ShaderDesc, "aCLR");
    SamplerTEX = find_sampler(&ShaderDesc, "sTEX");
    UniformMSS = find_uniform(&ShaderDesc, "uMSS");

    create_sprite_batch(&BatchData, initial_capacity);
    create_sprite_effect(&EffectData, initial_capacity, sizeof(sprite_vertex_ptc_t), sizeof(uint16_t));
    sprite_effect_setup_vao_ptc(&EffectData);
}

SpriteBatch::~SpriteBatch(void)
{
    Dispose();
}

void SpriteBatch::Dispose(void)
{
    if (Program)
    {
        delete_sprite_effect(&EffectData);
        delete_sprite_batch(&BatchData);
        shader_desc_free(&ShaderDesc);
        glDeleteProgram(Program);
        SpriteData.clear();
        AttribPTX  = NULL;
        AttribCLR  = NULL;
        SamplerTEX = NULL;
        UniformMSS = NULL;
    }
}

void SpriteBatch::SetViewport(int width, int height)
{
    sprite_effect_set_viewport(&EffectData, width, height);
}

static uint32_t color32(float const *rgba)
{
    uint32_t r = (uint32_t) clamp(rgba[0] * 255.0f, 0.0f, 255.0f);
    uint32_t g = (uint32_t) clamp(rgba[1] * 255.0f, 0.0f, 255.0f);
    uint32_t b = (uint32_t) clamp(rgba[2] * 255.0f, 0.0f, 255.0f);
    uint32_t a = (uint32_t) clamp(rgba[3] * 255.0f, 0.0f, 255.0f);
    return ((a << 24) | (b << 16) | (g << 8) | r);
}

void SpriteBatch::Add(uint32_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba)
{
    sprite_t sprite;
    sprite.ScreenX       = dst.X;
    sprite.ScreenY       = dst.Y;
    sprite.OriginX       = 0.0f;
    sprite.OriginY       = 0.0f;
    sprite.ScaleX        = 1.0f;
    sprite.ScaleY        = 1.0f;
    sprite.Orientation   = 0.0f;
    sprite.TintColor     = color32(rgba);
    sprite.ImageX        = src.X;
    sprite.ImageY        = src.Y;
    sprite.ImageWidth    = src.Width;
    sprite.ImageHeight   = src.Height;
    sprite.TextureWidth  = t->GetWidth();
    sprite.TextureHeight = t->GetHeight();
    sprite.LayerDepth    = z;
    sprite.RenderState   = uint32_t(t->GetId());
    SpriteData.push_back(sprite);
}

void SpriteBatch::Add(uint32_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba)
{
    sprite_t sprite;
    sprite.ScreenX       = x;
    sprite.ScreenY       = y;
    sprite.OriginX       = 0.0f;
    sprite.OriginY       = 0.0f;
    sprite.ScaleX        = 1.0f;
    sprite.ScaleY        = 1.0f;
    sprite.Orientation   = 0.0f;
    sprite.TintColor     = color32(rgba);
    sprite.ImageX        = src.X;
    sprite.ImageY        = src.Y;
    sprite.ImageWidth    = src.Width;
    sprite.ImageHeight   = src.Height;
    sprite.TextureWidth  = t->GetWidth();
    sprite.TextureHeight = t->GetHeight();
    sprite.LayerDepth    = z;
    sprite.RenderState   = uint32_t(t->GetId());
    SpriteData.push_back(sprite);
}

void SpriteBatch::Add(uint32_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba, float rot, float ox, float oy)
{
    sprite_t sprite;
    sprite.ScreenX       = dst.X;
    sprite.ScreenY       = dst.Y;
    sprite.OriginX       = ox;
    sprite.OriginY       = oy;
    sprite.ScaleX        = 1.0f;
    sprite.ScaleY        = 1.0f;
    sprite.Orientation   = rot;
    sprite.TintColor     = color32(rgba);
    sprite.ImageX        = src.X;
    sprite.ImageY        = src.Y;
    sprite.ImageWidth    = src.Width;
    sprite.ImageHeight   = src.Height;
    sprite.TextureWidth  = t->GetWidth();
    sprite.TextureHeight = t->GetHeight();
    sprite.LayerDepth    = z;
    sprite.RenderState   = uint32_t(t->GetId());
    SpriteData.push_back(sprite);
}

void SpriteBatch::Add(uint32_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba, float rot, float ox, float oy, float sx, float sy)
{
    sprite_t sprite;
    sprite.ScreenX       = x;
    sprite.ScreenY       = y;
    sprite.OriginX       = ox;
    sprite.OriginY       = oy;
    sprite.ScaleX        = sx;
    sprite.ScaleY        = sy;
    sprite.Orientation   = rot;
    sprite.TintColor     = color32(rgba);
    sprite.ImageX        = src.X;
    sprite.ImageY        = src.Y;
    sprite.ImageWidth    = src.Width;
    sprite.ImageHeight   = src.Height;
    sprite.TextureWidth  = t->GetWidth();
    sprite.TextureHeight = t->GetHeight();
    sprite.LayerDepth    = z;
    sprite.RenderState   = uint32_t(t->GetId());
    SpriteData.push_back(sprite);
}

void SpriteBatch::SetBlendModeNone(void)
{
    Flush();
    sprite_effect_blend_none(&EffectData);
}

void SpriteBatch::SetBlendModeAlpha(void)
{
    Flush();
    sprite_effect_blend_alpha(&EffectData);
}

void SpriteBatch::SetBlendModeAdditive(void)
{
    Flush();
    sprite_effect_blend_additive(&EffectData);
}

void SpriteBatch::SetBlendModePremultiplied(void)
{
    Flush();
    sprite_effect_blend_premultiplied(&EffectData);
}

static void sprite_effect_setup(sprite_effect_t *effect, void *context)
{
    UNUSED_ARG(effect);
    UNUSED_ARG(context);
}

static void sprite_effect_apply_state(sprite_effect_t *effect, uint32_t state, void *context)
{
    UNUSED_ARG(effect);
    SpriteBatch *batch = (SpriteBatch*) context;
    set_sampler (batch->GetSampler(), GLuint(state));
}

void SpriteBatch::Flush(void)
{
    uint32_t count = uint32_t(SpriteData.size());
    if (count > 0)
    {
        sprite_effect_apply_t fxfuncs = {
            sprite_effect_setup,
            sprite_effect_apply_state
        };

        glFrontFace(GL_CCW);
        glUseProgram(Program);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        sprite_effect_bind_buffers(&EffectData);
        sprite_effect_apply_blendstate(&EffectData);
        set_uniform(UniformMSS, EffectData.Projection, false);

        ensure_sprite_batch(&BatchData, count);
        generate_quads(BatchData.Quads, BatchData.State, BatchData.Order, 0, &SpriteData[0], 0, count);
        BatchData.Count = count;

        sprite_effect_draw_batch_ptc(&EffectData, &BatchData, &fxfuncs, this);

        flush_sprite_batch(&BatchData);
        SpriteData.clear();
    }
}

SpriteFont::SpriteFont(void)
    :
    GlyphTexture(NULL),
    CharWidth(0),
    CharHeight(0),
    SpacingX(0),
    SpacingY(0),
    FirstChar(0),
    LastChar(0)
{
    /* empty */
}

SpriteFont::~SpriteFont(void)
{
    /* empty */
}

void SpriteFont::SetSource(Texture *t, float chw, float chh, float sx, float sy, char first, char last)
{
    GlyphTexture      = t;
    SourceRect.X      = 0;
    SourceRect.Y      = 0;
    SourceRect.Width  = (float) t->GetWidth();
    SourceRect.Height = (float) t->GetHeight();
    CharWidth         = chw;
    CharHeight        = chh;
    SpacingX          = sx;
    SpacingY          = sy;
    FirstChar         = first;
    LastChar          = last;
}

void SpriteFont::Measure(std::string const &str, rect_t *bounds)
{
    size_t maxcol = 1;
    size_t curcol = 0;
    size_t nlines = 1;
    size_t nchars = str.length();

    for (size_t i = 0; i < nchars; ++i)
    {
        curcol++;
        if (str[i] == '\n')
        {
            if (curcol > maxcol)
            {
                maxcol = curcol - 1;
            }
            curcol = 0;
            nlines++;
        }
    }
    if (curcol > maxcol)
    {
        maxcol = curcol;
    }
    bounds->Width  = maxcol * SpacingX;
    bounds->Height = nlines * SpacingY;
}

void SpriteFont::Draw(std::string const &str, float x, float y, uint32_t z, float const *rgba, float sx, float sy, SpriteBatch *batch)
{
    float  cur_x  = x;
    float  cur_y  = y;
    size_t nchars = str.length();
    size_t maxcol = GlyphTexture->GetWidth() / CharWidth;
    for (size_t i = 0; i < nchars; ++i)
    {
        char ch = str[i];
        if (ch != '\n')
        {
            uint8_t chi = uint8_t(ch - FirstChar);
            uint8_t sc  = chi % maxcol; // column on source texture
            uint8_t sr  = chi / maxcol; // row on source texture
            rect_t src  = {
                SourceRect.X + sc * CharWidth,
                SourceRect.Y + sr * CharHeight,
                CharWidth, CharHeight
            };
            batch->Add(z, GlyphTexture, cur_x, cur_y, src, rgba, 0, 0, 0, sx, sy);
            cur_x += SpacingX * sx;
        }
        else
        {
            cur_x  = x;
            cur_y += SpacingY * sy;
        }
    }
}

DisplayManager::DisplayManager(void)
    :
    MainWindow(NULL),
    DefaultBatch(NULL),
    DefaultFont(NULL),
    FontTexture(NULL),
    PlayerTexture(NULL)
{
    /* empty */
}

DisplayManager::~DisplayManager(void)
{
    Shutdown();
}


bool DisplayManager::Init(GLFWwindow *win)
{
    if (win != NULL)
    {
        MainWindow     = win;
        DefaultBatch   = new SpriteBatch(16383);
        DefaultFont    = new SpriteFont();
        FontTexture    = new Texture();
        PlayerTexture  = new Texture();

        if (FontTexture->LoadFromFile("assets/font.tga") == false)
        {
            fprintf(stderr, "ERROR: Could not load assets/font.tga.\n");
            return false;
        }
        else DefaultFont->SetSource(FontTexture, 8, 12, 6, 10, ' ', '~');

        if (PlayerTexture->LoadFromFile("assets/player.tga") == false)
        {
            fprintf(stderr, "ERROR: Could not load assets/player.tga.\n");
            return false;
        }
        return true;
    }
    else return false;
}

void DisplayManager::Clear(float r, float g, float b, float a, float z, uint8_t s)
{
    glClearColor(r, g, b, a);
    glClearDepth(z);
    glClearStencil(s);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void DisplayManager::BeginFrame(void)
{
    DefaultBatch->SetBlendModeNone();
}

void DisplayManager::SetViewport(int width, int height)
{
    glViewport(0, 0, width, height);
    DefaultBatch->SetViewport(width, height);
}

void DisplayManager::EndFrame(void)
{
    DefaultBatch->Flush();
}

void DisplayManager::Shutdown(void)
{
    if (DefaultBatch)
    {
        delete DefaultBatch;
        DefaultBatch = NULL;
    }
    if (DefaultFont)
    {
        delete DefaultFont;
        DefaultFont = NULL;
    }
    if (FontTexture)
    {
        delete FontTexture;
        FontTexture = NULL;
    }
    if (PlayerTexture)
    {
        delete PlayerTexture;
        PlayerTexture = NULL;
    }
}
