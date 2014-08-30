/*/////////////////////////////////////////////////////////////////////////////
/// @summary Defines the low-level interface to the input system, which is
/// built on top of the abstraction provided by GLFW3.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef LL_INPUT_HPP
#define LL_INPUT_HPP

/*////////////////
//   Includes   //
////////////////*/
#include "common.hpp"
#include "platform.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary Define the number of uin32_t values required to store the keyboard
/// state data. One bit corresponds to each key.
#define INPUT_KEY_WORDS              10U

/// @summary The key code offset value. This is subtracted from the key identifier
/// before indexing into the key state array.
#define INPUT_KEY_OFFSET             32

/// @summary Define the maximum number of buttons supported on a mouse.
#define INPUT_MAX_MOUSE_BUTTONS      32

/// @summary Define the maximum number of axis inputs supported on a joystick.
#define INPUT_MAX_CONTROLLER_AXES     8

/// @summary Define the maximum number of button inputs supported on a joystick.
#define INPUT_MAX_CONTROLLER_BUTTONS 32

/// @summary Define the maximum number of joystick controllers.
#define INPUT_MAX_CONTROLLERS        16

/// @summary Internal data associated with the input system attached to a window.
struct input_context_t
{
    GLFWwindow *Window;
    float       ScaleX;
    float       ScaleY;
    float       MouseX;
    float       MouseY;
    uint32_t    MouseState;
    uint32_t    MouseModifiers;
    uint32_t    KeyboardModifiers;
    uint32_t    KeyboardState[INPUT_KEY_WORDS];
};

/// @summary Represents a snapshot of state for all input devices at a single point in time.
struct input_snapshot_t
{
    GLFWwindow *Window;
    float       ScaleX;
    float       ScaleY;
    float       MouseX;
    float       MouseY;
    uint32_t    MouseState;
    uint32_t    MouseModifiers;
    uint32_t    KeyboardModifiers;
    uint32_t    KeyboardState[INPUT_KEY_WORDS];
    size_t      ControllerCount;
    int         ControllerIds[INPUT_MAX_CONTROLLERS];
    size_t      ControllerAxisCount[INPUT_MAX_CONTROLLERS];
    size_t      ControllerButtonCount[INPUT_MAX_CONTROLLERS];
    float       ControllerAxes[INPUT_MAX_CONTROLLERS][INPUT_MAX_CONTROLLER_AXES];
    uint8_t     ControllerButtons[INPUT_MAX_CONTROLLERS][INPUT_MAX_CONTROLLER_BUTTONS];
};

/*///////////////
//  Functions  //
///////////////*/
/// @summary Attaches event handlers to the given window.
/// @param window The window to attach event handlers to.
bool input_attach(GLFWwindow *window);

/// @summary Detaches event handlers from the given window.
/// @param window The window to detach from.
void input_detach(GLFWwindow *window);

/// @summary Grabs a snapshot of input device state for the specified window.
/// @param dst The snapshot structure to populate.
/// @param window The window whose input state is being queried.
void input_snapshot(input_snapshot_t *dst, GLFWwindow *window);

#endif /* !defined(LL_INPUT_HPP) */
