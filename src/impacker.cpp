/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a rectangle packer using an (unbalanced) binary tree
/// subdividing the space in the master rectangle. The algorithm is described
/// at http://www.blackpawn.com/texts/lightmaps.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "impacker.hpp"

/*//////////////////////
//   Implementation   //
//////////////////////*/
static const size_t NODE_INDEX_NONE = (size_t) -1;

static packer_node_t* node_insert(packer_t *p, size_t n, size_t w, size_t h)
{
    packer_node_t *node = &p->Nodes[n];
    if (node->Child[0] != 0 && node->Child[1] != 0)
    {
        // this node is not a leaf node, so attempt the insert in the subtree.
        packer_node_t *target_node = node_insert(p, node->Child[0], w, h);
        if (target_node) return target_node;
        else return node_insert(p, node->Child[1], w, h);
    }
    else
    {
        if (node->Flags & NODE_FLAGS_USED)
            return NULL;

        // if the sub-rect won't fit, don't continue down this path.
        size_t rect_width  = node->Bound[2] - node->Bound[0];
        size_t rect_height = node->Bound[3] - node->Bound[1];
        if (w  > rect_width || h  > rect_height)
            return NULL;

        // if the sub-rect fits exactly, we'll store it at 'node'.
        if (w == rect_width && h == rect_height)
            return node;

        // otherwise, we'll split the space at this node into a used
        // portion, stored in node->Child[0], and an unsed portion in
        // stored in node->Child[1].
        packer_node_t  a;
        a.Flags      = NODE_FLAGS_NONE;
        a.Index      = NODE_INDEX_NONE;
        a.Child[0]   = 0;
        a.Child[1]   = 0;

        packer_node_t  b;
        b.Flags      = NODE_FLAGS_NONE;
        b.Index      = NODE_INDEX_NONE;
        b.Child[0]   = 0;
        b.Child[1]   = 0;

        size_t dw = rect_width  - w;
        size_t dh = rect_height - h;
        if (dw > dh)
        {
            a.Bound[0] = node->Bound[0];
            a.Bound[1] = node->Bound[1];
            a.Bound[2] = node->Bound[0] + w;
            a.Bound[3] = node->Bound[3];

            b.Bound[0] = node->Bound[0] + w;
            b.Bound[1] = node->Bound[1];
            b.Bound[2] = node->Bound[2];
            b.Bound[3] = node->Bound[3];
        }
        else
        {
            a.Bound[0] = node->Bound[0];
            a.Bound[1] = node->Bound[1];
            a.Bound[2] = node->Bound[2];
            a.Bound[3] = node->Bound[1] + h;

            b.Bound[0] = node->Bound[0];
            b.Bound[1] = node->Bound[1] + h;
            b.Bound[2] = node->Bound[2];
            b.Bound[3] = node->Bound[3];
        }

        node->Child[0] = p->Nodes.size() + 0;
        node->Child[1] = p->Nodes.size() + 1;
        p->Nodes.push_back(a);
        p->Nodes.push_back(b);
        return node_insert(p, node->Child[0], w, h);
    }
}

void packer_init(packer_t *packer, size_t target_width, size_t target_height, size_t initial_capacity)
{
    packer->Width  = target_width;
    packer->Height = target_height;
    packer->Free   = target_width * target_height;
    packer->Used   = 0;
    packer->Nodes.reserve(initial_capacity * 3); // node, left child, right child
    packer->Rects.reserve(initial_capacity);
    packer_clear(packer);
}

void packer_clear(packer_t *packer)
{
    packer_node_t root;
    root.Flags    = NODE_FLAGS_NONE;
    root.Index    = NODE_INDEX_NONE;
    root.Child[0] = 0;
    root.Child[1] = 0;
    root.Bound[0] = 0;
    root.Bound[1] = 0;
    root.Bound[2] = packer->Width;
    root.Bound[3] = packer->Height;

    packer->Nodes.clear();
    packer->Rects.clear();
    packer->Nodes.push_back(root);
    packer->Free  = packer->Width * packer->Height;
    packer->Used  = 0;
}

bool packer_insert(packer_t *packer, size_t width, size_t height, size_t h_pad, size_t v_pad, size_t image_id, packer_rect_t *rect)
{
    size_t w = width  + (h_pad * 2);
    size_t h = height + (v_pad * 2);
    size_t a = w * h;

    // if there isn't enough space available, don't
    // bother searching for a place to put this image.
    if (a > packer->Free)
        return false;

    packer_node_t *target_node = node_insert(packer, 0, w, h);
    if (target_node)
    {
        packer_rect_t r;
        r.X       = target_node->Bound[0] + h_pad;
        r.Y       = target_node->Bound[1] + v_pad;
        r.Width   = width;
        r.Height  = height;
        r.Image   = image_id;
        r.Flags   = target_node->Flags;
        if (rect) *rect = r;

        target_node->Flags |= NODE_FLAGS_USED;
        target_node->Index  = packer->Rects.size();
        packer->Rects.push_back(r);
        packer->Free -= a;
        packer->Used += a;
        return true;
    }
    else return false;
}
