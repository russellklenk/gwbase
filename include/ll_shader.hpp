/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines some useful utility functions for working with OpenGL
/// shader programs. The implementation targets the OpenGL 3.3 core profile,
/// but does not utilize ARB_shader_subroutine, ARB_program_interface_query,
/// ARB_shader_storage_block or ARB_separate_shader_objects, since those
/// extensions are not widely supported on all platforms.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef LL_SHADER_HPP
#define LL_SHADER_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Defines the maximum number of shader stages. In OpenGL 3.3 Core,
/// we have stages GL_VERTEX_SHADER, GL_GEOMETRY_SHADER and GL_FRAGMENT_SHADER,
/// with extensions adding GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER.
#ifndef GL_MAX_SHADER_STAGES
#define GL_MAX_SHADER_STAGES  (5U)
#endif

/// @summary Describes an active GLSL attribute.
struct attribute_desc_t
{
    GLenum  DataType;     /// The data type, ex. GL_FLOAT
    GLint   Location;     /// The assigned location within the program
    size_t  DataSize;     /// The size of the attribute data, in bytes
    size_t  DataOffset;   /// The offset of the attribute data, in bytes
    size_t  Dimension;    /// The data dimension, for array types
};

/// @summary Describes an active GLSL sampler.
struct sampler_desc_t
{
    GLenum  SamplerType;  /// The sampler type, ex. GL_SAMPLER_2D
    GLenum  BindTarget;   /// The texture bind target, ex. GL_TEXTURE_2D
    GLint   Location;     /// The assigned location within the program
    GLint   ImageUnit;    /// The assigned texture image unit, ex. GL_TEXTURE0
};

/// @summary Describes an active GLSL uniform.
struct uniform_desc_t
{
    GLenum  DataType;     /// The data type, ex. GL_FLOAT
    GLint   Location;     /// The assigned location within the program
    size_t  DataSize;     /// The size of the uniform data, in bytes
    size_t  DataOffset;   /// The offset of the uniform data, in bytes
    size_t  Dimension;    /// The data dimension, for array types
};

/// @summary Describes a successfully compiled and linked GLSL shader program.
struct shader_desc_t
{
    size_t            UniformCount;   /// Number of active uniforms
    uint32_t         *UniformNames;   /// Hashed names of active uniforms
    uniform_desc_t   *Uniforms;       /// Information about active uniforms
    size_t            AttributeCount; /// Number of active inputs
    uint32_t         *AttributeNames; /// Hashed names of active inputs
    attribute_desc_t *Attributes;     /// Information about active inputs
    size_t            SamplerCount;   /// Number of active samplers
    uint32_t         *SamplerNames;   /// Hashed names of samplers
    sampler_desc_t   *Samplers;       /// Information about active samplers
    void             *Metadata;       /// Pointer to the raw memory
};

