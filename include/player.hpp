/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the state associated with the player ship.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_PLAYER_HPP
#define GW_PLAYER_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "entity.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Represents a single player entity.
class Player : public Entity
{
protected:
    float TargetPoint[2];
    float TargetVector[2];
    float CooldownRemaining;
    float TimeUntilRespawn;
    float ViewportWidth;
    float ViewportHeight;
    float ShipSpeed;
    int   PlayerIndex;

public:
    Player(int index);
    virtual ~Player(void);

public:
    /// @summary Retrieve the unique player identifier.
    /// @return The unique identifier of the player.
    int  GetIndex(void) const { return PlayerIndex; }

    /// @summary Check whether the player has died.
    /// @return true if the player is dead or waiting to respawn.
    bool IsDead(void) const;

    /// @summary Marks the player as being dead.
    void Kill(void);

public:
    /// @summary Perform initialization when the entity is spawned.
    /// @param dm The DisplayManager, which can be used to retrieve textures.
    virtual void Init(DisplayManager *dm);

    /// @summary Executes a single simulation tick for the entity.
    /// @param currentTime The current simulation time, in seconds.
    /// @param elapsedTime The time elapsed since the last simulation tick.
    virtual void Update(double currentTime, double elapsedTime);

    /// @summary Handles user input for the entity.
    /// @param currentTime The current game time, in seconds.
    /// @param elapsedTime The time elapsed since the previous frame, in seconds.
    /// @param dm The input manager used to query input device state.
    virtual void Input(double currentTime, double elapsedTime, InputManager *im);

    /// @summary Sets state and submits geometry used for rendering.
    /// @param currentTime The current game time, in seconds.
    /// @param elapsedTime The time elapsed since the previous frame, in seconds.
    /// @param dm The display manager used to submit rendering commands.
    virtual void Draw(double currentTime, double elapsedTime, DisplayManager *dm);
};

#endif /* !defined(GW_PLAYER_HPP) */
