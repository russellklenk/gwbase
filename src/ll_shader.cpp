/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines some useful utility functions for working with OpenGL
/// shader programs. The implementation targets the OpenGL 3.3 core profile,
/// but does not utilize ARB_shader_subroutine, ARB_program_interface_query,
/// ARB_shader_storage_block or ARB_separate_shader_objects, since those
/// extensions are not widely supported on all platforms.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#ifdef  _MSC_VER
#include <intrin.h>
#endif
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "ll_shader.hpp"

static inline uint32_t rotl32(uint32_t x, int8_t r)
{
    return (x << r) | (x >> (32 - r));
}

static size_t data_size(GLenum data_type)
{
    switch (data_type)
    {
    case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
    case GL_FLOAT:          return sizeof(GLfloat);
    case GL_FLOAT_VEC2:     return sizeof(GLfloat) *  2;
    case GL_FLOAT_VEC3:     return sizeof(GLfloat) *  3;
    case GL_FLOAT_VEC4:     return sizeof(GLfloat) *  4;
    case GL_INT:            return sizeof(GLint);
    case GL_INT_VEC2:       return sizeof(GLint)   *  2;
    case GL_INT_VEC3:       return sizeof(GLint)   *  3;
    case GL_INT_VEC4:       return sizeof(GLint)   *  4;
    case GL_BOOL:           return sizeof(GLint);
    case GL_BOOL_VEC2:      return sizeof(GLint)   *  2;
    case GL_BOOL_VEC3:      return sizeof(GLint)   *  3;
    case GL_BOOL_VEC4:      return sizeof(GLint)   *  4;
    case GL_FLOAT_MAT2:     return sizeof(GLfloat) *  4;
    case GL_FLOAT_MAT3:     return sizeof(GLfloat) *  9;
    case GL_FLOAT_MAT4:     return sizeof(GLfloat) * 16;
    case GL_FLOAT_MAT2x3:   return sizeof(GLfloat) *  6;
    case GL_FLOAT_MAT2x4:   return sizeof(GLfloat) *  8;
    case GL_FLOAT_MAT3x2:   return sizeof(GLfloat) *  6;
    case GL_FLOAT_MAT3x4:   return sizeof(GLfloat) * 12;
    case GL_FLOAT_MAT4x2:   return sizeof(GLfloat) *  8;
    case GL_FLOAT_MAT4x3:   return sizeof(GLfloat) * 12;
    case GL_BYTE:           return sizeof(GLbyte);
    case GL_UNSIGNED_SHORT: return sizeof(GLushort);
    case GL_SHORT:          return sizeof(GLshort);
    case GL_UNSIGNED_INT:   return sizeof(GLuint);
    default:                break;
    }
    return 0;
}

static GLenum texture_target(GLenum sampler_type)
{
    switch (sampler_type)
    {
        case GL_SAMPLER_1D:
        case GL_INT_SAMPLER_1D:
        case GL_UNSIGNED_INT_SAMPLER_1D:
        case GL_SAMPLER_1D_SHADOW:
            return GL_TEXTURE_1D;

        case GL_SAMPLER_2D:
        case GL_INT_SAMPLER_2D:
        case GL_UNSIGNED_INT_SAMPLER_2D:
        case GL_SAMPLER_2D_SHADOW:
            return GL_TEXTURE_2D;

        case GL_SAMPLER_3D:
        case GL_INT_SAMPLER_3D:
        case GL_UNSIGNED_INT_SAMPLER_3D:
            return GL_TEXTURE_3D;

        case GL_SAMPLER_CUBE:
        case GL_INT_SAMPLER_CUBE:
        case GL_UNSIGNED_INT_SAMPLER_CUBE:
        case GL_SAMPLER_CUBE_SHADOW:
            return GL_TEXTURE_CUBE_MAP;

        case GL_SAMPLER_1D_ARRAY:
        case GL_SAMPLER_1D_ARRAY_SHADOW:
        case GL_INT_SAMPLER_1D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            return GL_TEXTURE_1D_ARRAY;

        case GL_SAMPLER_2D_ARRAY:
        case GL_SAMPLER_2D_ARRAY_SHADOW:
        case GL_INT_SAMPLER_2D_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;

        case GL_SAMPLER_BUFFER:
        case GL_INT_SAMPLER_BUFFER:
        case GL_UNSIGNED_INT_SAMPLER_BUFFER:
            return GL_TEXTURE_BUFFER;

        case GL_SAMPLER_2D_RECT:
        case GL_SAMPLER_2D_RECT_SHADOW:
        case GL_INT_SAMPLER_2D_RECT:
        case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
            return GL_TEXTURE_RECTANGLE;

        case GL_SAMPLER_2D_MULTISAMPLE:
        case GL_INT_SAMPLER_2D_MULTISAMPLE:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            return GL_TEXTURE_2D_MULTISAMPLE;

        case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

        default:
            break;
    }
    return GL_TEXTURE_1D;
}

