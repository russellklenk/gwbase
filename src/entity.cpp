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
    x(0), y(0),
    scale_x(1.0f),
    scale_y(1.0f),
    orientation(0.0f),
    is_visible(false),
    is_collidable(false),
    is_active(false)
{
    /* empty */
}

Entity::~Entity(void)
{
    /* empty */
}

void Entity::input(double currentTime, double elapsedTime, InputManager *im)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    UNUSED_ARG(im);
}

void Entity::update(double currentTime, double elapsedTime)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
}

void Entity::render(double currentTime, double elapsedTime, DisplayManager *dm)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    UNUSED_ARG(dm);
}
