/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the base class for all game entities.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "entity.hpp"
#include "display.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
Entity::Entity(void) :
    Image(NULL),
    Orientation(0.0f),
    Radius(0.0f),
    IsExpired(false),
    Kind(ENTITY_DONT_CARE)
{
    Color[0] = 1.0f;
    Color[1] = 1.0f;
    Color[2] = 1.0f;
    Color[3] = 1.0f;
}

Entity::~Entity(void)
{
    /* empty */
}

void Entity::Input(double currentTime, double elapsedTime, InputManager *im)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    UNUSED_ARG(im);
}

void Entity::Draw(double currentTime, double elapsedTime, DisplayManager *dm)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    float  width   = (float) Image->GetWidth();
    float  height  = (float) Image->GetHeight();
    rect_t src     = { 0, 0, width, height };
    float  posx    = Position[0];
    float  posy    = Position[1];
    float  originx = width  * 0.5f;
    float  originy = height * 0.5f;
    float  scalex  = 1.0f;
    float  scaley  = 1.0f;
    dm->GetBatch()->Add(1, Image, posx, posy, src, Color, Orientation, originx, originy, scalex, scaley);
}