uint32_t shader_name(char const *name)
{
    #define HAS_NULL_BYTE(x) (((x) - 0x01010101) & (~(x) & 0x80808080))
    #ifdef  _MSC_VER
        #define ROTL32(x, y) _rotl((x), (y))
    #else
        #define ROTL32(x, y) rotl32((x), (y))
    #endif

    uint32_t hash = 0;
    if (name != NULL)
    {
        // hash the majority of the data in 4-byte chunks.
        while (!HAS_NULL_BYTE(*((uint32_t*)name)))
        {
            hash  = ROTL32(hash, 7) + name[0];
            hash  = ROTL32(hash, 7) + name[1];
            hash  = ROTL32(hash, 7) + name[2];
            hash  = ROTL32(hash, 7) + name[3];
            name += 4;
        }
        // hash the remaining 0-3 bytes.
        while (*name) hash = ROTL32(hash, 7) + *name++;
    }
    return hash;
}

bool glsl_builtin(char const *name)
{
    char prefix[4] = {'g','l','_','\0'};
    return (strncmp(name, prefix, 3) == 0);
}

bool compile_shader(
    GLenum   shader_type,
    char   **shader_source,
    size_t   string_count,
    GLuint  *out_shader,
    size_t  *out_log_size)
{
    GLuint  shader   = 0;
    GLsizei log_size = 0;
    GLint   result   = GL_FALSE;

    shader = glCreateShader(shader_type);
    if (0 == shader)
    {
        if (out_shader)   *out_shader   = 0;
        if (out_log_size) *out_log_size = 1;
        return false;
    }
    glShaderSource (shader, string_count, (GLchar const**) shader_source, NULL);
    glCompileShader(shader);
    glGetShaderiv  (shader, GL_COMPILE_STATUS,  &result);
    glGetShaderiv  (shader, GL_INFO_LOG_LENGTH, &log_size);
    glGetError();

    if (out_shader)   *out_shader   = shader;
    if (out_log_size) *out_log_size = log_size + 1;
    return (result == GL_TRUE);
}

void copy_compile_log(GLuint shader, char *buffer, size_t buffer_size)
{
    GLsizei len = 0;
    glGetShaderInfoLog(shader, (GLsizei) buffer_size, &len, buffer);
    buffer[len] = '\0';
}

bool attach_shaders(GLuint *shader_list, size_t shader_count, GLuint *out_program)
{
    GLuint  program = 0;
    program = glCreateProgram();
    if (0  == program)
    {
        if (out_program) *out_program = 0;
        return false;
    }

    for (size_t i = 0; i < shader_count;  ++i)
    {
        glAttachShader(program, shader_list[i]);
        if (glGetError() != GL_NO_ERROR)
        {
            glDeleteProgram(program);
            if (out_program) *out_program = 0;
            return false;
        }
    }
    if (out_program) *out_program = program;
    return true;
}

bool assign_vertex_attributes(GLuint program, char const **attrib_names, GLuint *attrib_locations, size_t attrib_count)
{
    bool result   = true;
    for (size_t i = 0; i < attrib_count; ++i)
    {
        glBindAttribLocation(program, attrib_locations[i], attrib_names[i]);
        if (glGetError() != GL_NO_ERROR)
            result = false;
    }
    return result;
}

bool assign_fragment_outputs(GLuint program, char const **output_names, GLuint *output_locations, size_t output_count)
{
    bool result   = true;
    for (size_t i = 0; i < output_count; ++i)
    {
        glBindFragDataLocation(program, output_locations[i], output_names[i]);
        if (glGetError() != GL_NO_ERROR)
            result = false;
    }
    return result;
}

bool link_program(GLuint program, size_t *out_max_name, size_t *out_log_size)
{
    GLsizei log_size = 0;
    GLint   result   = GL_FALSE;

    glLinkProgram (program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
    if (out_log_size) *out_log_size = log_size;

    if (result == GL_TRUE)
    {
        GLint a_max = 0;
        GLint u_max = 0;
        GLint n_max = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &u_max);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &a_max);
        n_max = u_max > a_max ? u_max : a_max;
        if (out_max_name)  *out_max_name = (size_t)(n_max + 1);
    }
    else if (out_max_name) *out_max_name = 1;
    return  (result == GL_TRUE);
}

