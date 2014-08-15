/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a series of transforms for generating screenspace quads
/// representing sprite data to be streamed to the GPU each frame. Sprites are
/// transformed on the CPU and written to a dynamic vertex buffer each frame.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "glsprite.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
void create_sprite_batch(sprite_batch_t *batch, size_t capacity)
{
    if (batch)
    {
        batch->Count    = 0;
        batch->Capacity = capacity;
        if (capacity)
        {
            batch->Quads = (quad_t  *) malloc(capacity * sizeof(quad_t));
            batch->State = (qsdata_t*) malloc(capacity * sizeof(qsdata_t));
            batch->Order = (uint32_t*) malloc(capacity * sizeof(uint32_t));
        }
        else
        {
            batch->Quads = NULL;
            batch->State = NULL;
            batch->Order = NULL;
        }
    }
}

void delete_sprite_batch(sprite_batch_t *batch)
{
    if (batch)
    {
        if (batch->Capacity)
        {
            free(batch->Order);
            free(batch->State);
            free(batch->Quads);
        }
        batch->Count    = 0;
        batch->Capacity = 0;
        batch->Quads    = NULL;
        batch->State    = NULL;
        batch->Order    = NULL;
    }
}

// need to pass batch->Count + NumToAdd as capacity
void ensure_sprite_batch(sprite_batch_t *batch, size_t capacity)
{
    if (batch->Capacity < capacity)
    {
        batch->Capacity = capacity;
        batch->Quads    = (quad_t  *) realloc(batch->Quads, capacity * sizeof(quad_t));
        batch->State    = (qsdata_t*) realloc(batch->State, capacity * sizeof(qsdata_t));
        batch->Order    = (uint32_t*) realloc(batch->Order, capacity * sizeof(uint32_t));
    }
}

void flush_sprite_batch(sprite_batch_t *batch)
{
    batch->Count = 0;
}

void generate_quads(quad_t *quads, qsdata_t *sdata, uint32_t *indices, size_t quad_offset, sprite_t const *sprites, size_t sprite_offset, size_t sprite_count)
{
    size_t qindex = quad_offset;
    size_t sindex = sprite_offset;
    for (size_t i = 0;  i < sprite_count; ++i, ++qindex, ++sindex)
    {
        sprite_t const &s = sprites[sindex];
        qsdata_t       &r = sdata[qindex];
        quad_t         &q = quads[qindex];

        q.Source[0]   = s.ImageX;
        q.Source[1]   = s.ImageY;
        q.Source[2]   = s.ImageWidth;
        q.Source[3]   = s.ImageHeight;
        q.Target[0]   = s.ScreenX;
        q.Target[1]   = s.ScreenY;
        q.Target[2]   = s.ImageWidth  * s.ScaleX;
        q.Target[3]   = s.ImageHeight * s.ScaleY;
        q.Origin[0]   = s.OriginX;
        q.Origin[1]   = s.OriginY;
        q.Scale [0]   = 1.0f / s.TextureWidth;
        q.Scale [1]   = 1.0f / s.TextureHeight;
        q.Orientation = s.Orientation;
        q.TintColor   = s.TintColor;

        r.LayerDepth  = s.LayerDepth;
        r.RenderState = s.RenderState;

        indices[qindex] = qindex;
    }
}

