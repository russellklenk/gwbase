/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include <stdio.h>
#include "display.hpp"
#include "glimage.hpp"
#include "imtga.hpp"
#include "math.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
static char const *SpriteBatch_VSS =
    "#version 330\n"
    "uniform  mat4 uMSS;\n"
    "layout (location = 0) in vec2 aPOS;\n"
    "layout (location = 1) in vec2 aTEX;\n"
    "layout (location = 2) in vec4 aCLR;\n"
    "out vec4 vCLR;\n"
    "out vec2 vTEX;\n"
    "void main() {\n"
    "    vCLR = aCLR;\n"
    "    vTEX = vec2(aTEX.x, aTEX.y);\n"
    "    gl_Position = uMSS * vec4(aPOS.x, aPOS.y, 0, 1);\n"
    "}\n";

static char const *SpriteBatch_FSS =
    "#version 330\n"
    "uniform sampler2D  sTEX;\n"
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

SpriteBatch::SpriteBatch(void)
    :
    AttribPos(NULL),
    AttribTex(NULL),
    AttribClr(NULL),
    UniformMSS(NULL),
    SamplerTex(NULL),
    ShaderProgram(0),
    VAO(0)
{
    /* empty */
}

SpriteBatch::~SpriteBatch(void)
{
    DeleteGPUResources();
}

void SpriteBatch::FourPoints(std::vector<float> &v, rect_t const &rect)
{
    v.push_back(rect.X);
    v.push_back(rect.Y);
    v.push_back(rect.X + rect.Width);
    v.push_back(rect.Y);
    v.push_back(rect.X + rect.Width);
    v.push_back(rect.Y + rect.Height);
    v.push_back(rect.X);
    v.push_back(rect.Y + rect.Height);
}

void SpriteBatch::SixIndices(std::vector<uint16_t> &v, uint16_t start)
{
    v.push_back(start + 0);
    v.push_back(start + 2);
    v.push_back(start + 1);
    v.push_back(start + 0);
    v.push_back(start + 3);
    v.push_back(start + 2);
}

bool SpriteBatch::CreateGPUResources(void)
{
    shader_source_t     sources;
    shader_source_init(&sources);
    shader_source_add (&sources, GL_VERTEX_SHADER,   (char**) &SpriteBatch_VSS, 1);
    shader_source_add (&sources, GL_FRAGMENT_SHADER, (char**) &SpriteBatch_FSS, 1);
    if (build_shader(&sources, &ShaderDesc, &ShaderProgram))
    {
        AttribPos  = find_attribute(&ShaderDesc, "aPOS");
        AttribTex  = find_attribute(&ShaderDesc, "aTEX");
        AttribClr  = find_attribute(&ShaderDesc, "aCLR");
        SamplerTex = find_sampler  (&ShaderDesc, "sTEX");
        UniformMSS = find_uniform  (&ShaderDesc, "uMSS");

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(AttribPos->Location);
        glEnableVertexAttribArray(AttribTex->Location);
        glEnableVertexAttribArray(AttribClr->Location);
        return true;
    }
    else return false;
}

void SpriteBatch::DeleteGPUResources(void)
{
    if (VAO)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (ShaderProgram)
    {
        shader_desc_free(&ShaderDesc);
        glDeleteProgram(ShaderProgram);
        ShaderProgram = 0;
    }
}

void SpriteBatch::SetProjection(float const *m16)
{
    mat4_set_mat4(Matrix, m16);
}

void SpriteBatch::Draw(uint8_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba)
{
    std::pair<uint8_t, Texture*> key(z, t);
    sprite_bucket_t &ref = Buckets[key];
    size_t index = ref.Vertices.size();

    FourPoints(ref.Vertices, dst);

    rect_t uv = {
        src.X      / t->GetWidth(),
        src.Y      / t->GetHeight(),
        src.Width  / t->GetWidth(),
        src.Height / t->GetHeight()
    };
    FourPoints(ref.TexCoords, uv);

    ref.Colors.push_back(rgba[0]);
    ref.Colors.push_back(rgba[1]);
    ref.Colors.push_back(rgba[2]);
    ref.Colors.push_back(rgba[3]);

    SixIndices(ref.Indices, uint16_t(index));
}