void copy_linker_log(GLuint program, char *buffer, size_t buffer_size)
{
    GLsizei len = 0;
    glGetProgramInfoLog(program, (GLsizei) buffer_size, &len, buffer);
    buffer[len] = '\0';
}

bool shader_desc_alloc(shader_desc_t *desc, size_t num_attribs, size_t num_samplers, size_t num_uniforms)
{
    attribute_desc_t *attribs       = NULL;
    sampler_desc_t   *samplers      = NULL;
    uniform_desc_t   *uniforms      = NULL;
    uint32_t         *attrib_names  = NULL;
    uint32_t         *sampler_names = NULL;
    uint32_t         *uniform_names = NULL;
    uint8_t          *memory_block  = NULL;
    uint8_t          *memory_ptr    = NULL;
    size_t            aname_size    = 0;
    size_t            sname_size    = 0;
    size_t            uname_size    = 0;
    size_t            attrib_size   = 0;
    size_t            sampler_size  = 0;
    size_t            uniform_size  = 0;
    size_t            total_size    = 0;

    // calculate the total size of all shader program metadata.
    aname_size    = sizeof(uint32_t)           * num_attribs;
    sname_size    = sizeof(uint32_t)           * num_samplers;
    uname_size    = sizeof(uint32_t)           * num_uniforms;
    attrib_size   = sizeof(attribute_desc_t)   * num_attribs;
    sampler_size  = sizeof(sampler_desc_t)     * num_samplers;
    uniform_size  = sizeof(uniform_desc_t)     * num_uniforms;
    total_size    = aname_size  + sname_size   + uname_size +
                    attrib_size + sampler_size + uniform_size;

    // perform a single large memory allocation for the metadata.
    memory_block  = (uint8_t*) malloc(total_size);
    memory_ptr    = memory_block;
    if (memory_block == NULL)
        return false;

    // assign pointers to various sub-blocks within the larger memory block.
    attrib_names  = (uint32_t*) memory_ptr;
    memory_ptr   +=  aname_size;
    attribs       = (attribute_desc_t*) memory_ptr;
    memory_ptr   +=  attrib_size;

    sampler_names = (uint32_t*) memory_ptr;
    memory_ptr   +=  sname_size;
    samplers      = (sampler_desc_t*) memory_ptr;
    memory_ptr   +=  sampler_size;

    uniform_names = (uint32_t*) memory_ptr;
    memory_ptr   +=  uname_size;
    uniforms      = (uniform_desc_t*) memory_ptr;
    memory_ptr   +=  uniform_size;

    // set all of the fields on the shader_desc_t structure.
    desc->UniformCount   = num_uniforms;
    desc->UniformNames   = uniform_names;
    desc->Uniforms       = uniforms;
    desc->AttributeCount = num_attribs;
    desc->AttributeNames = attrib_names;
    desc->Attributes     = attribs;
    desc->SamplerCount   = num_samplers;
    desc->SamplerNames   = sampler_names;
    desc->Samplers       = samplers;
    desc->Metadata       = memory_block;
    return true;
}

void shader_desc_free(shader_desc_t *desc)
{
    if (desc->Metadata != NULL)
    {
        free(desc->Metadata);
        desc->Metadata       = NULL;
        desc->UniformCount   = 0;
        desc->UniformNames   = NULL;
        desc->Uniforms       = NULL;
        desc->AttributeCount = 0;
        desc->AttributeNames = NULL;
        desc->Attributes     = NULL;
        desc->SamplerCount   = 0;
        desc->SamplerNames   = NULL;
        desc->Samplers       = NULL;
    }
}

