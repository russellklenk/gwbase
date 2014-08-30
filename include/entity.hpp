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
#include "display.hpp"
#include "input.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Define the various types of entities.
enum EntityType
{
    ENTITY_DONT_CARE = 0,
    ENTITY_BULLET    = 1,
    ENTITY_ENEMY     = 2,
    ENTITY_BLACKHOLE = 3
};

/// @summary The base class for all game entities.
class Entity
{
protected:
    Texture   *Image;       /// The texture used to render the entity.
    float      Color[4];    /// The RGBA tint color used to modify the entity.
    float      Position[2]; /// The entity position [0] = X, [1] = Y.
    float      Velocity[2]; /// The entity velocity [0] = X, [1] = Y.
    float      Orientation; /// The entity angle of orientation, in radians.
    float      Radius;      /// The entity radius, used for collision detection.
    bool       IsExpired;   /// true if this entity has 'died'.
    EntityType Kind;        /// The type of entity.

public:
    Entity(void);
    virtual ~Entity(void);

public:
    EntityType GetKind(void) const { return Kind; }
    float GetWidth(void) const { return (float) Image->GetWidth(); }
    float GetHeight(void) const { return (float) Image->GetHeight(); }
    float const* GetPosition(void) const { return Position; }
    float const* GetVelocity(void) const { return Velocity; }
    float GetRadius(void) const { return Radius; }
    bool GetExpired(void) const { return IsExpired; }
    void SetVelocity(float x, float y) { Velocity[0] = x; Velocity[1] = y; }
    void SetExpired(void) { IsExpired = true; }

public:
    /// @summary Perform initialization when the entity is spawned.
    /// @param dm The DisplayManager, which can be used to retrieve textures.
    virtual void Init(DisplayManager *dm) = 0;

    /// @summary Executes a single simulation tick for the entity.
    /// @param currentTime The current simulation time, in seconds.
    /// @param elapsedTime The time elapsed since the last simulation tick.
    virtual void Update(double currentTime, double elapsedTime) = 0;

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

#endif /* !defined(GW_ENTITY_HPP) */
