/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the base class for all game entities.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_ENTITY_HPP
#define GW_ENTITY_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// Forward declare the input manager used for reading input events.
class InputManager;

/// Forward declare the display manager used for rendering.
class DisplayManager;

/// @summary The base class for all game entities.
class Entity
{
public:
    float x;
    float y;
    float scale_x;
    float scale_y;
    float orientation;
    bool  is_visible;
    bool  is_collidable;
    bool  is_active;

protected:
    Entity(void);

public:
    virtual ~Entity(void);

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

#endif /* !defined(GW_ENTITY_HPP) */