void reflect_program_counts(
    GLuint  program,
    char   *buffer,
    size_t  buffer_size,
    bool    include_builtins,
    size_t *out_num_attribs,
    size_t *out_num_samplers,
    size_t *out_num_uniforms)
{
    size_t  num_attribs  = 0;
    GLint   attrib_count = 0;
    GLsizei buf_size     = (GLsizei) buffer_size;

    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_count);
    for (GLint i = 0; i < attrib_count; ++i)
    {
        GLenum type = GL_FLOAT;
        GLuint idx  = (GLuint) i;
        GLint  len  = 0;
        GLint  sz   = 0;
        glGetActiveAttrib(program, idx, buf_size, &len, &sz, &type, buffer);
        if (glsl_builtin(buffer) && !include_builtins)
            continue;
        num_attribs++;
    }

    size_t num_samplers  = 0;
    size_t num_uniforms  = 0;
    GLint  uniform_count = 0;

    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
    for (GLint i = 0; i < uniform_count; ++i)
    {
        GLenum type = GL_FLOAT;
        GLuint idx  = (GLuint) i;
        GLint  len  = 0;
        GLint  sz   = 0;
        glGetActiveUniform(program, idx, buf_size, &len, &sz, &type, buffer);
        if (glsl_builtin(buffer) && !include_builtins)
            continue;

        switch (type)
        {
            case GL_SAMPLER_1D:
            case GL_INT_SAMPLER_1D:
            case GL_UNSIGNED_INT_SAMPLER_1D:
            case GL_SAMPLER_1D_SHADOW:
            case GL_SAMPLER_2D:
            case GL_INT_SAMPLER_2D:
            case GL_UNSIGNED_INT_SAMPLER_2D:
            case GL_SAMPLER_2D_SHADOW:
            case GL_SAMPLER_3D:
            case GL_INT_SAMPLER_3D:
            case GL_UNSIGNED_INT_SAMPLER_3D:
            case GL_SAMPLER_CUBE:
            case GL_INT_SAMPLER_CUBE:
            case GL_UNSIGNED_INT_SAMPLER_CUBE:
            case GL_SAMPLER_CUBE_SHADOW:
            case GL_SAMPLER_1D_ARRAY:
            case GL_SAMPLER_1D_ARRAY_SHADOW:
            case GL_INT_SAMPLER_1D_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            case GL_SAMPLER_2D_ARRAY:
            case GL_SAMPLER_2D_ARRAY_SHADOW:
            case GL_INT_SAMPLER_2D_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
            case GL_SAMPLER_BUFFER:
            case GL_INT_SAMPLER_BUFFER:
            case GL_UNSIGNED_INT_SAMPLER_BUFFER:
            case GL_SAMPLER_2D_RECT:
            case GL_SAMPLER_2D_RECT_SHADOW:
            case GL_INT_SAMPLER_2D_RECT:
            case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
            case GL_SAMPLER_2D_MULTISAMPLE:
            case GL_INT_SAMPLER_2D_MULTISAMPLE:
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
            case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
                num_samplers++;
                break;

            default:
                num_uniforms++;
                break;
        }
    }

    if (out_num_attribs)  *out_num_attribs  = num_attribs;
    if (out_num_samplers) *out_num_samplers = num_samplers;
    if (out_num_uniforms) *out_num_uniforms = num_uniforms;
}

