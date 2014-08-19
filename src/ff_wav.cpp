/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the interface to a loader for uncompressed audio data
/// stored in the WAV format. It is assumed that both the host system and
/// input data are little-endian. Currently, streaming is not supported.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "ff_wav.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////
//   Globals   //
///////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/
/// @summary Generate a little-endian FOURCC.
/// @param name A four-character string.
/// @return The packed FOURCC.
static inline uint32_t chunk_id(char const *name)
{
    uint32_t a = uint32_t(name[0]);
    uint32_t b = uint32_t(name[1]) <<  8;
    uint32_t c = uint32_t(name[2]) << 16;
    uint32_t d = uint32_t(name[3]) << 24;
    return  (a | b | c | d);
}

/// @summary Locate a specific RIFF chunk within the file data.
/// @param start The starting point of the search. This address should be the
/// start of a RIFF chunk header.
/// @param end The pointer representing the end of the file or search space.
/// @param id The chunk identifier to search for.
/// @return A pointer to the start of the chunk header, or NULL.
static void* find_chunk(void const *start, void const *end, uint32_t id)
{
    size_t const  hsize = sizeof(riff_chunk_header_t);
    uint8_t const *iter = (uint8_t const*) start;
    while (iter < end)
    {
        riff_chunk_header_t const *head = (riff_chunk_header_t const*) iter;

        if (head->ChunkId == id)
            return (void*) iter;

        iter += hsize + head->DataSize; // move to the next chunk start
        if (size_t(iter) & 1) iter++;   // chunks start on an even address
    }
    return NULL;
}

/*///////////////////////
//  Public Functions   //
///////////////////////*/
bool wav_describe(void const *data, size_t data_size, size_t *out_format_offset, size_t *out_data_offset, size_t *out_data_size, float *out_duration, wav_format_chunk_t *out_format_chunk)
{
    riff_chunk_header_t const *dc = NULL;
    wav_format_chunk_t  *fmt  = NULL;
    riff_header_t const *riff = NULL;
    uint8_t const *format_ptr = NULL;
    uint8_t const *search_ptr = NULL;
    uint8_t const *data_ptr   = NULL;
    uint8_t const *base_ptr   = (uint8_t const *) data;
    uint8_t const *end_ptr    = (uint8_t const *) data + data_size;
    size_t         min_size   = sizeof(riff_header_t) + sizeof(riff_chunk_header_t) * 2 + sizeof(wav_format_chunk_t);
    float          duration   = 0.0f;

    if (data == NULL || data_size < min_size)
        goto wav_error;

    riff = (riff_header_t const*) base_ptr;
    if (riff->ChunkId != chunk_id("RIFF"))
        goto wav_error;
    if (riff->RiffType != chunk_id("WAVE"))
        goto wav_error;

    search_ptr = base_ptr + sizeof(riff_header_t);
    format_ptr = (uint8_t const*) find_chunk(search_ptr, end_ptr, chunk_id("fmt "));
    if (format_ptr == NULL)
        goto wav_error;

    fmt = (wav_format_chunk_t*) (format_ptr + sizeof(riff_chunk_header_t));
    if (fmt->CompressionType != WAV_COMPRESSION_PCM)
        goto wav_unsupported;

    data_ptr = (uint8_t const*) find_chunk(search_ptr, end_ptr, chunk_id("data"));
    if (data_ptr == NULL)
        goto wav_unsupported;

    dc = (riff_chunk_header_t const*) data_ptr;
    duration = float(dc->DataSize) / (fmt->SampleRate * fmt->ChannelCount * (fmt->BitsPerSample / 8));

    if (out_format_offset) *out_format_offset = size_t(format_ptr - base_ptr);
    if (out_data_offset) *out_data_offset = size_t(data_ptr - base_ptr);
    if (out_data_size) *out_data_size = size_t(dc->DataSize);
    if (out_duration) *out_duration = duration;
    if (out_format_chunk) *out_format_chunk = *fmt;
    return true;

wav_unsupported:
    if (out_format_offset) *out_format_offset = size_t(format_ptr - base_ptr);
    if (out_data_offset) *out_data_offset = 0;
    if (out_data_size) *out_data_size = 0;
    if (out_duration) *out_duration = 0.0f;
    if (out_format_chunk) *out_format_chunk = *fmt;
    return false;

wav_error:
    if (out_format_offset) *out_format_offset = 0;
    if (out_data_offset) *out_data_offset = 0;
    if (out_data_size) *out_data_size = 0;
    if (out_duration) *out_duration = 0.0f;
    return false;
}

void* wav_sample_data(void const *data, size_t data_chunk_offset)
{
    uint8_t *head_ptr = (uint8_t*) data + data_chunk_offset;
    return  (head_ptr + sizeof(riff_chunk_header_t));
}
