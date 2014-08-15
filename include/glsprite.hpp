/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines a series of functions and data types for representing
/// sprite data to be streamed to the GPU each frame. Sprites are transformed
/// on the CPU, and written to a dynamic vertex buffer each frame.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GL_SPRITE_HPP
#define GL_SPRITE_HPP

/*////////////////
//   Includes   //
////////////////*/
#include <algorithm>
#include "common.hpp"
#include "platform.hpp"

/*///////////////
//  Constants  //
///////////////*/
/// @summary Defines the location of the position and texture attributes within
/// the position-texture-color vertex. These attributes are encoded as a vec4.
#define SPRITE_PTC_LOCATION_PTX    (0)

/// @summary Defines the location of the tint color attribute within the vertex.
/// This attribute is encoded as a packed 32-bit RGBA color value.
#define SPRITE_PTC_LOCATION_CLR    (1)

/*////////////////
//  Data Types  //
////////////////*/
/// @summary A structure representing a single interleaved sprite vertex in
/// the vertex buffer. The vertex encodes 2D screen space position, texture
/// coordinate, and packed ABGR color values into 20 bytes per-vertex. The
/// GPU expands the vertex into 32 bytes. Tint color is constant per-sprite.
#pragma pack(push, 1)
struct sprite_vertex_ptc_t
{
    float    XYUV[4];
    uint32_t TintColor;
};
#pragma pack(pop)

/// @summary A structure representing the data required to describe a single
/// sprite within the application. Each sprite is translated into four vertices
/// and six indices. The application fills out a sprite descriptor and pushes
/// it to the batch for later processing.
struct sprite_t
{
    float    ScreenX;           /// Screen-space X coordinate of the origin.
    float    ScreenY;           /// Screen-space Y coordinate of the origin.
    float    OriginX;           /// Origin X-offset from the upper-left corner.
    float    OriginY;           /// Origin Y-offset from the upper-left corner.
    float    ScaleX;            /// The horizontal scale factor.
    float    ScaleY;            /// The vertical scale factor.
    float    Orientation;       /// The angle of orientation, in radians.
    uint32_t TintColor;         /// The ABGR tint color.
    uint32_t ImageX;            /// Y-offset of the upper-left corner of the source image.
    uint32_t ImageY;            /// Y-offset of the upper-left corner of the source image.
    uint32_t ImageWidth;        /// The width of the source image, in pixels.
    uint32_t ImageHeight;       /// The height of the source image, in pixels.
    uint32_t TextureWidth;      /// The width of the texture defining the source image.
    uint32_t TextureHeight;     /// The height of the texture defining the source image.
    uint32_t LayerDepth;        /// The layer depth of the sprite, increasing into the background.
    uint32_t RenderState;       /// An application-defined render state identifier.
};

/// @summary A structure storing the data required to represent a sprite within
/// the sprite batch. Sprites are transformed to quads when they are pushed to
/// the sprite batch. Each quad definition is 64 bytes.
struct quad_t
{
    float    Source[4];         /// The XYWH rectangle on the source texture.
    float    Target[4];         /// The XYWH rectangle on the screen.
    float    Origin[2];         /// The XY origin point of rotation.
    float    Scale[2];          /// Texture coordinate scale factors.
    float    Orientation;       /// The angle of orientation, in radians.
    uint32_t TintColor;         /// The ABGR tint color.
};

/// @summary Data used for sorting buffered quads. Grouped together to improve
/// cache usage by avoiding loading all of the data for a quad_t.
struct qsdata_t
{
    uint32_t LayerDepth;        /// The layer depth of the sprite, increasing into the background.
    uint32_t RenderState;       /// The render state associated with the sprite.
};

/// @summary A structure for buffering data associated with a set of sprites.
struct sprite_batch_t
{
    size_t    Count;            /// The number of buffered sprites.
    size_t    Capacity;         /// The capacity of the various buffers.
    quad_t   *Quads;            /// Buffer for transformed quad data.
    qsdata_t *State;            /// Render state identifiers for each quad.
    uint32_t *Order;            /// Insertion order values for each quad.
};