void reflect_program_details(
    GLuint            program,
    char             *buffer,
    size_t            buffer_size,
    bool              include_builtins,
    uint32_t         *attrib_names,
    attribute_desc_t *attrib_info,
    uint32_t         *sampler_names,
    sampler_desc_t   *sampler_info,
    uint32_t         *uniform_names,
    uniform_desc_t   *uniform_info)
{
    size_t  num_attribs  = 0;
    GLint   attrib_count = 0;
    GLsizei buf_size     = (GLsizei) buffer_size;

    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_count);
    for (GLint i = 0; i < attrib_count; ++i)
    {
        GLenum type = GL_FLOAT;
        GLuint idx  = (GLuint) i;
        GLint  len  = 0;
        GLint  loc  = 0;
        GLint  sz   = 0;
        glGetActiveAttrib(program, idx, buf_size, &len, &sz, &type, buffer);
        if (glsl_builtin(buffer) && !include_builtins)
            continue;

        attribute_desc_t va;
        loc           = glGetAttribLocation(program, buffer);
        va.DataType   = (GLenum) type;
        va.Location   = (GLint)  loc;
        va.DataSize   = (size_t) data_size(type) * sz;
        va.DataOffset = (size_t) 0; // for application use only
        va.Dimension  = (size_t) sz;
        attrib_names[num_attribs] = shader_name(buffer);
        attrib_info [num_attribs] = va;
        num_attribs++;
    }

    size_t num_samplers  = 0;
    size_t num_uniforms  = 0;
    GLint  uniform_count = 0;
    GLint  texture_unit  = 0;

    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);
    for (GLint i = 0; i < uniform_count; ++i)
    {
        GLenum type = GL_FLOAT;
        GLuint idx  = (GLuint) i;
        GLint  len  = 0;
        GLint  loc  = 0;
        GLint  sz   = 0;
        glGetActiveUniform(program, idx, buf_size, &len, &sz, &type, buffer);
        if (glsl_builtin(buffer) && !include_builtins)
            continue;

        switch (type)
        {
            case GL_SAMPLER_1D:
            case GL_INT_SAMPLER_1D:
            case GL_UNSIGNED_INT_SAMPLER_1D:
            case GL_SAMPLER_1D_SHADOW:
            case GL_SAMPLER_2D:
            case GL_INT_SAMPLER_2D:
            case GL_UNSIGNED_INT_SAMPLER_2D:
            case GL_SAMPLER_2D_SHADOW:
            case GL_SAMPLER_3D:
            case GL_INT_SAMPLER_3D:
            case GL_UNSIGNED_INT_SAMPLER_3D:
            case GL_SAMPLER_CUBE:
            case GL_INT_SAMPLER_CUBE:
            case GL_UNSIGNED_INT_SAMPLER_CUBE:
            case GL_SAMPLER_CUBE_SHADOW:
            case GL_SAMPLER_1D_ARRAY:
            case GL_SAMPLER_1D_ARRAY_SHADOW:
            case GL_INT_SAMPLER_1D_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
            case GL_SAMPLER_2D_ARRAY:
            case GL_SAMPLER_2D_ARRAY_SHADOW:
            case GL_INT_SAMPLER_2D_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
            case GL_SAMPLER_BUFFER:
            case GL_INT_SAMPLER_BUFFER:
            case GL_UNSIGNED_INT_SAMPLER_BUFFER:
            case GL_SAMPLER_2D_RECT:
            case GL_SAMPLER_2D_RECT_SHADOW:
            case GL_INT_SAMPLER_2D_RECT:
            case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
            case GL_SAMPLER_2D_MULTISAMPLE:
            case GL_INT_SAMPLER_2D_MULTISAMPLE:
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
            case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
                {
                    sampler_desc_t ts;
                    loc            = glGetUniformLocation(program, buffer);
                    ts.SamplerType = (GLenum) type;
                    ts.BindTarget  = (GLenum) texture_target(type);
                    ts.Location    = (GLint)  loc;
                    ts.ImageUnit   = (GLint)  texture_unit++;
                    sampler_names[num_samplers] = shader_name(buffer);
                    sampler_info [num_samplers] = ts;
                    num_samplers++;
                }
                break;

            default:
                {
                    uniform_desc_t uv;
                    loc            = glGetUniformLocation(program, buffer);
                    uv.DataType    = (GLenum) type;
                    uv.Location    = (GLint)  loc;
                    uv.DataSize    = (size_t) data_size(type) * sz;
                    uv.DataOffset  = (size_t) 0; // for application use only
                    uv.Dimension   = (size_t) sz;
                    uniform_names[num_uniforms] = shader_name(buffer);
                    uniform_info [num_uniforms] = uv;
                    num_uniforms++;
                }
                break;
        }
    }
}

void set_sampler(sampler_desc_t *sampler, GLuint texture)
{
    glActiveTexture(GL_TEXTURE0 + sampler->ImageUnit);
    glBindTexture(sampler->BindTarget, texture);
    glUniform1i(sampler->Location, sampler->ImageUnit);
}

