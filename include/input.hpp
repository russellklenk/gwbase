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
/// @summary Define the maximum number of axes supported per-controller.
#define MAX_CONTROLLER_AXES        8

/// @summary Define the maximum number of buttons supported per-controller.
#define MAX_CONTROLLER_BUTTONS    16

/// @summary Define the maximum number of controllers supported by the implementation.
#define MAX_CONTROLLERS            2

/// @summary Stores a snapshot of the current mouse state.
struct mouse_state_t
{
    float    X;             /// The x-coordinate of the mouse cursor (relative to client area.)
    float    Y;             /// The y-coordinate of the mouse cursor (relative to client area.)
    uint32_t Buttons;       /// Bit i is set if the corresponding button is down.
    uint32_t ModifierState; /// See http://www.glfw.org/docs/latest/group__mods.html
};

/// @summary Stores a snapshot of the current key state.
struct keyboard_state_t
{
    uint32_t ModifierState; /// See http://www.glfw.org/docs/latest/group__mods.html
    uint32_t KeyStates[11]; /// Bit i is set if the corresponding key is down. Offset by 32.
};

/// @summary Stores a snapshot of the current controller state.
struct controller_state_t
{
    uint32_t Index;         /// The zero-based index of the controller.
    uint32_t Connected;     /// Set to one if the controller is connected.
    uint32_t AxisCount;     /// The number of axes supplied by the controller.
    uint32_t ButtonCount;   /// The number of buttons supplied by the controller.
    float    AxisValues[8]; /// The values read for each valid axis.
    uint8_t  ButtonValues[16]; /// The values read for each valid button.
};

/// @summary Stores a snapshot of the input state for all input devices.
class InputState
{
public:
    mouse_state_t      Mouse;
    keyboard_state_t   Keyboard;
    controller_state_t Controllers[MAX_CONTROLLERS];

public:
    InputState(void);
    InputState(InputState const &other);
    ~InputState(void);

public:
    InputState& operator =(InputState const &other);

public:
    void Reset(void);
};

/// @summary Maintains a snapshot of state for user input devices.
class InputManager
{
private:
    GLFWwindow         *MainWindow;           /// The window to which we're attached.
    InputState          CurrentState;         /// Input state for the current tick.
    InputState          PreviousState;        /// Input state from the previous tick.

public:
    float               MouseDeltaX;          /// Mouse delta from the previous tick.
    float               MouseDeltaY;          /// Mouse delta from the previous tick.
    float               MouseX;               /// Current mouse position, relative to client area.
    float               MouseY;               /// Current mouse position, relative to client area.
    uint32_t            Modifiers;            /// Modifier keys active this tick.
    size_t              KeysDownCount;        /// Number of keys down this tick.
    size_t              KeysPressedCount;     /// Number of keys pressed this tick.
    size_t              KeysReleasedCount;    /// Number of keys released this tick.
    size_t              ButtonsDownCount;     /// Number of mouse buttons down this tick.
    size_t              ButtonsPressedCount;  /// Number of mouse buttons pressed this tick.
    size_t              ButtonsReleasedCount; /// Number of mouse buttons released this tick.
    size_t              ControllerCount;      /// Number of active controllers this tick.
    uint32_t           *KeysDown;             /// Keys down this tick.
    uint32_t           *KeysPressed;          /// Keys pressed this tick.
    uint32_t           *KeysReleased;         /// Keys released this tick.
    uint32_t           *ButtonsDown;          /// Mouse buttons down this tick.
    uint32_t           *ButtonsPressed;       /// Mouse buttons pressed this tick.
    uint32_t           *ButtonsReleased;      /// Mouse buttons released this tick.
    controller_state_t *Controllers;          /// Attached controllers this tick.

public:
    InputManager(void);
    ~InputManager(void);

public:
    /// @summary Performs one-time initialization for the input manager and
    /// resets the state of all input devices.
    /// @param win The window to which the input manager is attached.
    /// @return true if initialization was successful.
    bool Init(GLFWwindow *win);

    /// @summary Performs any teardown required by the input manager.
    void Shutdown(void);

public:
    /// @summary Pushes a mouse movement event to the input system.
    /// @param x The current position of the mouse cursor, in pixels, relative
    /// to the client area of the attached window.
    /// @param y The current position of the mouse cursor, in pixels, relative
    /// to the client area of the attached window.
    void MouseMove(double x, double y);

    /// @summary Pushes a mouse button event to the input system.
    /// @param button One of the GLFW_MOUSE_BUTTON_x identifiers.
    /// @param action One of GLFW_PRESS or GLFW_RELEASE.
    /// @param modifiers GLFW modifier bitflags.
    void MouseButton(int button, int action, int modifiers);

    /// @summary Pushes a keyboard event to the input system.
    /// @param key One of the GLFW_KEY_x identifiers.
    /// @param scan The raw OS scancode associated with the key.
    /// @param action One of GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT.
    /// @param modifiers GLFW modifier bitflags.
    void KeyChange(int key, int scan, int action, int modifiers);

public:
    /// @summary Updates the current input device state.
    /// @param currentTime The total elapsed application time, in seconds.
    /// @param elapsedTime The time elapsed since the previous tick, in seconds.
    void Update(double currentTime, double elapsedTime);

private:
    InputManager(InputManager const &other);
    InputManager& operator =(InputManager const &other);
};

#endif /* !defined(GW_INPUT_HPP) */