/// @summary Describes the source code input to the shader compiler and linker.
/// This is provided as a convenience and allows building a shader in one step.
struct shader_source_t
{
    size_t  StageCount;                        /// Number of valid stages
    GLenum  StageNames [GL_MAX_SHADER_STAGES]; /// GL_VERTEX_SHADER, etc.
    GLsizei StringCount[GL_MAX_SHADER_STAGES]; /// Number of strings per-stage
    char  **SourceCode [GL_MAX_SHADER_STAGES]; /// NULL-terminated ASCII strings
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Given an ASCII string name, calculates a 32-bit hash value. This
/// function is used for generating names for shader attributes, uniforms and
/// samplers, allowing for more efficient look-up by name.
/// @param name A NULL-terminated ASCII string identifier.
/// @return A 32-bit unsigned integer hash of the name.
uint32_t shader_name(char const *name);

/// @summary Determines whether an identifier would be considered a GLSL built-
/// in value; that is, whether the identifier starts with 'gl_'.
/// @param name A NULL-terminated ASCII string identifier.
/// @return true if @a name starts with 'gl_'.
bool glsl_builtin(char const *name);

/// @summary Creates an OpenGL shader object and compiles shader source code.
/// @param shader_type The shader type, for example GL_VERTEX_SHADER.
/// @param shader_source An array of NULL-terminated ASCII strings representing
/// the source code of the shader program.
/// @param string_count The number of strings in the @a shader_source array.
/// @param out_shader On return, this address stores the OpenGL shader object.
/// @param out_log_size On return, this address stores the number of bytes in
/// the shader compile log. Retrieve log content with glsprite_compile_log().
/// @return true if shader compilation was successful.
bool compile_shader(GLenum shader_type, char **shader_source, size_t string_count, GLuint *out_shader, size_t *out_log_size);

/// @summary Retrieves the log for the most recent shader compilation.
/// @param shader The OpenGL shader object.
/// @param buffer The destination buffer.
/// @param buffer_size The maximum number of bytes to write to @a buffer.
void copy_compile_log(GLuint shader, char *buffer, size_t buffer_size);

/// @summary Creates an OpenGL program object and attaches, but does not link,
/// associated shader fragments. Prior to linking, vertex attribute and
/// fragment output bindings should be specified by the application using
/// assign_vertex_attributes() and assign_fragment_outputs().
/// @param shader_list The list of shader object to attach.
/// @param shader_count The number of shader objects in the shader list.
/// @param out_program On return, this address stores the program object.
/// @return true if the shader objects were attached successfully.
bool attach_shaders(GLuint *shader_list, size_t shader_count, GLuint *out_program);

/// @summary Sets the mapping of vertex attribute names to zero-based indices
/// of the vertex attributes within the vertex format definition. See the
/// glEnableVertexAttribArray and glVertexAttribPointer documentation. The
/// vertex attribute bindings should be set before linking the shader program.
/// @param program The OpenGL program object being modified.
/// @param attrib_names An array of NULL-terminated ASCII strings specifying
/// the vertex attribute names.
/// @param attrib_locations An array of zero-based integers specifying the
/// vertex attribute location assignments, such that attrib_names[i] is bound
/// to attrib_locations[i].
/// @param attrib_count The number of elements in the name and location arrays.
/// @return true if the bindings were assigned without error.
bool assign_vertex_attributes(GLuint program, char const **attrib_names, GLuint  *attrib_locations, size_t attrib_count);

/// @summary Sets the mapping of fragment shader outputs to draw buffer
/// indices. See the documentation for glBindFragDataLocation and glDrawBuffers
/// for more information. The fragment shader output bindings should be set
/// before linking the shader program.
/// @param program The OpenGL program object being modified.
/// @param output_names An array of NULL-terminated ASCII strings specifying
/// the fragment shader output names.
/// @param output_locations An array of zero-based integers specifying the
/// draw buffer index assignments, such that output_names[i] is bound
/// to draw buffer output_locations[i].
/// @param output_count The number of elements in the name and location arrays.
/// @return true if the bindings were assigned without error.
bool assign_fragment_outputs(GLuint program, char const **output_names, GLuint *output_locations, size_t output_count);

/// @summary Links and validates shader fragments and assigns any automatic
/// vertex attribute, fragment shader output and uniform locations.
/// @param program The OpenGL program object to link.
/// @param out_max_name On return, this address is updated with number of bytes
/// required to store the longest name string of a vertex attribute, uniform,
/// texture sampler or fragment shader output, including the terminating NULL.
/// @param out_log_size On return, this address is updated with the number of
/// bytes in the shader linker log. Retrieve log content with the function
/// glsprite_linker_log().
/// @return true if shader linking was successful.
bool link_program(GLuint program, size_t *out_max_name, size_t *out_log_size);

/// @summary Retrieves the log for the most recent shader program linking.
/// @param program The OpenGL program object.
/// @param buffer The destination buffer.
/// @param buffer_size The maximum number of bytes to write to @a buffer.
void copy_linker_log(GLuint program, char *buffer, size_t buffer_size);

/// @summary Allocates memory for a shader_desc_t structure using the standard
/// C library malloc() function. The various counts can be obtained from the
/// values returned by reflect_program_counts().
/// @param desc The shader description to initialize.
/// @param num_attribs The number of active attributes for the program.
/// @param num_samplers The number of active texture samplers for the program.
/// @param num_uniforms The number of active uniforms for the program.
/// @return true if memory was allocated successfully.
bool shader_desc_alloc(shader_desc_t *desc, size_t num_attribs, size_t num_samplers, size_t num_uniforms);

/// @summary Releases memory for a shader_desc_t structure using the standard
/// C library free() function.
/// @param desc The shader description to release.
void shader_desc_free(shader_desc_t *desc);

/// @summary Counts the number of active vertex attribues, texture samplers
/// and uniform values defined in a shader program.
/// @param program The OpenGL program object to query.
/// @param buffer A temporary buffer used to hold attribute and uniform names.
/// @param buffer_size The maximum number of bytes that can be written to the
/// temporary name buffer.
/// @param include_builtins Specify true to include GLSL builtin values in the
/// returned vertex attribute count.
/// @param out_num_attribs On return, this address is updated with the number
/// of active vertex attribute values.
/// @param out_num_samplers On return, this address is updated with the number
/// of active texture sampler values.
/// @param out_num_uniforms On return, this address is updated with the number
/// of active uniform values.
void reflect_program_counts(
    GLuint  program,
    char   *buffer,
    size_t  buffer_size,
    bool    include_builtins,
    size_t *out_num_attribs,
    size_t *out_num_samplers,
    size_t *out_num_uniforms);

/// @summary Retrieves descriptions of active vertex attributes, texture
/// samplers and uniform values defined in a shader program.
/// @param program The OpenGL program object to query.
/// @param buffer A temporary buffer used to hold attribute and uniform names.
/// @param buffer_size The maximum number of bytes that can be written to the
/// temporary name buffer.
/// @param include_builtins Specify true to include GLSL builtin values in the
/// returned vertex attribute count.
/// @param attrib_names Pointer to an array to be filled with the 32-bit hash
/// values of the active vertex attribute names.
/// @param attrib_info Pointer to an array to be filled with vertex attribute
/// descriptions.
/// @param sampler_names Pointer to an array to be filled with the 32-bit hash
/// values of the active texture sampler names.
/// @param sampler_info Pointer to an array to be filled with texture sampler
/// descriptions.
/// @param uniform_names Pointer to an array to be filled with the 32-bit hash
/// values of the active uniform names.
/// @param uniform_info Pointer to an array to be filled with uniform
/// descriptions.
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
    uniform_desc_t   *uniform_info);

