/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_INPUT_HPP
#define GW_INPUT_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary
class InputManager
{
public:
    GLFWwindow *main_window;

public:
    InputManager(void);
    ~InputManager(void);

public:
    bool init(GLFWwindow *win);
    void shutdown(void);

private:
    InputManager(InputManager const &other);
    InputManager& operator =(InputManager const &other);
};

#endif /* !defined(GW_INPUT_HPP) */
