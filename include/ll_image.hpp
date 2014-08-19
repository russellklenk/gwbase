/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines some useful utility functions for working with OpenGL
/// pixel format enumerations and performing pixel transfer operations. The
/// implementation targets the OpenGL 3.3 core profile.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef LL_IMAGE_HPP
#define LL_IMAGE_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Macro to convert a byte offset into a pointer.
/// @param x The byte offset value.
/// @return The offset value, as a pointer.
#ifndef GL_BUFFER_OFFSET
#define GL_BUFFER_OFFSET(x)   ((GLvoid*)(((uint8_t*)NULL)+(x)))
#endif

/// @summary Describes a single level of an image in a mipmap chain.
struct level_desc_t
{
    size_t Index;           /// The mipmap level index (0=highest resolution)
    size_t Width;           /// The width of the level, in pixels
    size_t Height;          /// The height of the level, in pixels
    size_t Slices;          /// The number of slices in the level
    size_t BytesPerElement; /// The number of bytes per-channel or pixel
    size_t BytesPerRow;     /// The number of bytes per-row
    size_t BytesPerSlice;   /// The number of bytes per-slice
    GLenum Format;          /// The OpenGL internal format
    GLenum DataType;        /// The OpenGL element data type
    GLenum BaseFormat;      /// The OpenGL base format
};

/// @summary Describes a transfer of pixel data from the device (GPU) to the
/// host (CPU), using glReadPixels, glGetTexImage or glGetCompressedTexImage.
/// The Target[X/Y/Z] fields can be specified to have the call calculate the
/// offset from the TransferBuffer pointer, or can be set to 0 if the
/// TransferBuffer is pointing to the start of the target data. Note that for
/// texture images, the entire mip level is transferred, and so the TransferX,
/// TransferY, TransferWidth and TransferHeight fields are ignored. The fields
/// Target[Width/Height] describe dimensions of the target image, which may be
/// different than the dimensions of the region being transferred. Note that
/// the Format and DataType describe the target image.
struct pixel_transfer_d2h_t
{
    GLenum Target;          /// The image source, ex. GL_READ_FRAMEBUFFER or GL_TEXTURE_2D
    GLenum Format;          /// The desired pixel data format, ex. GL_BGRA
    GLenum DataType;        /// The desired pixel data type, ex. GL_UNSIGNED_INT_8_8_8_8_REV
    GLuint PackBuffer;      /// The PBO to use as the pack target, or 0
    size_t SourceIndex;     /// The source mip level or array index, 0 for framebuffer
    size_t TargetX;         /// The upper-left-front corner on the target image
    size_t TargetY;         /// The upper-left-front corner on the target image
    size_t TargetZ;         /// The upper-left-front corner on the target image
    size_t TargetWidth;     /// The width of the full target image, in pixels
    size_t TargetHeight;    /// The height of the full target image, in pixels
    size_t TransferX;       /// The upper-left corner of the region to transfer
    size_t TransferY;       /// The upper-left corner of the region to transfer
    size_t TransferWidth;   /// The width of the region to transfer, in pixels
    size_t TransferHeight;  /// The height of the region to transfer, in pixels
    void  *TransferBuffer;  /// Pointer to target image data, or PBO byte offset
};

