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
#include "ll_input.hpp"

/*////////////////
//  Data Types  //
////////////////*/
/// @summary A helper structure to describe the state of a controller.
/// Pointers point to internal memory and should not be freed.
struct controller_state_t
{
    int                  ControllerId;  /// The system identifier of the controller.
    bool                 IsAttached;    /// true if the controller is attached.
    size_t               AxisCount;     /// The number of axis values provided.
    size_t               ButtonCount;   /// The number of button values provided.
    float const         *AxisValues;    /// The controller axis values.
    unsigned char const *ButtonValues;  /// The controller button values.
};

/// @summary Maintains a snapshot of state for user input devices.
class InputManager
{
private:
    static InputManager *IM;
public:
    static InputManager *GetInstance(void);

private:
    GLFWwindow       *MainWindow;       /// The window to which we're attached.
    input_snapshot_t  CurrentState;     /// Input state for the current tick.
    input_snapshot_t  PreviousState;    /// Input state from the previous tick.
    uint32_t          ConnectEvents;    /// Bit i set if controller index i was connected.
    uint32_t          DisconnectEvents; /// Bit i set if controller index i was disconnected.
    float             MouseDeltaX;      /// Mouse delta from the previous tick.
    float             MouseDeltaY;      /// Mouse delta from the previous tick.

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
    /// @summary Retrieve the state of all input devices for the current tick.
    /// @return The input device state for the current tick.
    input_snapshot_t const* GetCurrentSnapshot(void)  const { return &CurrentState;  }

    /// @summary Retrieve the state of all input devices for the previous tick.
    /// @return The input device state for the previous tick.
    input_snapshot_t const* GetPreviousSnapshot(void) const { return &PreviousState; }

    /// @summary Retrieve the window attached to this InputManager.
    /// @return The attached window.
    GLFWwindow* GetWindow(void) const { return MainWindow; }

    /// @summary Retrieve the change in mouse position during the tick.
    /// @return The change in mouse position along the x-axis.
    float GetMouseDeltaX(void) const { return MouseDeltaX; }

    /// @summary Retrieve the change in mouse position during the tick.
    /// @return The change in mouse position along the y-axis.
    float GetMouseDeltaY(void) const { return MouseDeltaY; }

public:
    /// @summary Determines whether a key is currently not pressed.
    /// @param key The GLFW button ID to query.
    /// @return true if the key is currently not pressed.
    bool IsKeyUp(int key) const;

    /// @summary Determines whether a key is currently down.
    /// @param key The GLFW button ID to query.
    /// @return true if the key is currently pressed.
    bool IsKeyDown(int key) const;

    /// @summary Determines whether a mouse button is currently not pressed.
    /// @param button The GLFW button ID to query.
    /// @return true if the mouse button is currently not pressed.
    bool IsMouseButtonUp(int button) const;

    /// @summary Determines whether a mouse button is currently down.
    /// @param button The GLFW button ID to query.
    /// @return true if the mouse button is currently pressed.
    bool IsMouseButtonDown(int button) const;

    /// @summary Determines whether a controller is currently connected.
    /// @param id The GLFW joystick ID to query.
    /// @return true of the specified controller is currently connected.
    bool IsControllerConnected(int id) const;

    /// @summary Determines whether a key was pressed this tick.
    /// @param key The GLFW key ID to query.
    /// @return true if the specified key was pressed.
    bool WasKeyPressed(int key) const;

    /// @summary Determines whether a key was released this tick.
    /// @param key The GLFW key ID to query.
    /// @return true if the specified key was released.
    bool WasKeyReleased(int key) const;

    /// @summary Determines whether a mouse button was pressed this tick.
    /// @param button The GLFW button ID to query.
    /// @return true if the specified mouse button was pressed.
    bool WasMouseButtonPressed(int button) const;

    /// @summary Determines whether a mouse button was released this tick.
    /// @param button The GLFW button ID to query.
    /// @return true if the specified mouse button was released.
    bool WasMouseButtonReleased(int button) const;

    /// @summary Determines whether a controller was connected this tick.
    /// @param id The GLFW joystick ID to query.
    /// @return true if the specified controller was connected this tick.
    bool WasControllerConnected(int id) const;

    /// @summary Determines whether a controller was disconnected this tick.
    /// @param id The GLFW joystick ID to query.
    /// @return true if the specified controller was disconnected this tick.
    bool WasControllerDisconnected(int id) const;

    /// @summary Retrieve the state of a controller. The state snapshots are
    /// valid until the next call to InputManager::Update().
    /// @param id The GLFW joystick identifier of the controller.
    /// @param prev On return, stores the state of the controller on the previous tick.
    /// @param curr On return, stores the state of the controller on the current tick.
    void GetController(int id, controller_state_t *prev, controller_state_t *curr) const;

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