void generate_quad_vertices_ptc(
    void           *buffer,
    size_t          buffer_offset,
    quad_t const   *quads,
    uint32_t const *indices,
    size_t          quad_offset,
    size_t          quad_count)
{
    static const size_t X      =  0;
    static const size_t Y      =  1;
    static const size_t W      =  2;
    static const size_t H      =  3;
    static const float  XCO[4] = {0.0f, 1.0f, 1.0f, 0.0f};
    static const float  YCO[4] = {0.0f, 0.0f, 1.0f, 1.0f};

    sprite_vertex_ptc_t   *vertex_buffer = (sprite_vertex_ptc_t*) buffer;
    size_t                 vertex_offset = buffer_offset;
    for (size_t i = 0; i < quad_count; ++i)
    {
        // figure out which quad we're working with.
        const uint32_t  id    = indices[quad_offset + i];
        quad_t const   &quad  = quads[id];

        // pre-calculate values constant across the quad.
        const float     src_x = quad.Source[X];
        const float     src_y = quad.Source[Y];
        const float     src_w = quad.Source[W];
        const float     src_h = quad.Source[H];
        const float     dst_x = quad.Target[X];
        const float     dst_y = quad.Target[Y];
        const float     dst_w = quad.Target[W];
        const float     dst_h = quad.Target[H];
        const float     ctr_x = quad.Origin[X] / src_w;
        const float     ctr_y = quad.Origin[Y] / src_h;
        const float     scl_u = quad.Scale[X];
        const float     scl_v = quad.Scale[Y];
        const float     angle = quad.Orientation;
        const uint32_t  color = quad.TintColor;
        const float     sin_o = sinf(angle);
        const float     cos_o = cosf(angle);

        // calculate values that change per-vertex.
        for (size_t j = 0; j < 4; ++j)
        {
            sprite_vertex_ptc_t &vert = vertex_buffer[vertex_offset++];
            float ofs_x    = XCO[j];
            float ofs_y    = YCO[j];
            float x_dst    = (ofs_x - ctr_x)  *  dst_w;
            float y_dst    = (ofs_y - ctr_y)  *  dst_h;
            vert.XYUV[0]   = (dst_x + (x_dst * cos_o)) - (y_dst * sin_o);
            vert.XYUV[1]   = (dst_y + (x_dst * sin_o)) + (y_dst * cos_o);
            vert.XYUV[2]   = (src_x + (ofs_x * src_w)) *  scl_u;
            vert.XYUV[3]   = 1.0f - ((src_y + (ofs_y * src_h)) *  scl_v);
            vert.TintColor = color;
        }
    }
}

void generate_quad_indices_u16(void *buffer, size_t offset, size_t base_vertex, size_t quad_count)
{
    uint16_t *u16 = (uint16_t*) buffer;
    uint16_t  b16 = (uint16_t ) base_vertex;
    for (size_t i = 0; i < quad_count; ++i)
    {
        u16[offset++]  = (b16 + 1);
        u16[offset++]  = (b16 + 0);
        u16[offset++]  = (b16 + 2);
        u16[offset++]  = (b16 + 2);
        u16[offset++]  = (b16 + 0);
        u16[offset++]  = (b16 + 3);
        b16 += 4;
    }
}

void generate_quad_indices_u32(void *buffer, size_t offset, size_t base_vertex, size_t quad_count)
{
    uint32_t *u32 = (uint32_t*) buffer;
    uint32_t  b32 = (uint32_t ) base_vertex;
    for (size_t i = 0; i < quad_count; ++i)
    {
        u32[offset++]  = (b32 + 1);
        u32[offset++]  = (b32 + 0);
        u32[offset++]  = (b32 + 2);
        u32[offset++]  = (b32 + 2);
        u32[offset++]  = (b32 + 0);
        u32[offset++]  = (b32 + 3);
        b32 += 4;
    }
}

bool create_sprite_effect(sprite_effect_t *effect, size_t quad_count, size_t vertex_size, size_t index_size)
{
    assert(effect     != NULL);
    assert(quad_count  > 0);
    assert(vertex_size > 0);
    assert(index_size  > 0);

    GLuint  vao        =  0;
    GLuint  buffers[2] = {0, 0};
    size_t  vcount     = quad_count  * 4;
    size_t  icount     = quad_count  * 6;
    GLsizei abo_size   = vertex_size * vcount;
    GLsizei eao_size   = index_size  * icount;

    // @todo: error handling.
    glGenBuffers(2, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, abo_size, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eao_size, NULL, GL_DYNAMIC_DRAW);
    glGenVertexArrays(1, &vao);

    effect->VertexCapacity   = vcount;
    effect->VertexOffset     = 0;
    effect->VertexSize       = vertex_size;
    effect->IndexCapacity    = icount;
    effect->IndexOffset      = 0;
    effect->IndexSize        = index_size;
    effect->CurrentState     = 0xFFFFFFFFU;
    effect->VertexArray      = vao;
    effect->VertexBuffer     = buffers[0];
    effect->IndexBuffer      = buffers[1];
    effect->BlendEnabled     = GL_FALSE;
    effect->BlendSourceColor = GL_ONE;
    effect->BlendSourceAlpha = GL_ONE;
    effect->BlendTargetColor = GL_ZERO;
    effect->BlendTargetAlpha = GL_ZERO;
    effect->BlendFuncColor   = GL_FUNC_ADD;
    effect->BlendFuncAlpha   = GL_FUNC_ADD;
    effect->BlendColor[0]    = 0.0f;
    effect->BlendColor[1]    = 0.0f;
    effect->BlendColor[2]    = 0.0f;
    effect->BlendColor[3]    = 0.0f;
    return true;
}

