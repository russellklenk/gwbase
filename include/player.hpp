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
public:
    int player_index;

public:
    Player(int index);
    virtual ~Player(void);

public:
    /// @summary Processes user input during a single tick.
    /// @param currentTime The current game time, in seconds.
    /// @param elapsedTime The time elapsed since the previous frame, in seconds.
    /// @param im The input manager used to read user input events.
    virtual void input(double currentTime, double elapsedTime, InputManager *im);

    /// @summary Executes a single simulation tick for the entity.
    /// @param currentTime The current simulation time, in seconds.
    /// @param elapsedTime The time elapsed since the last simulation tick.
    virtual void update(double currentTime, double elapsedTime);

    /// @summary Sets state and submits geometry used for rendering.
    /// @param currentTime The current game time, in seconds.
    /// @param elapsedTime The time elapsed since the previous frame, in seconds.
    /// @param dm The display manager used to submit rendering commands.
    virtual void render(double currentTime, double elapsedTime, DisplayManager *dm);
};

#endif /* !defined(GW_PLAYER_HPP) */
