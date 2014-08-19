/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <string.h>
#include "input.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
#define MAX_KEY_CODES        (11U * 32U)
#define MAX_MOUSE_BUTTONS    (32U)

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
InputState::InputState(void)
{
    Reset();
}

InputState::InputState(InputState const &other)
    :
    Mouse(other.Mouse),
    Keyboard(other.Keyboard)
{
    for (size_t i = 0; i < MAX_CONTROLLERS; ++i)
    {
        Controllers[i] = other.Controllers[i];
    }
}

InputState::~InputState(void)
{
    /* empty */
}

InputState& InputState::operator =(InputState const &other)
{
    if (this != &other)
    {
        Mouse    = other.Mouse;
        Keyboard = other.Keyboard;
        for (size_t i = 0; i < MAX_CONTROLLERS; ++i)
        {
            Controllers[i] = other.Controllers[i];
        }
    }
    return *this;
}

void InputState::Reset(void)
{
    memset(&Mouse,       0, sizeof(mouse_state_t));
    memset(&Keyboard,    0, sizeof(keyboard_state_t));
    memset(&Controllers, 0, sizeof(controller_state_t) * MAX_CONTROLLERS);
}

InputManager::InputManager(void)
    :
    MainWindow(NULL),
    MouseDeltaX(0.0f),
    MouseDeltaY(0.0f),
    MouseX(0.0f),
    MouseY(0.0f),
    Modifiers(0),
    KeysDownCount(0),
    KeysPressedCount(0),
    KeysReleasedCount(0),
    ButtonsDownCount(0),
    ButtonsPressedCount(0),
    ButtonsReleasedCount(0),
    ControllerCount(0),
    KeysDown(NULL),
    KeysPressed(NULL),
    KeysReleased(NULL),
    ButtonsDown(NULL),
    ButtonsPressed(NULL),
    ButtonsReleased(NULL),
    Controllers(NULL)
{
    KeysDown        = new uint32_t[MAX_KEY_CODES];
    KeysPressed     = new uint32_t[MAX_KEY_CODES];
    KeysReleased    = new uint32_t[MAX_KEY_CODES];
    ButtonsDown     = new uint32_t[MAX_MOUSE_BUTTONS];
    ButtonsPressed  = new uint32_t[MAX_MOUSE_BUTTONS];
    ButtonsReleased = new uint32_t[MAX_MOUSE_BUTTONS];
    Controllers     = new controller_state_t[MAX_CONTROLLERS];
}

InputManager::~InputManager(void)
{
    Shutdown();
    MainWindow = NULL;
    delete[] Controllers;     Controllers     = NULL;
    delete[] ButtonsReleased; ButtonsReleased = NULL;
    delete[] ButtonsPressed;  ButtonsPressed  = NULL;
    delete[] ButtonsDown;     ButtonsDown     = NULL;
    delete[] KeysReleased;    KeysReleased    = NULL;
    delete[] KeysPressed;     KeysPressed     = NULL;
    delete[] KeysDown;        KeysDown        = NULL;
    KeysDownCount        = 0;
    KeysPressedCount     = 0;
    KeysReleasedCount    = 0;
    ButtonsDownCount     = 0;
    ButtonsPressedCount  = 0;
    ButtonsReleasedCount = 0;
    ControllerCount      = 0;
}


bool InputManager::Init(GLFWwindow *win)
{
    if (win != NULL)
    {
        MainWindow = win;
        CurrentState.Reset();
        PreviousState.Reset();
        MouseDeltaX          = 0.0f;
        MouseDeltaY          = 0.0f;
        MouseX               = 0.0f;
        MouseY               = 0.0f;
        Modifiers            = 0;
        KeysDownCount        = 0;
        KeysPressedCount     = 0;
        KeysReleasedCount    = 0;
        ButtonsDownCount     = 0;
        ButtonsPressedCount  = 0;
        ButtonsReleasedCount = 0;
        ControllerCount      = 0;
        return true;
    }
    else return false;
}

void InputManager::Shutdown(void)
{
    /* empty */
}

void InputManager::MouseMove(double x, double y)
{
    CurrentState.Mouse.X = float(x);
    CurrentState.Mouse.Y = float(y);
}

void InputManager::MouseButton(int button, int action, int modifiers)
{
    if (action == GLFW_PRESS)
    {
        CurrentState.Mouse.Buttons |= (1 << button);
        CurrentState.Mouse.ModifierState = modifiers;
    }
    if (action == GLFW_RELEASE)
    {
        CurrentState.Mouse.Buttons &=~(1 << button);
        CurrentState.Mouse.ModifierState = modifiers;
    }
}

