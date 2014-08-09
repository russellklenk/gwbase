/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a rectangle packer using an (unbalanced) binary tree
/// subdividing the space in the master rectangle. The algorithm is described
/// at http://www.blackpawn.com/texts/lightmaps.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef IM_PACKER_HPP
#define IM_PACKER_HPP

/*////////////////
//   Includes   //
////////////////*/
#include <vector>
#include "common.hpp"

/*////////////////
//  Data Types  //
////////////////*/
enum packer_node_flags_e
{
    NODE_FLAGS_NONE    =  0,
    NODE_FLAGS_USED    = (1 << 0)
};

/// @summary Represents a single node in a binary tree used for packing
/// image rectangles within a larger texture.
struct packer_node_t
{
    size_t    Flags;           /// Whether the node is empty, etc.
    size_t    Index;           /// Index of the associated packer_rect_t
    size_t    Child[2];        /// Index of the child nodes, 0 if no child
    size_t    Bound[4];        /// Left-Top-Right-Bottom bounding rectangle
};

/// @summary Represents a single sub-rectangle within a larger image. This
/// data is stored separately from nodes for more cache-friendly behavior.
struct packer_rect_t
{
    size_t    X;               /// X-coordinate of upper-left corner
    size_t    Y;               /// Y-coordinate of upper-left corner
    size_t    Width;           /// Width of the rectangle, in pixels
    size_t    Height;          /// Height of the rectangle, in pixels
    size_t    Image;           /// Application-defined image identifier
    size_t    Flags;           /// Flag bits, copied from the node
};

/// @summary Stores the data necessary for maintaining the set of sub-images
/// packed together within a single larger image.
struct packer_t
{
    size_t    Width;           /// Width of primary image, in pixels
    size_t    Height;          /// Height of primary image, in pixels
    size_t    Free;            /// Total area currently unused
    size_t    Used;            /// Total area currently used
    std::vector<packer_node_t> Nodes;    /// Storage for node instances
    std::vector<packer_rect_t> Rects;    /// Storage for rectangle data
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Initializes a new image packer. Image packers are used to dynamically
/// pack several images onto a single, larger image.
/// @param packer The image packer to initialize.
/// @param target_width The width of the atlas image, in pixels.
/// @param target_height The height of the atlas image, in pixels.
/// @param initial_capacity The expected number of sub-images. This value is
/// used for pre-allocation only; the actual number of images may be larger
/// or smaller than the initial capacity.
void packer_init(packer_t *packer, size_t target_width, size_t target_height, size_t initial_capacity);

/// @summary Resets an image packer to its initial empty state, without freeing
/// the underlying storage resources.
/// @param packer The image packer to reset.
void packer_clear(packer_t *packer);

/// @summary Attempts to position a sub-image within the master image.
/// @param packer The image packer used to position sub-images.
/// @param width The un-padded width of the sub-image, in pixels.
/// @param height The un-padded height of the sub-image, in pixels.
/// @param h_padding The amount of horizontal padding to use, in pixels.
/// @param v_padding The amount of vertical padding to use, in pixels.
/// @param image_id The application-defined identifier for the sub-image.
/// @param rect On return, this address is filled with information about the
/// rectangle of the sub-image within the master image. If the sub-image will
/// not fit on the master image, this value is not modified.
/// @return true if the sub-image was positioned on the master image.
bool packer_insert(packer_t *packer, size_t width, size_t height, size_t h_pad, size_t v_pad, size_t image_id, packer_rect_t *rect);

#endif /* !defined(IM_PACKER_HPP) */