void delete_sprite_effect(sprite_effect_t *effect)
{
    GLuint buffers[2] = {
        effect->VertexBuffer,
        effect->IndexBuffer
    };
    glDeleteBuffers(2, buffers);
    glDeleteVertexArrays(1, &effect->VertexArray);
    effect->VertexCapacity = 0;
    effect->VertexOffset   = 0;
    effect->VertexSize     = 0;
    effect->IndexCapacity  = 0;
    effect->IndexOffset    = 0;
    effect->IndexSize      = 0;
    effect->VertexArray    = 0;
    effect->VertexBuffer   = 0;
    effect->IndexBuffer    = 0;
}

void sprite_effect_blend_none(sprite_effect_t *effect)
{
    effect->BlendEnabled     = GL_FALSE;
    effect->BlendSourceColor = GL_ONE;
    effect->BlendSourceAlpha = GL_ONE;
    effect->BlendTargetColor = GL_ZERO;
    effect->BlendTargetAlpha = GL_ZERO;
    effect->BlendFuncColor   = GL_FUNC_ADD;
    effect->BlendFuncAlpha   = GL_FUNC_ADD;
    effect->BlendColor[0]    = 0.0f;
    effect->BlendColor[1]    = 0.0f;
    effect->BlendColor[2]    = 0.0f;
    effect->BlendColor[3]    = 0.0f;
}

void sprite_effect_blend_alpha(sprite_effect_t *effect)
{
    effect->BlendEnabled     = GL_TRUE;
    effect->BlendSourceColor = GL_SRC_COLOR;
    effect->BlendSourceAlpha = GL_SRC_ALPHA;
    effect->BlendTargetColor = GL_ONE_MINUS_SRC_ALPHA;
    effect->BlendTargetAlpha = GL_ONE_MINUS_SRC_ALPHA;
    effect->BlendFuncColor   = GL_FUNC_ADD;
    effect->BlendFuncAlpha   = GL_FUNC_ADD;
    effect->BlendColor[0]    = 0.0f;
    effect->BlendColor[1]    = 0.0f;
    effect->BlendColor[2]    = 0.0f;
    effect->BlendColor[3]    = 0.0f;
}

void sprite_effect_blend_additive(sprite_effect_t *effect)
{
    effect->BlendEnabled     = GL_TRUE;
    effect->BlendSourceColor = GL_SRC_COLOR;
    effect->BlendSourceAlpha = GL_SRC_ALPHA;
    effect->BlendTargetColor = GL_ONE;
    effect->BlendTargetAlpha = GL_ONE;
    effect->BlendFuncColor   = GL_FUNC_ADD;
    effect->BlendFuncAlpha   = GL_FUNC_ADD;
    effect->BlendColor[0]    = 0.0f;
    effect->BlendColor[1]    = 0.0f;
    effect->BlendColor[2]    = 0.0f;
    effect->BlendColor[3]    = 0.0f;
}