void set_uniform(uniform_desc_t *uniform, void const *value, bool transpose)
{
    GLint          loc = uniform->Location;
    GLsizei        dim = uniform->Dimension;
    GLboolean      tm  = transpose ? GL_TRUE : GL_FALSE;
    GLint const   *id  = (GLint const*)   value;
    GLfloat const *fd  = (GLfloat const*) value;
    switch (uniform->DataType)
    {
        case GL_FLOAT:        glUniform1fv(loc, dim, fd);             break;
        case GL_FLOAT_VEC2:   glUniform2fv(loc, dim, fd);             break;
        case GL_FLOAT_VEC3:   glUniform3fv(loc, dim, fd);             break;
        case GL_FLOAT_VEC4:   glUniform4fv(loc, dim, fd);             break;
        case GL_INT:          glUniform1iv(loc, dim, id);             break;
        case GL_INT_VEC2:     glUniform2iv(loc, dim, id);             break;
        case GL_INT_VEC3:     glUniform3iv(loc, dim, id);             break;
        case GL_INT_VEC4:     glUniform4iv(loc, dim, id);             break;
        case GL_BOOL:         glUniform1iv(loc, dim, id);             break;
        case GL_BOOL_VEC2:    glUniform2iv(loc, dim, id);             break;
        case GL_BOOL_VEC3:    glUniform3iv(loc, dim, id);             break;
        case GL_BOOL_VEC4:    glUniform4iv(loc, dim, id);             break;
        case GL_FLOAT_MAT2:   glUniformMatrix2fv  (loc, dim, tm, fd); break;
        case GL_FLOAT_MAT3:   glUniformMatrix3fv  (loc, dim, tm, fd); break;
        case GL_FLOAT_MAT4:   glUniformMatrix4fv  (loc, dim, tm, fd); break;
        case GL_FLOAT_MAT2x3: glUniformMatrix2x3fv(loc, dim, tm, fd); break;
        case GL_FLOAT_MAT2x4: glUniformMatrix2x4fv(loc, dim, tm, fd); break;
        case GL_FLOAT_MAT3x2: glUniformMatrix3x2fv(loc, dim, tm, fd); break;
        case GL_FLOAT_MAT3x4: glUniformMatrix3x4fv(loc, dim, tm, fd); break;
        case GL_FLOAT_MAT4x2: glUniformMatrix4x2fv(loc, dim, tm, fd); break;
        case GL_FLOAT_MAT4x3: glUniformMatrix4x3fv(loc, dim, tm, fd); break;
        default: break;
    }
}

void shader_source_init(shader_source_t *source)
{
    source->StageCount = 0;
    for (size_t i = 0; i < GL_MAX_SHADER_STAGES; ++i)
    {
        source->StageNames [i] = 0;
        source->StringCount[i] = 0;
        source->SourceCode [i] = NULL;
    }
}

void shader_source_add(shader_source_t *source, GLenum shader_stage, char **source_code, size_t string_count)
{
    if (source->StageCount < GL_MAX_SHADER_STAGES)
    {
        source->StageNames [source->StageCount] = shader_stage;
        source->StringCount[source->StageCount] = string_count;
        source->SourceCode [source->StageCount] = source_code;
        source->StageCount++;
    }
}

bool build_shader(shader_source_t *source, shader_desc_t *shader, GLuint *out_program)
{
    GLuint shader_list[GL_MAX_SHADER_STAGES];
    GLuint program       = 0;
    char  *name_buffer   = NULL;
    size_t num_attribs   = 0;
    size_t num_samplers  = 0;
    size_t num_uniforms  = 0;
    size_t max_name      = 0;

    for (size_t  i = 0; i < source->StageCount;  ++i)
    {
        size_t  ls = 0;
        GLenum  sn = source->StageNames [i];
        GLsizei fc = source->StringCount[i];
        char  **sc = source->SourceCode [i];
        if (!compile_shader(sn, sc, fc, &shader_list[i], &ls))
            goto error_cleanup;
    }

    if (!attach_shaders(shader_list, source->StageCount, &program))
        goto error_cleanup;

    if (!link_program(program, &max_name, NULL))
        goto error_cleanup;

    // flag each attached shader for deletion when the program is deleted.
    // the shaders are automatically detached when the program is deleted.
    for (size_t i = 0; i < source->StageCount; ++i)
        glDeleteShader(shader_list[i]);

    // figure out how many attributes, samplers and uniforms we have.
    name_buffer = (char*)  malloc(max_name);
    reflect_program_counts(program, name_buffer, max_name,
        false, &num_attribs, &num_samplers, &num_uniforms);

    if (!shader_desc_alloc(shader, num_attribs, num_samplers, num_uniforms))
        goto error_cleanup;

    // now reflect the shader program to retrieve detailed information
    // about all vertex attributes, texture samplers and uniform variables.
    reflect_program_details(program, name_buffer, max_name, false,
        shader->AttributeNames, shader->Attributes,
        shader->SamplerNames,   shader->Samplers,
        shader->UniformNames,   shader->Uniforms);

    *out_program = program;
    return true;

error_cleanup:
    for (size_t i = 0; i < source->StageCount; ++i)
    {
        if (shader_list[i] != 0)
        {
            glDeleteShader(shader_list[i]);
        }
    }
    if (shader->Metadata != NULL) shader_desc_free(shader);
    if (name_buffer  != NULL)     free(name_buffer);
    if (program != 0)             glDeleteProgram(program);
    *out_program = 0;
    return false;
}