/// @summary A structure storing all of the data required to render sprites
/// using a particular effect. All of the shader state is maintained externally.
struct sprite_effect_t
{
    size_t    VertexCapacity;   /// The maximum number of vertices we can buffer.
    size_t    VertexOffset;     /// Current offset (in vertices) in buffer.
    size_t    VertexSize;       /// Size of one vertex, in bytes.
    size_t    IndexCapacity;    /// The maximum number of indices we can buffer.
    size_t    IndexOffset;      /// Current offset (in indices) in buffer.
    size_t    IndexSize;        /// Size of one index, in bytes.
    uint32_t  CurrentState;     /// The active render state identifier.
    GLuint    VertexArray;      /// The VAO describing the vertex layout.
    GLuint    VertexBuffer;     /// Buffer object for dynamic vertex data.
    GLuint    IndexBuffer;      /// Buffer object for dynamic index data.
    GLboolean BlendEnabled;     /// GL_TRUE if blending is enabled.
    GLenum    BlendSourceColor; /// The source color blend factor.
    GLenum    BlendSourceAlpha; /// The source alpha blend factor.
    GLenum    BlendTargetColor; /// The destination color blend factor.
    GLenum    BlendTargetAlpha; /// The destination alpha blend factor.
    GLenum    BlendFuncColor;   /// The color channel blend function.
    GLenum    BlendFuncAlpha;   /// The alpha channel blend function.
    GLfloat   BlendColor[4];    /// RGBA constant blend color.
    float     Projection[16];   /// Projection matrix for current viewport
};

/// @summary Signature for a function used to apply render state for an effect
/// prior to rendering any quads. The function should perform operations like
/// setting the active program, calling sprite_effect_bind_buffers() and
/// sprite_effect_apply_blendstate(), and so on.
/// @param effect The effect being used for rendering.
/// @param context Opaque data passed by the application.
typedef void (*sprite_effect_setup_fn)(sprite_effect_t *effect, void *context);

/// @summary Signature for a function used to apply render state for a quad primitive.
/// The function should perform operations like setting up samplers, uniforms, and so on.
/// @param effect The effect being used for rendering.
/// @param render_state The application render state identifier.
/// @param context Opaque data passed by the application.
typedef void (*sprite_effect_apply_fn)(sprite_effect_t *effect, uint32_t render_state, void *context);

/// @summary Generates and uploads vertex and index data for a batch of quads
/// to the vertex and index buffers of an effect. The buffers act as circular
/// buffers. If the end of the buffers is reached, as much data as possible is
/// buffered, and the function returns.
/// @param effect The effect to update.
/// @param batch The source sprite batch.
/// @param quad_offset The offset, in quads, of the first quad to read.
/// @param quad_count The number of quads to read.
/// @param base_index On return, this address is updated with the offset, in
/// indices, of the first buffered primitive written to the index buffer.
/// @return The number of quads actually buffered. May be less than @a count.
size_t sprite_effect_buffer_data_ptc(
    sprite_effect_t *effect,
    sprite_batch_t  *batch,
    size_t           quad_offset,
    size_t           quad_count,
    size_t          *base_index);

/// @summary Wraps a set of function pointers used to apply effect-specific state.
struct sprite_effect_apply_t
{
    sprite_effect_setup_fn SetupEffect;
    sprite_effect_apply_fn ApplyState;
};

/*//////////////
//  Functors  //
//////////////*/
/// @summary Functor used for sorting sprites into back-to-front order.
struct back_to_front
{
    sprite_batch_t *batch;

    inline back_to_front(sprite_batch_t *sprite_batch)
        :
        batch(sprite_batch)
    { /* empty */ }

    inline bool operator()(uint32_t ia, uint32_t ib)
    {
        qsdata_t const &sdata_a = batch->State[ia];
        qsdata_t const &sdata_b = batch->State[ib];
        if (sdata_a.LayerDepth  > sdata_b.LayerDepth)  return true;
        if (sdata_a.LayerDepth  < sdata_b.LayerDepth)  return false;
        if (sdata_a.RenderState < sdata_b.RenderState) return true;
        if (sdata_a.RenderState > sdata_b.RenderState) return false;
        return  (ia < ib);
    }
};

/// @summary Functor used for sorting sprites into front-to-back order.
struct front_to_back
{
    sprite_batch_t *batch;

    inline front_to_back(sprite_batch_t *sprite_batch)
        :
        batch(sprite_batch)
    { /* empty */ }

