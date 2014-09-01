/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the game logic associated with a bullet.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include "math.hpp"
#include "bullet.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
Bullet::Bullet(float p_x, float p_y, float v_x, float v_y)
    :
    ViewportWidth(0.0f),
    ViewportHeight(0.0f)
{
    Position[0] = p_x;
    Position[1] = p_y;
    Velocity[0] = v_x;
    Velocity[1] = v_y;
    Kind        = ENTITY_BULLET;
}

Bullet::~Bullet(void)
{
    /* empty */
}

void Bullet::Init(DisplayManager *dm)
{
    Image          = dm->GetBulletTexture();
    Radius         = max2(float(Image->GetWidth()), float(Image->GetHeight()));
    ViewportWidth  = dm->GetViewportWidth();
    ViewportHeight = dm->GetViewportHeight();
}

void Bullet::Update(double currentTime, double elapsedTime)
{
    float current = float(currentTime);
    float elapsed = float(elapsedTime);

    Orientation   = atan2(Velocity[1], Velocity[0]);
    Position[0]  += Velocity[0];
    Position[1]  += Velocity[1];

    if (Position[0] < 0 || Position[0] > ViewportWidth ||
        Position[1] < 0 || Position[1] > ViewportHeight)
    {
        IsExpired = true;
    }

    UNUSED_LOCAL(current);
    UNUSED_LOCAL(elapsed);
}