void sprite_effect_blend_premultiplied(sprite_effect_t *effect)
{
    effect->BlendEnabled     = GL_TRUE;
    effect->BlendSourceColor = GL_ONE;
    effect->BlendSourceAlpha = GL_ONE;
    effect->BlendTargetColor = GL_ONE_MINUS_SRC_ALPHA;
    effect->BlendTargetAlpha = GL_ONE_MINUS_SRC_ALPHA;
    effect->BlendFuncColor   = GL_FUNC_ADD;
    effect->BlendFuncAlpha   = GL_FUNC_ADD;
    effect->BlendColor[0]    = 0.0f;
    effect->BlendColor[1]    = 0.0f;
    effect->BlendColor[2]    = 0.0f;
    effect->BlendColor[3]    = 0.0f;
}

void sprite_effect_set_viewport(sprite_effect_t *effect, int width, int height)
{
    float *dst16 = effect->Projection;
    float  s_x = 1.0f / (width   * 0.5f);
    float  s_y = 1.0f / (height  * 0.5f);
    dst16[ 0]  = s_x ; dst16[ 1] = 0.0f; dst16[ 2] = 0.0f; dst16[ 3] = 0.0f;
    dst16[ 4]  = 0.0f; dst16[ 5] = -s_y; dst16[ 6] = 0.0f; dst16[ 7] = 0.0f;
    dst16[ 8]  = 0.0f; dst16[ 9] = 0.0f; dst16[10] = 1.0f; dst16[11] = 0.0f;
    dst16[12]  =-1.0f; dst16[13] = 1.0f; dst16[14] = 0.0f; dst16[15] = 1.0f;
}

void sprite_effect_bind_buffers(sprite_effect_t *effect)
{
    glBindVertexArray(effect->VertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, effect->VertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, effect->IndexBuffer);
}

void sprite_effect_apply_blendstate(sprite_effect_t *effect)
{
    if (effect->BlendEnabled)
    {
        glEnable(GL_BLEND);
        glBlendColor(effect->BlendColor[0], effect->BlendColor[1], effect->BlendColor[2], effect->BlendColor[3]);
        glBlendFuncSeparate(effect->BlendSourceColor, effect->BlendTargetColor, effect->BlendSourceAlpha, effect->BlendTargetAlpha);
        glBlendEquationSeparate(effect->BlendFuncColor, effect->BlendFuncAlpha);
    }
    else glDisable(GL_BLEND);
}

void sprite_effect_setup_vao_ptc(sprite_effect_t *effect)
{
    glBindVertexArray(effect->VertexArray);
    glEnableVertexAttribArray(SPRITE_PTC_LOCATION_PTX);
    glEnableVertexAttribArray(SPRITE_PTC_LOCATION_CLR);
    glVertexAttribPointer(SPRITE_PTC_LOCATION_PTX, 4, GL_FLOAT,         GL_FALSE, sizeof(sprite_vertex_ptc_t), (GLvoid const*)  0);
    glVertexAttribPointer(SPRITE_PTC_LOCATION_CLR, 4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(sprite_vertex_ptc_t), (GLvoid const*) 16);
}