/// @summary Binds a texture object to a texture sampler for the currently bound shader program.
/// @param sampler The description of the sampler to set.
/// @param texture The OpenGL texture object to bind to the sampler.
void set_sampler(sampler_desc_t *sampler, GLuint texture);

/// @summary Sets a uniform value for the currently bound shader program.
/// @param uniform The description of the uniform to set.
/// @param value The data to copy to the uniform.
/// @param transpose For matrix values, specify true to transpose the matrix
/// elements before passing them to the shader program.
void set_uniform(uniform_desc_t *uniform, void const *value, bool transpose);

/// @summary Initializes a shader source code buffer to empty.
/// @param source The source code buffer to clear.
void shader_source_init(shader_source_t *source);

/// @summary Adds source code for a shader stage to a shader source buffer.
/// @param source The source code buffer to modify.
/// @param shader_stage The shader stage, for example, GL_VERTEX_SHADER.
/// @param source_code An array of NULL-terminated ASCII strings specifying the
/// source code fragments for the specified shader stage.
/// @param string_count The number of strings in the source_code array.
void shader_source_add(shader_source_t *source, GLenum shader_stage, char **source_code, size_t string_count);

/// @summary Compiles, links and reflects a shader program.
/// @param source The shader source code buffer.
/// @param shader The shader program object to initialize.
/// @param out_program On return, this address is set to the identifier of the
/// OpenGL shader program object. If an error occurs, this value will be 0.
/// @return true if the build process was successful.
bool build_shader(shader_source_t *source, shader_desc_t *shader, GLuint *out_program);

/// @summary Searches a list of name-value pairs for a named item.
/// @param name_u32 The 32-bit unsigned integer hash of the search query.
/// @param name_list A list of 32-bit unsigned integer name hashes.
/// @param value_list A list of values, ordered such that name_list[i]
/// corresponds to value_list[i].
/// @param count The number of items in the name and value lists.
template <typename T>
static inline T* gl_shader_kv_find(
    uint32_t        name_u32,
    uint32_t const *name_list,
    T              *value_list,
    size_t          count)
{
    for (size_t i = 0;  i < count; ++i)
    {
        if (name_list[i] == name_u32)
            return &value_list[i];
    }
    return NULL;
}
/// @summary Searches a list of name-value pairs for a named item.
/// @param name_str A NULL-terminated ASCII string specifying the search query.
/// @param name_list A list of 32-bit unsigned integer name hashes.
/// @param value_list A list of values, ordered such that name_list[i]
/// corresponds to value_list[i].
/// @param count The number of items in the name and value lists.
template <typename T>
static inline T* gl_shader_kv_find(
    char const     *name_str,
    uint32_t const *name_list,
    T              *value_list,
    size_t          count)
{
    uint32_t name_u32 = shader_name(name_str);
    return gl_shader_kv_find(name_u32, name_list, value_list, count);
}

/// @summary Searches for a vertex attribute definition by name.
/// @param shader The shader program object to query.
/// @param name A NULL-terminated ASCII string vertex attribute identifier.
/// @return The corresponding vertex attribute definition, or NULL.
static inline attribute_desc_t *find_attribute(shader_desc_t *shader, char const *name)
{
    return gl_shader_kv_find(name, shader->AttributeNames, shader->Attributes, shader->AttributeCount);
}

/// @summary Searches for a texture sampler definition by name.
/// @param shader The shader program object to query.
/// @param name A NULL-terminated ASCII string texture sampler identifier.
/// @return The corresponding texture sampler definition, or NULL.
static inline sampler_desc_t* find_sampler(shader_desc_t *shader, char const *name)
{
    return gl_shader_kv_find(name, shader->SamplerNames, shader->Samplers, shader->SamplerCount);
}

/// @summary Searches for a uniform variable definition by name.
/// @param shader The shader program object to query.
/// @param name A NULL-terminated ASCII string uniform variable identifier.
/// @return The corresponding uniform variable definition, or NULL.
static inline uniform_desc_t* find_uniform(shader_desc_t *shader, char const *name)
{
    return gl_shader_kv_find(name, shader->UniformNames, shader->Uniforms, shader->UniformCount);
}

#endif /* !defined(LL_SHADER_HPP) */