    inline bool operator()(uint32_t ia, uint32_t ib)
    {
        qsdata_t const &sdata_a = batch->State[ia];
        qsdata_t const &sdata_b = batch->State[ib];
        if (sdata_a.LayerDepth  < sdata_b.LayerDepth)  return true;
        if (sdata_a.LayerDepth  > sdata_b.LayerDepth)  return false;
        if (sdata_a.RenderState < sdata_b.RenderState) return true;
        if (sdata_a.RenderState > sdata_b.RenderState) return false;
        return  (ia > ib);
    }
};

/// @summary Functor used for sorting sprites by render state.
struct by_render_state
{
    sprite_batch_t *batch;

    inline by_render_state(sprite_batch_t *sprite_batch)
        :
        batch(sprite_batch)
    { /* empty */ }

    inline bool operator()(uint32_t ia, uint32_t ib)
    {
        qsdata_t const &sdata_a = batch->State[ia];
        qsdata_t const &sdata_b = batch->State[ib];
        if (sdata_a.RenderState < sdata_b.RenderState) return true;
        if (sdata_a.RenderState > sdata_b.RenderState) return false;
        return  (ia < ib);
    }
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Initializes a sprite batch with the specified capacity.
/// @param batch The sprite batch.
/// @param capacity The initial capacity of the batch, in quads.
void create_sprite_batch(sprite_batch_t *batch, size_t capacity);

/// @summary Frees the memory associated with a sprite batch.
/// @param batch The sprite batch to free.
void delete_sprite_batch(sprite_batch_t *batch);

/// @summary Ensures that the sprite batch has at least the specified capacity.
/// @param batch The sprite batch.
void ensure_sprite_batch(sprite_batch_t *batch, size_t capacity);

/// @summary Discards data buffered by a sprite batch.
/// @param batch The sprite batch to flush.
void flush_sprite_batch(sprite_batch_t *batch);

/// @summary Sorts a sprite batch using std::sort(). The sort is indirect; the
/// order array is what gets sorted. The order array can then be used to read
/// quad definitions from the batch in sorted order.
/// @param batch The sprite batch to sort.
template <typename TComp>
inline void sort_sprite_batch(sprite_batch_t *batch)
{
    TComp cmp(batch);
    std::sort(batch->Order, batch->Order + batch->Count, cmp);
}

/// @summary Transforms a set of sprite definitions into a series of quad definitions.
/// @param quads The buffer of quads to write to.
/// @param sdata The buffer of state data to write to.
/// @param indices The buffer of order indices to write to.
/// @param quad_offset The zero-based index of the first quad to write.
/// @param sprites The buffer of sprite definitions to read from.
/// @param sprite_offset The zero-based index of the first sprite to read.
/// @param count The number of sprite definitions to read.
void generate_quads(
    quad_t         *quads,
    qsdata_t       *sdata,
    uint32_t       *indices,
    size_t          quad_offset,
    sprite_t const *sprites,
    size_t          sprite_offset,
    size_t          sprite_count);

/// @summary Generates transformed position-texture-color vertex data for a set, or subset, of quads.
/// @param buffer The buffer to which vertex data will be written.
/// @param buffer_offset The offset into the buffer of the first vertex.
/// @param quads The buffer from which quad attributes will be read.
/// @param indices An array of zero-based indices specifying the order in which to read quads from the quad buffer.
/// @param quad_offset The offset into the quad list of the first quad.
/// @param quad_count The number of quads to generate.
void generate_quad_vertices_ptc(
    void           *buffer,
    size_t          buffer_offset,
    quad_t const   *quads,
    uint32_t const *indices,
    size_t          quad_offset,
    size_t          quad_count);

/// @summary Generates index data for a set, or subset, of quads. Triangles are
/// specified using counter-clockwise winding. Indices are 16-bit unsigned int.
/// @param buffer The destination buffer.
/// @param offset The offset into the buffer of the first index.
/// @param base_vertex The zero-based index of the first vertex of the batch.
/// This allows multiple batches to write into the same index buffer.
/// @param quad_count The number of quads being generated.
void generate_quad_indices_u16(void *buffer, size_t offset, size_t base_vertex, size_t quad_count);

/// @summary Generates index data for a set, or subset, of quads. Triangles are
/// specified using counter-clockwise winding. Indices are 32-bit unsigned int.
/// @param buffer The destination buffer.
/// @param offset The offset into the buffer of the first index.
/// @param base_vertex The zero-based index of the first vertex of the batch.
/// This allows multiple batches to write into the same index buffer.
/// @param quad_count The number of quads being generated.
void generate_quad_indices_u32(void *buffer, size_t offset, size_t base_vertex, size_t quad_count);

/// @summary Creates the GPU resources required to buffer and render quads.
/// @param effect The effect to initialize.
/// @param quad_count The maximum number of quads that can be buffered.
/// @param vertex_size The size of a single vertex, in bytes.
/// @param index_size The size of a single index, in bytes.
bool create_sprite_effect(sprite_effect_t *effect, size_t quad_count, size_t vertex_size, size_t index_size);

/// @summary Releases the GPU resources used for buffering and rendering quads.
/// @param effect The effect to destroy.
void delete_sprite_effect(sprite_effect_t *effect);

/// @summary Disables alpha blending for an effect. The state changes do not
/// take effect until the effect is (re)bound.
/// @param effect The effect to update.
void sprite_effect_blend_none(sprite_effect_t *effect);

/// @summary Enables standard alpha blending (texture transparency) for an
/// effect. The state changes do not take effect until the effect is (re)bound.
/// @param effect The effect to update.
void sprite_effect_blend_alpha(sprite_effect_t *effect);

/// @summary Enables additive alpha blending for an effect. The state changes
/// do not take effect until the effect is (re)bound.
/// @param effect The effect to update.
void sprite_effect_blend_additive(sprite_effect_t *effect);

/// @summary Enables alpha blending with premultiplied alpha in the source texture.
/// The state changes do not take effect until the effect is (re)bound.
/// @param effect The effect to update.
void sprite_effect_blend_premultiplied(sprite_effect_t *effect);

/// @summary Sets up the effect projection matrix for the given viewport.
/// @param effect The effect to update.
/// @param width The viewport width.
/// @param height The viewport height.
void sprite_effect_set_viewport(sprite_effect_t *effect, int width, int height);

/// @summary Binds the vertex and index buffers of an effect for use in
/// subsequent rendering commands.
/// @param effect The effect being applied.
void sprite_effect_bind_buffers(sprite_effect_t *effect);

/// @summary Applies the alpha blending state specified by the effect.
/// @param effect The effect being applied.
void sprite_effect_apply_blendstate(sprite_effect_t *effect);

/// @summary Configures the Vertex Array Object for an effect using the standard
/// Position-TexCoord-Color layout configuration.
void sprite_effect_setup_vao_ptc(sprite_effect_t *effect);

/// @summary Generates and uploads vertex and index data for a batch of quads
/// to the vertex and index buffers of an effect. The buffers act as circular
/// buffers. If the end of the buffers is reached, as much data as possible is
/// buffered, and the function returns.
/// @param effect The effect to update.
/// @param quads The source quad definitions.
/// @param indices An array of zero-based indices specifying the order in which to read quads from the quad buffer.
/// @param quad_offset The offset, in quads, of the first quad to read.
/// @param quad_count The number of quads to read.
/// @param base_index On return, this address is updated with the offset, in
/// indices, of the first buffered primitive written to the index buffer.
/// @return The number of quads actually buffered. May be less than @a count.
size_t sprite_effect_buffer_data_ptc(
    sprite_effect_t *effect,
    quad_t const    *quads,
    uint32_t const  *indices,
    size_t           quad_offset,
    size_t           quad_count,
    size_t          *base_index);

/// @summary Renders an entire sprite batch with a given effect.
/// @param effect The effect being applied.
/// @param batch The sprite batch being rendered.
/// @param fxfuncs The effect-specific functions for applying render state.
/// @param context Opaque data defined by the application.
void sprite_effect_draw_batch_ptc(
    sprite_effect_t             *effect,
    sprite_batch_t              *batch,
    sprite_effect_apply_t const *fxfuncs,
    void                        *context);

/// @summary Renders a portion of a sprite batch for which the vertex and index
/// data has already been buffered. This function is generally not called by
/// the user directly; it is called internally from sprite_effect_draw_batch_ptc().
/// @param effect The effect being applied.
/// @param batch The sprite batch being rendered.
/// @param quad_offset The index of the first quad in the batch to be rendered.
/// @param quad_count The number of quads to draw.
/// @param base_index The first index to read from the index buffer.
void sprite_effect_draw_batch_region_ptc(
    sprite_effect_t             *effect,
    sprite_batch_t              *batch,
    size_t                       quad_offset,
    size_t                       quad_count,
    size_t                       base_index,
    sprite_effect_apply_t const *fxfuncs,
    void                        *context);

#endif /* !defined(GL_SPRITE_HPP) */