/// @summary Describes a transfer of pixel data from the host (CPU) to the
/// device (GPU), using glTexSubImage or glCompressedTexSubImage. The fields
/// Target[X/Y/Z] indicate where to place the data on the target texture. The
/// Source[X/Y/Z] fields can be specified to have the call calculate the offset
/// from the TransferBuffer pointer, or can be set to 0 if TransferBuffer is
/// pointing to the start of the source data. The Source[Width/Height] describe
/// the dimensions of the entire source image, which may be different than the
/// dimensions of the region being transferred. The source of the transfer may
/// be either a Pixel Buffer Object, in which case the UnpackBuffer should be
/// set to the handle of the PBO, and TransferBuffer is an offset, or an
/// application memory buffer, in which case UnpackBuffer should be set to 0
/// and TransferBuffer should point to the source image data. A host to device
/// transfer can be used to upload a source image, or a portion thereof, to a
/// texture object on the GPU. Note that the Format and DataType describe the
/// source image, and not the texture object.
struct pixel_transfer_h2d_t
{
    GLenum Target;          /// The image target, ex. GL_TEXTURE_2D
    GLenum Format;          /// The format of your pixel data, ex. GL_BGRA
    GLenum DataType;        /// The layout of your pixel data, ex. GL_UNSIGNED_INT_8_8_8_8_REV
    GLuint UnpackBuffer;    /// The PBO to use as the unpack source, or 0
    size_t TargetIndex;     /// The target mip level or array index
    size_t TargetX;         /// The upper-left-front corner on the target texture
    size_t TargetY;         /// The upper-left-front corner on the target texture
    size_t TargetZ;         /// The upper-left-front corner on the target texture
    size_t SourceX;         /// The upper-left-front corner on the source image
    size_t SourceY;         /// The upper-left-front corner on the source image
    size_t SourceZ;         /// The upper-left-front corner on the source image
    size_t SourceWidth;     /// The width of the full source image, in pixels
    size_t SourceHeight;    /// The height of the full source image, in pixels
    size_t TransferWidth;   /// The width of the region to transfer, in pixels
    size_t TransferHeight;  /// The height of the region to transfer, in pixels
    size_t TransferSlices;  /// The number of slices to transfer
    size_t TransferSize;    /// The total number of bytes to transfer
    void  *TransferBuffer;  /// Pointer to source image data, or PBO byte offset
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Given an OpenGL block-compressed internal format identifier,
/// determine the size of each compressed block, in pixels. For non block-
/// compressed formats, the block size is defined to be 1.
/// @param internal_format The OpenGL internal format value.
/// @return The dimension of a block, in pixels.
size_t block_dimension(GLenum internal_format);

/// @summary Given an OpenGL block-compressed internal format identifier,
/// determine the size of each compressed block of pixels.
/// @param internal_format The OpenGL internal format value. RGB/RGBA/SRGB and
/// SRGBA S3TC/DXT format identifiers are the only values currently accepted.
/// @return The number of bytes per compressed block of pixels, or zero.
size_t bytes_per_block(GLenum internal_format);

/// @summary Given an OpenGL internal format value, calculates the number of
/// bytes per-element (or per-block, for block-compressed formats).
/// @param internal_format The OpenGL internal format, for example, GL_RGBA.
/// @param data_type The OpenGL data type, for example, GL_UNSIGNED_BYTE.
/// @return The number of bytes per element (pixel or block), or zero.
size_t bytes_per_element(GLenum internal_format, GLenum data_type);

/// @summary Given an OpenGL internal_format value and a width, calculates the
/// number of bytes between rows in a 2D image slice.
/// @param internal_format The OpenGL internal format, for example, GL_RGBA.
/// @param data_type The OpenGL data type, for example, GL_UNSIGNED_BYTE.
/// @param width The row width, in pixels.
/// @param alignment The alignment requirement of the OpenGL implementation,
/// corresponding to the pname of GL_PACK_ALIGNMENT or GL_UNPACK_ALIGNMENT for
/// the glPixelStorei function. The specification default is 4.
/// @return The number of bytes per-row, or zero.
size_t bytes_per_row(GLenum internal_format, GLenum data_type, size_t width, size_t alignment);

/// @summary Calculates the size of the buffer required to store an image
/// with the specified attributes.
/// @param internal_format The OpenGL internal format value, for example,
/// GL_RGBA. The most common compressed formats are supported (DXT/S3TC).
/// @param data_type The data type identifier, for example, GL_UNSIGNED_BYTE.
/// @param width The width of the image, in pixels.
/// @param height The height of the image, in pixels.
/// @param alignment The alignment requirement of the OpenGL implementation,
/// corresponding to the pname of GL_PACK_ALIGNMENT or GL_UNPACK_ALIGNMENT for
/// the glPixelStorei function. The specification default is 4.
/// @return The number of bytes required to store the image data.
size_t bytes_per_slice(GLenum internal_format, GLenum data_type, size_t width, size_t height, size_t alignment);

/// @summary Calculates the dimension of an image (width, height, etc.) rounded
/// up to the next alignment boundary based on the internal format.
/// @summary internal_format The OpenGL internal format value, for example,
/// GL_RGBA. The most common compressed formats are supported (DXT/S3TC).
/// @param dimension The dimension value (width, height, etc.), in pixels.
/// @return The dimension value padded up to the next alignment boundary. The
/// returned value is always specified in pixels.
size_t image_dimension(GLenum internal_format, size_t dimension);

/// @summary Given an OpenGL internal format type value, determines the
/// corresponding base format value.
/// @param internal_format The OpenGL internal format value. See the
/// documentation for glTexImage2D(), internalFormat argument.
/// @return The OpenGL base internal format values. See the documentation for
/// glTexImage2D(), format argument.
GLenum base_format(GLenum internal_format);

/// @summary Given an OpenGL sampler type value, determines the corresponding
/// texture bind target identifier.
/// @param sampler_type The OpenGL sampler type, for example, GL_SAMPLER_2D.
/// @return The corresponding bind target, for example, GL_TEXTURE_2D.
GLenum texture_target(GLenum sampler_type);

/// @summary Computes the number of levels in a mipmap chain given the
/// dimensions of the highest resolution level.
/// @param width The width of the highest resolution level, in pixels.
/// @param height The height of the highest resolution level, in pixels.
/// @param slice_count The number of slices of the highest resolution level.
/// For everything except a 3D image, this value should be specified as 1.
/// @param max_levels The maximum number of levels in the mipmap chain. If
/// there is no limit, this value should be specified as 0.
size_t level_count(size_t width, size_t height, size_t slice_count, size_t max_levels);

/// @summary Computes the dimension (width, height or number of slices) of a
/// particular level in a mipmap chain given the dimension for the highest
/// resolution image.
/// @param dimension The dimension in the highest resolution image.
/// @param level_index The index of the target mip-level, with index 0
/// representing the highest resolution image.
/// @return The dimension in the specified mip level.
size_t level_dimension(size_t dimension, size_t level_index);

/// @summary Given basic image attributes, builds a complete description of
/// the levels in a mipmap chain.
/// @param internal_format The OpenGL internal format, for example GL_RGBA.
/// See the documentation for glTexImage2D(), internalFormat argument.
/// @param data_type The OpenGL data type, for example, GL_UNSIGNED_BYTE.
/// @param width The width of the highest resolution image, in pixels.
/// @param height The height of the highest resolution image, in pixels.
/// @param slice_count The number of slices in the highest resolution image.
/// For all image types other than 3D images, specify 1 for this value.
/// @param alignment The alignment requirement of the OpenGL implementation,
/// corresponding to the pname of GL_PACK_ALIGNMENT or GL_UNPACK_ALIGNMENT for
/// the glPixelStorei function. The specification default is 4.
/// @param max_levels The maximum number of levels in the mipmap chain. To
/// describe all possible levels, specify 0 for this value.
/// @param level_desc The array of level descriptors to populate.
void describe_mipmaps(
    GLenum            internal_format,
    GLenum            data_type,
    size_t            width,
    size_t            height,
    size_t            slice_count,
    size_t            alignment,
    size_t            max_levels,
    level_desc_t *level_desc);

/// @summary Fills a memory buffer with a checkerboard pattern. This is useful
/// for indicating uninitialized textures and for testing. The image internal
/// format is expected to be GL_RGBA, data type GL_UNSIGNED_INT_8_8_8_8_REV,
/// and the data is written using the native system byte ordering (GL_BGRA).
/// @param width The image width, in pixels.
/// @param height The image height, in pixels.
/// @param alpha The value to write to the alpha channel, in [0, 1].
/// @param buffer The buffer to which image data will be written.
void checker_image(size_t width, size_t height, float alpha, void *buffer);

/// @summary Given basic texture attributes, allocates storage for all levels
/// of a texture, such that the texture is said to be complete. This should
/// only be performed once per-texture. After calling this function, the
/// texture object attributes should be considered immutable. Transfer data to
/// the texture using the transfer_pixels_h2d() function. The wrapping modes
/// are always set to GL_CLAMP_TO_EDGE. The caller is responsible for creating
/// and binding the texture object prior to calling this function.
/// @param target The OpenGL texture target, defining the texture type.
/// @param internal_format The OpenGL internal format, for example GL_RGBA.
/// @param data_type The OpenGL data type, for example, GL_UNSIGNED_BYTE.
/// @param min_filter The minification filter to use.
/// @param mag_filter The magnification filter to use.
/// @param width The width of the highest resolution image, in pixels.
/// @param height The height of the highest resolution image, in pixels.
/// @param slice_count The number of slices in the highest resolution image. If
/// the @a target argument specifies an array target, this represents the
/// number of items in the texture array. For 3D textures, it represents the
/// number of slices in the image. For all other types, this value must be 1.
/// @param max_levels The maximum number of levels in the mipmap chain. To
/// define all possible levels, specify 0 for this value.
void texture_storage(
    GLenum target,
    GLenum internal_format,
    GLenum data_type,
    GLenum min_filter,
    GLenum mag_filter,
    size_t width,
    size_t height,
    size_t slice_count,
    size_t max_levels);

/// @summary Copies pixel data from the device (GPU) to the host (CPU). The
/// pixel data consists of the framebuffer contents, or the contents of a
/// single mip-level of a texture image.
/// @param transfer An object describing the transfer operation to execute.
void transfer_pixels_d2h(pixel_transfer_d2h_t *transfer);

/// @summary Copies pixel data from the host (CPU) to the device (GPU). The
/// pixel data is copied to a single mip-level of a texture image.
/// @param transfer An object describing the transfer operation to execute.
void transfer_pixels_h2d(pixel_transfer_h2d_t *transfer);

#endif /* !defined(LL_IMAGE_HPP) */