void SpriteBatch::Draw(uint8_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba)
{
    rect_t dst = { x, y, src.Width, src.Height };
    Draw(z, t, dst, src, rgba);
}

void SpriteBatch::Draw(uint8_t z, Texture *t, rect_t const &dst, rect_t const &src, float const *rgba, float rot, float ox, float oy)
{
    std::pair<uint8_t, Texture*> key(z, t);
    sprite_bucket_t& ref = Buckets[key];
    size_t index = ref.Vertices.size();

    FourPoints(ref.Vertices, dst);

    float so = sinf(rot);
    float co = cosf(rot);
    for (size_t i = 0; i < 4; ++i)
    {
        float in_x = ref.Vertices[index + (i * 2) + 0] - dst.X - ox;
        float in_y = ref.Vertices[index + (i * 2) + 1] - dst.Y - oy;
        float tx_x = dst.X + (in_x * co)- (in_y * so);
        float tx_y = dst.Y + (in_x * so)+ (in_y * co);
        ref.Vertices[index + (i * 2)+0] =  tx_x;
        ref.Vertices[index + (i * 2)+1] =  tx_y;
    }

    rect_t uv = {
        src.X      / t->GetWidth(),
        src.Y      / t->GetHeight(),
        src.Width  / t->GetWidth(),
        src.Height / t->GetHeight()
    };
    FourPoints(ref.TexCoords, uv);

    ref.Colors.push_back(rgba[0]);
    ref.Colors.push_back(rgba[1]);
    ref.Colors.push_back(rgba[2]);
    ref.Colors.push_back(rgba[3]);

    SixIndices(ref.Indices, uint16_t(index));
}

void SpriteBatch::Draw(uint8_t z, Texture *t, float x, float y, rect_t const &src, float const *rgba, float rot, float ox, float oy, float sx, float sy)
{
    float origin_x = ox * sx;
    float origin_y = oy * sy;
    rect_t dst = { x, y, src.Width * sx, src.Height * sy };
    Draw(z, t, dst, src, rgba, rot, origin_x, origin_y);
}

void SpriteBatch::Flush(void)
{
    if (!Buckets.empty())
    {
        glBindVertexArray(VAO);
        glUseProgram(ShaderProgram);
        set_uniform(UniformMSS, Matrix, false);
        for (bucket_map_t::iterator iter = Buckets.begin(); iter != Buckets.end(); ++iter)
        {
            if (!(*iter).second.Vertices.empty())
            {
                set_sampler(SamplerTex, (*iter).first.second->GetId());

                glVertexAttribPointer(AttribPos->Location, 2, GL_FLOAT, GL_FALSE, 0, &(*iter).second.Vertices[0]);
                glVertexAttribPointer(AttribTex->Location, 2, GL_FLOAT, GL_FALSE, 0, &(*iter).second.TexCoords[0]);
                glVertexAttribPointer(AttribClr->Location, 4, GL_FLOAT, GL_FALSE, 0, &(*iter).second.Colors[0]);

                glDrawElements(GL_TRIANGLES, int32_t((*iter).second.Indices.size()), GL_UNSIGNED_SHORT, &(*iter).second.Indices[0]);

                (*iter).second.Vertices.clear();
                (*iter).second.TexCoords.clear();
                (*iter).second.Colors.clear();
                (*iter).second.Indices.clear();
            }
        }
    }
}

DisplayManager::DisplayManager(void)
    :
    main_window(NULL)
{
    /* empty */
}

DisplayManager::~DisplayManager(void)
{
    main_window = NULL;
}


bool DisplayManager::init(GLFWwindow *win)
{
    if (win != NULL)
    {
        main_window = win;
        return true;
    }
    else return false;
}

void DisplayManager::shutdown(void)
{
    main_window = NULL;
}