void InputManager::KeyChange(int key, int scancode, int action, int modifiers)
{
    UNUSED_ARG(scancode);
    if (action == GLFW_PRESS)
    {
        CurrentState.Keyboard.KeyStates[key >> 5] |= (1 << (key & 0x1F));
        CurrentState.Keyboard.ModifierState = modifiers;
    }
    if (action == GLFW_RELEASE)
    {
        CurrentState.Keyboard.KeyStates[key >> 5] |= (1 << (key & 0x1F));
        CurrentState.Keyboard.ModifierState = modifiers;
    }
}

void InputManager::Update(double currentTime, double elapsedTime)
{
    uint32_t curr    = 0;
    uint32_t prev    = 0;
    uint32_t changes = 0;
    uint32_t downs   = 0;
    uint32_t ups     = 0;
    uint32_t mask    = 0;

    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);

    // update the current mouse state.
    MouseX      = CurrentState.Mouse.X;
    MouseY      = CurrentState.Mouse.Y;
    MouseDeltaX = CurrentState.Mouse.X - PreviousState.Mouse.X;
    MouseDeltaY = CurrentState.Mouse.Y - PreviousState.Mouse.Y;
    ButtonsDownCount     = 0;
    ButtonsPressedCount  = 0;
    ButtonsReleasedCount = 0;
    curr    = CurrentState.Mouse.Buttons;
    prev    = PreviousState.Mouse.Buttons;
    changes = (curr    ^  prev);
    downs   = (changes &  curr);
    ups     = (changes & ~curr);
    for (size_t j = 0; j < 32; ++j)
    {
        mask = (1 << j);
        if (curr  & mask) ButtonsDown[ButtonsDownCount++] = j;
        if (downs & mask) ButtonsPressed[ButtonsPressedCount++] = j;
        if (ups   & mask) ButtonsReleased[ButtonsReleasedCount++] = j;
    }

    // generate events for the keyboard:
    KeysDownCount     = 0;
    KeysPressedCount  = 0;
    KeysReleasedCount = 0;
    for (size_t i = 0; i < 11; ++i)
    {
        curr = CurrentState.Keyboard.KeyStates[i];
        prev = PreviousState.Keyboard.KeyStates[i];
        changes = (curr    ^  prev);
        downs   = (changes &  curr);
        ups     = (changes & ~curr);

        for (size_t j = 0; j < 32; ++j)
        {
            uint32_t key = (i << 5) + j;
            mask = (1 << j);
            if (curr  & mask) KeysDown[KeysDownCount++] = key;
            if (downs & mask) KeysPressed[KeysPressedCount++] = key;
            if (ups   & mask) KeysReleased[KeysReleasedCount++] = key;
        }
    }

    // poll joystick state.
    ControllerCount = 0;
    memset(CurrentState.Controllers, 0, sizeof(controller_state_t) * MAX_CONTROLLERS);
    for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
    {
        if (glfwJoystickPresent(i) == GL_TRUE && ControllerCount < MAX_CONTROLLERS)
        {
            int           naxes    = 0;
            float const   *axes    = glfwGetJoystickAxes(i, &naxes);
            int           nbuttons = 0;
            uint8_t const *buttons = glfwGetJoystickButtons(i, &nbuttons);
            controller_state_t &cs = CurrentState.Controllers[ControllerCount];

            if (naxes > MAX_CONTROLLER_AXES)
                naxes = MAX_CONTROLLER_AXES;

            if (nbuttons > MAX_CONTROLLER_BUTTONS)
                nbuttons = MAX_CONTROLLER_BUTTONS;

            cs.Index        = uint32_t(i);
            cs.Connected    = 1;
            cs.AxisCount    = uint32_t(naxes);
            cs.ButtonCount  = uint32_t(nbuttons);
            for (uint32_t j = 0; j < cs.AxisCount; ++j)
            {
                cs.AxisValues[j] = axes[j];
            }
            for (uint32_t j = 0; j < cs.ButtonCount; ++j)
            {
                cs.ButtonValues[j] = buttons[j];
            }

            Controllers[ControllerCount++] = cs;
            if (ControllerCount == MAX_CONTROLLERS)
            {
                break;
            }
        }
    }

    // copy current state to the previous state.
    PreviousState = CurrentState;
}
