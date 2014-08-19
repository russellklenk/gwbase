/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the low-level interface to the audio system, which is
/// built on top of the abstraction provided by OpenAL.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef LL_AUDIO_HPP
#define LL_AUDIO_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary The number of channels in a mono audio source.
#define AUDIO_MONO      1U

/// @summary The number of channels in a stereo audio source.
#define AUDIO_STEREO    2U

/// @summary Represents a sound buffer. Currently, only static sounds are supported.
struct sound_buffer_t
{
    ALuint Id;              /// The unique identifier of the buffer.
    ALenum Format;          /// AL_FORMAT_MONO16, AL_FORMAT_STEREO_16, etc.
    size_t ChannelCount;    /// The number of channels (1 = mono, 2 = stereo.)
    size_t SampleRate;      /// The sample playback rate.
    size_t BitsPerSample;   /// The number of bits in a single sample value.
    size_t DataSize;        /// The number of bytes allocated to the buffer.
    float  Duration;        /// The number of seconds of data the buffer can store.
};

/// @summary Represents a sound source. The sound source can move throughout the scene.
struct sound_source_t
{
    ALuint    Id;           /// The unique identifier of the source.
    ALuint    Buffer;       /// The buffer currently bound to the source.
    ALboolean Loop;         /// Whether to loop the sound.
    float     Gain;         /// The gain value.
    float     Pitch;        /// The pitch value.
    float     Position[2];  /// The source position.
    float     Velocity[2];  /// The source velocity.
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Initializes the audio subsystem, opens the default device and
/// initializes the default listener.
/// @return true if the default device was opened.
bool audio_open_device(void);

/// @summary Closes the default audio device.
void audio_close_device(void);

/// @summary Creates a new sound buffer.
/// @param buffer The sound buffer to initialize.
/// @param channel_count The number of channels.
/// @param sample_rate The desired sample rate, in Hz.
/// @param bits_per_sample The number of bits used for a single sample value.
/// @param duration The number of seconds of sample data that can be buffered.
/// @return true if the sound buffer was initialized successfully.
bool create_sound_buffer(sound_buffer_t *buffer, size_t channel_count, size_t sample_rate, size_t bits_per_sample, float duration);

/// @summary Frees the storage associated with a sound buffer object.
/// @param buffer The sound buffer to free.
void delete_sound_buffer(sound_buffer_t *buffer);

/// @summary Uploads sample data into a sound buffer.
/// @param buffer The destination buffer object.
/// @param data The sample data to upload.
/// @param amount The number of bytes of sample data to buffer.
void buffer_sound_data(sound_buffer_t *buffer, void const *data, size_t amount);

/// @summary Creates a new sound source.
/// @param source The source to initialize.
/// @return true if the sound source is initialized.
bool create_sound_source(sound_source_t *source);

/// @summary Deletes a sound source.
/// @param The sound source to delete.
void delete_sound_source(sound_source_t *source);

/// @param Plays a sound.
/// @param source The sound source attributes.
/// @param buffer The buffer containing the sample data to play.
void play_sound(sound_source_t *source, sound_buffer_t *buffer);

#endif /* !defined(LL_AUDIO_HPP) */