size_t sprite_effect_buffer_data_ptc(
    sprite_effect_t *effect,
    quad_t const    *quads,
    uint32_t const  *indices,
    size_t           quad_offset,
    size_t           quad_count,
    size_t          *base_index_arg)
{
    if (effect->VertexOffset == effect->VertexCapacity)
    {
        // the buffer is completely full. time to discard it and
        // request a new buffer from the driver, to avoid stalls.
        GLsizei abo_size     = effect->VertexCapacity * effect->VertexSize;
        GLsizei eao_size     = effect->IndexCapacity  * effect->IndexSize;
        effect->VertexOffset = 0;
        effect->IndexOffset  = 0;
        glBufferData(GL_ARRAY_BUFFER, abo_size, NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, eao_size, NULL, GL_DYNAMIC_DRAW);
    }

    size_t num_indices  = quad_count * 6;
    size_t num_vertices = quad_count * 4;
    size_t max_vertices = effect->VertexCapacity;
    size_t base_vertex  = effect->VertexOffset;
    size_t vertex_size  = effect->VertexSize;
    size_t max_indices  = effect->IndexCapacity;
    size_t base_index   = effect->IndexOffset;
    size_t index_size   = effect->IndexSize;
    if (max_vertices < base_vertex + num_vertices)
    {
        // not enough space in the buffer to fit everything.
        // only a portion of the desired data will be buffered.
        num_vertices = max_vertices - base_vertex;
        num_indices  = max_indices  - base_index;
    }

    size_t buffer_count =  num_vertices / 4;
    if (buffer_count == 0) return 0;

    GLintptr   v_offset = base_vertex  * vertex_size;
    GLsizeiptr v_size   = num_vertices * vertex_size;
    GLbitfield v_access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
    GLvoid    *v_data   = glMapBufferRange(GL_ARRAY_BUFFER, v_offset, v_size, v_access);
    if (v_data != NULL)
    {
        generate_quad_vertices_ptc(v_data, 0, quads, indices, quad_offset, buffer_count);
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    GLintptr   i_offset = base_index  * index_size;
    GLsizeiptr i_size   = num_indices * index_size;
    GLbitfield i_access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
    GLvoid    *i_data   = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, i_offset, i_size, i_access);
    if (i_data != NULL)
    {
        if (index_size == sizeof(uint16_t)) generate_quad_indices_u16(i_data, 0, base_vertex, buffer_count);
        else generate_quad_indices_u32(i_data, 0, base_vertex, buffer_count);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    effect->VertexOffset += buffer_count * 4;
    effect->IndexOffset  += buffer_count * 6;
    *base_index_arg       = base_index;
    return buffer_count;
}

void sprite_effect_draw_batch_ptc(
    sprite_effect_t             *effect,
    sprite_batch_t              *batch,
    sprite_effect_apply_t const *fxfuncs,
    void                        *context)
{
    size_t quad_count = batch->Count;
    size_t quad_index = 0;
    size_t base_index = 0;
    size_t n          = 0;

    fxfuncs->SetupEffect(effect, context);
    effect->CurrentState = 0xFFFFFFFFU;

    while (quad_count > 0)
    {
        n = sprite_effect_buffer_data_ptc(effect, batch->Quads, batch->Order, quad_index, quad_count, &base_index);
        sprite_effect_draw_batch_region_ptc(effect, batch, quad_index, n, base_index, fxfuncs, context);
        base_index  = effect->IndexOffset;
        quad_index += n;
        quad_count -= n;
    }
}

void sprite_effect_draw_batch_region_ptc(
    sprite_effect_t             *effect,
    sprite_batch_t              *batch,
    size_t                       quad_offset,
    size_t                       quad_count,
    size_t                       base_index,
    sprite_effect_apply_t const *fxfuncs,
    void                        *context)
{
    #define GLPTR(x)  (GLvoid const*)(x)
    uint32_t state_0 = effect->CurrentState;
    uint32_t state_1 = effect->CurrentState;
    size_t   index   = 0; // index of start of sub-batch
    size_t   nquad   = 0; // count of quads in sub-batch
    size_t   nindex  = 0; // count of indices in sub-batch
    size_t   quad_id = 0; // quad insertion index
    GLsizei  size    = effect->IndexSize;
    GLenum   type    = size == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

    for (size_t i = 0; i < quad_count; ++i)
    {
        quad_id = batch->Order[quad_offset + i];
        state_1 = batch->State[quad_id].RenderState;
        if (state_1 != state_0)
        {
            // render the previous sub-batch with the current state,
            // as long as it has at least one quad in the sub-batch.
            if (i > index)
            {
                nquad  = i - index;  // the number of quads being submitted
                nindex = nquad * 6;  // the number of indices being submitted
                glDrawElements(GL_TRIANGLES, nindex, type, GLPTR(base_index * size));
                base_index += nindex;
            }
            // now apply the new state and start a new sub-batch.
            fxfuncs->ApplyState(effect, state_1, context);
            state_0 = state_1;
            index   = i;
        }
    }
    // submit the remainder of the sub-batch.
    nquad  = quad_count - index;
    nindex = nquad * 6;
    glDrawElements(GL_TRIANGLES, nindex, type, GLPTR(base_index * size));
    effect->CurrentState = state_1;
    #undef GLPTR
}
