/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the state associated with a single bullet.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_BULLET_HPP
#define GW_BULLET_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "entity.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Represents a single bullet entity.
class Bullet : public Entity
{
protected:
    float ViewportWidth;
    float ViewportHeight;

public:
    Bullet(float p_x=0.0f, float p_y=0.0f, float v_x=0.0f, float v_y=0.0f);
    virtual ~Bullet(void);

public:
    /// @summary Perform initialization when the entity is spawned.
    /// @param dm The DisplayManager, which can be used to retrieve textures.
    virtual void Init(DisplayManager *dm);

    /// @summary Executes a single simulation tick for the entity.
    /// @param currentTime The current simulation time, in seconds.
    /// @param elapsedTime The time elapsed since the last simulation tick.
    virtual void Update(double currentTime, double elapsedTime);
};

#endif /* !defined(GW_BULLET_HPP) */
