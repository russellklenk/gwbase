/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the low-level interface to the audio system, which is
/// built on top of the abstraction provided by OpenAL.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include "ll_audio.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*//////////////////
//   Data Types   //
//////////////////*/
/// @summary State data associated with the audio device.
struct audio_device_t
{
    ALCdevice  *Device;   /// The OpenAL device object.
    ALCcontext *Context;  /// The OpenAL rendering context.
};

/*///////////////
//   Globals   //
///////////////*/
/// @summary The global audio device object.
static audio_device_t gAudioDevice = {
    NULL,
    NULL
};

/*///////////////////////
//   Local Functions   //
///////////////////////*/
/// @summary Converts a channel count/bits per-sample pair to the corresponding OpenAL value.
/// @param channel_count The number of channels of sample data, either 1 or 2.
/// @param bits_per_sample The number of bits per-sample, either 8 or 16.
/// @return The corresponding OpenAL format enumeration value.
static ALenum al_format(size_t channel_count, size_t bits_per_sample)
{
    switch (bits_per_sample)
    {
        case 16:
            return channel_count > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        case 8:
            return channel_count > 1 ? AL_FORMAT_STEREO8  : AL_FORMAT_MONO8;
        default:
            break;
    }
    return (ALenum) -1;
}

/*///////////////////////
//  Public Functions   //
///////////////////////*/
bool audio_open_device(void)
{
    if (gAudioDevice.Device != NULL)
        return true;

    ALCdevice *dev = alcOpenDevice(NULL);
    if (dev == NULL)
    {
        return false;
    }

    ALCcontext *ctx = alcCreateContext(dev, NULL);
    if (ctx == NULL)
    {
        alcCloseDevice(dev);
        return false;
    }

    // save the device and context.
    alcMakeContextCurrent(ctx);
    gAudioDevice.Device  = dev;
    gAudioDevice.Context = ctx;
    alGetError(); // clear the error state.

    // configure the default listener.
    ALfloat orientation[] = {
        0.0f, 0.0f, -1.0f, /* at vector */
        0.0f, 1.0f,  0.0f  /* up vector */
    };
    alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f);
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, orientation);
    return true;
}

void audio_close_device(void)
{
    if (gAudioDevice.Device != NULL)
    {
        alcMakeContextCurrent(NULL);
        alcDestroyContext(gAudioDevice.Context);
        alcCloseDevice(gAudioDevice.Device);
        gAudioDevice.Device  = NULL;
        gAudioDevice.Context = NULL;
    }
}

bool create_sound_buffer(sound_buffer_t *buffer, size_t channel_count, size_t sample_rate, size_t bits_per_sample, float duration)
{
    if (buffer)
    {
        ALuint id     = 0;
        ALenum format = al_format(channel_count, bits_per_sample);
        size_t bps    = bits_per_sample * sample_rate * channel_count;
        size_t nbytes = size_t(ceil(duration * (bps / 8)));

        alGenBuffers(1, &id);
        if (id == 0) return false;

        buffer->Id            = id;
        buffer->Format        = format;
        buffer->ChannelCount  = channel_count;
        buffer->SampleRate    = sample_rate;
        buffer->BitsPerSample = bits_per_sample;
        buffer->DataSize      = nbytes;
        buffer->Duration      = duration;
        return true;
    }
    else return false;
}

void delete_sound_buffer(sound_buffer_t *buffer)
{
    if (buffer && buffer->Id)
    {
        alDeleteBuffers(1, &buffer->Id);
        buffer->Id = 0;
    }
}

void buffer_sound_data(sound_buffer_t *buffer, void const *data, size_t amount)
{
    alBufferData(buffer->Id, buffer->Format, data, amount, buffer->SampleRate);
}

bool create_sound_source(sound_source_t *source)
{
    if (source)
    {
        ALuint id = 0;

        alGenSources(1, &id);
        if (id == 0) return false;

        source->Id          = id;
        source->Buffer      = 0;
        source->Loop        = AL_FALSE;
        source->Gain        = 1.0f;
        source->Pitch       = 1.0f;
        source->Position[0] = 0.0f;
        source->Position[1] = 0.0f;
        source->Velocity[0] = 0.0f;
        source->Velocity[1] = 0.0f;
        return true;
    }
    else return false;
}

void delete_sound_source(sound_source_t *source)
{
    if (source && source->Id)
    {
        alSourcei(source->Id, AL_BUFFER, 0);
        alDeleteSources(1, &source->Id);
        source->Id    = 0;
        source->Buffer= 0;
    }
}

void play_sound(sound_source_t *source, sound_buffer_t *buffer)
{
    float pos[] = { source->Position[0], source->Position[1], 0.0f };
    float vel[] = { source->Velocity[0], source->Velocity[1], 0.0f };
    alSourcef(source->Id , AL_GAIN     , source->Gain);
    alSourcef(source->Id , AL_PITCH    , source->Pitch);
    alSourcei(source->Id , AL_LOOPING  , source->Loop);
    alSourcefv(source->Id, AL_POSITION , pos);
    alSourcefv(source->Id, AL_VELOCITY , vel);
    alSourcei(source->Id , AL_BUFFER   , buffer->Id);
    alSourcePlay(source->Id);
}
