/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the interface to a loader for uncompressed audio data
/// stored in the WAV format. It is assumed that both the host system and
/// input data are little-endian. Currently, streaming is not supported.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef FF_WAV_HPP
#define FF_WAV_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Defines the recognized compression types.
enum wav_compression_type_e
{
    WAV_COMPRESSION_UNKNOWN      = 0x0000,
    WAV_COMPRESSION_PCM          = 0x0001,
    WAV_COMPRESSION_ADPCM        = 0x0002,
    WAV_COMPRESSION_MPEG         = 0x0050,
    WAV_COMPRESISON_EXPERIMENTAL = 0xFFFF
};

#pragma pack(push, 1)
/// @summary Define the RIFF header that appears at the start of the file.
struct riff_header_t
{
    uint32_t ChunkId;         /// 'RIFF' (0x52494646)
    uint32_t DataSize;        /// The file size, minus 8 (for the header).
    uint32_t RiffType;        /// The file type, 'WAVE' (0x57415645)
};

/// @summary Define the header that appears at the start of each chunk.
/// Note that chunk headers start on even addresses only.
struct riff_chunk_header_t
{
    uint32_t ChunkId;         /// Varies; 'fmt ' (0x666D7420) and 'data' (0x64617461)
    uint32_t DataSize;        /// The size of the chunk data, not including the header.
};

/// @summary Define the data comprising the WAV format chunk, used to describe sample data.
struct wav_format_chunk_t
{
    uint16_t CompressionType; /// One of wav_compression_type_e.
    uint16_t ChannelCount;    /// Number of channels (1 = mono, 2 = stereo).
    uint32_t SampleRate;      /// Number of samples per-second.
    uint32_t BytesPerSecond;  /// Average number of bytes per-second for streaming.
    uint16_t BlockAlignment;  /// Number of bytes per-sample.
    uint16_t BitsPerSample;   /// Number of bytes per-sample.
    uint16_t FormatDataSize;  /// Size of the format-specific data, in bytes.
    uint8_t  FormatData[1];   /// Optional extra format-specific data.
};
#pragma pack(pop)

/*///////////////
//  Functions  //
///////////////*/
/// @summary Inspect the contents of a buffer representing a well-formed WAV
/// audio file and report various attributes about the audio file.
/// @param data The buffer containing the WAV data.
/// @param data_size The size of the WAV file data, in bytes.
/// @param out_format_offset On return, this location is updated with the byte
/// offset of the start of the WAV fmt chunk.
/// @param out_data_offset On return, this location is updated with the byte
/// offset of the start of the WAV sample data chunk.
/// @param out_data_size On return, this location is updated with the size of
/// the sample data, in bytes.
/// @param out_duration On return, this location is updated with the duration of
/// the audio data, in seconds.
/// @param out_format_chunk On return, this location is updated with the contents
/// of the WAV fmt chunk.
/// @return true if the WAV is a supported format.
bool wav_describe(
    void const *data,
    size_t      data_size,
    size_t     *out_format_offset,
    size_t     *out_data_offset,
    size_t     *out_data_size,
    float      *out_duration,
    wav_format_chunk_t *out_format_chunk);

/// @summary Retrieve a pointer to the sample data stored within a WAV file.
/// @param data The buffer containing the WAV data.
/// @param data_chunk_offset The byte offset of the data chunk.
/// @return A pointer to the raw sample data.
void* wav_sample_data(void const *data, size_t data_chunk_offset);

#endif /* !defined(FF_WAV_HPP) */
