/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the game logic associated with the player ship.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <math.h>
#include <sstream>
#include "player.hpp"
#include "math.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
static const float RESPAWN_TIME  = 300.0f / 60.0f;
static const float COOLDOWN_TIME = 6.0f   / 60.0f;
static const float SHIP_SPEED    = 550.0f;

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
Player::Player(int index) :
    CooldownRemaining(0.0f),
    TimeUntilRespawn(0.0f),
    ViewportWidth(0.0f),
    ViewportHeight(0.0f),
    ShipSpeed(SHIP_SPEED),
    PlayerIndex(index)
{
    /* empty */
}

Player::~Player(void)
{
    /* empty */
}

bool Player::IsDead(void) const
{
    return (TimeUntilRespawn > 0.0f);
}

void Player::Kill(void)
{
    TimeUntilRespawn = RESPAWN_TIME;
}

void Player::Init(DisplayManager *dm)
{
    disp            = dm;
    Image           = dm->GetPlayerTexture();
    Radius          = max2(Image->GetWidth(), Image->GetHeight());
    ShipSpeed       = SHIP_SPEED;
    ViewportWidth   = dm->GetViewportWidth();
    ViewportHeight  = dm->GetViewportHeight();
    TargetPoint[0]  = ViewportWidth  * 0.5f;
    TargetPoint[1]  = ViewportHeight * 0.5f;
    TargetVector[0] = 0.0f;
    TargetVector[1] = 0.0f;
    Position[0]     = ViewportWidth  * 0.5f;
    Position[1]     = ViewportHeight * 0.5f;
    Velocity[0]     = 0.0f;
    Velocity[1]     = 0.0f;
}

void Player::Input(double currentTime, double elapsedTime, InputManager *im)
{
    float current = float(currentTime);
    float elapsed = float(elapsedTime);
    float mouse_x = im->GetCurrentSnapshot()->MouseX;
    float mouse_y = im->GetCurrentSnapshot()->MouseY;
    float dist_x  = mouse_x - Position[0];
    float dist_y  = mouse_y - Position[1];
    if (dist_x != 0 && dist_y != 0)
    {
        Orientation     = atan2f(dist_y, dist_x);
        Velocity[0]     = dist_x / (ShipSpeed * elapsed);
        Velocity[1]     = dist_y / (ShipSpeed * elapsed);
        TargetPoint[0]  = mouse_x;
        TargetPoint[1]  = mouse_y;
        TargetVector[0] = dist_x;
        TargetVector[1] = dist_y;
    }
    UNUSED_LOCAL(current);
}

void Player::Update(double currentTime, double elapsedTime)
{
    float current = float(currentTime);
    float elapsed = float(elapsedTime);

    if (IsDead())
    {
        TimeUntilRespawn     -= elapsed;
        if (TimeUntilRespawn <= 0.0f)
        {
            // respawn the player.
            TargetPoint[0]    = ViewportWidth  * 0.5f;
            TargetPoint[1]    = ViewportHeight * 0.5f;
            TargetVector[0]   = 0.0f;
            TargetVector[1]   = 0.0f;
            Position[0]       = ViewportWidth  * 0.5f;
            Position[1]       = ViewportHeight * 0.5f;
            Velocity[0]       = 0.0f;
            Velocity[1]       = 0.0f;
            TimeUntilRespawn  = 0.0f;
        }
    }
    else
    {
        if (CooldownRemaining > 0.0f)
        {
            CooldownRemaining -= elapsed;
        }
        Position[0] += Velocity[0];
        Position[1] += Velocity[1];
        Position[0]  = clamp(Position[0], 0, ViewportWidth);
        Position[1]  = clamp(Position[1], 0, ViewportHeight);
    }
    UNUSED_LOCAL(current);
}

void Player::Draw(double currentTime, double elapsedTime, DisplayManager *dm)
{
    if (IsDead() == false)
    {
        Entity::Draw(currentTime, elapsedTime, dm);
    }
    ViewportWidth  = dm->GetViewportWidth();
    ViewportHeight = dm->GetViewportHeight();
}
