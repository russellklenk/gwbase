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
/// @summary The global InputManager instance.
InputManager* InputManager::IM = NULL;

/*///////////////////////
//   Local Functions   //
///////////////////////*/
static uint32_t controller_bitmap(input_snapshot_t const *state)
{
    uint32_t bitmap = 0;
    for (size_t i = 0; i < state->ControllerCount; ++i)
    {
        bitmap |= (1 << state->ControllerIds[i]);
    }
    return bitmap;
}

/*///////////////////////
//  Public Functions   //
///////////////////////*/
InputManager* InputManager::GetInstance(void)
{
    return IM;
}

InputManager::InputManager(void)
    :
    MainWindow(NULL),
    ConnectEvents(0),
    DisconnectEvents(0),
    MouseDeltaX(0.0f),
    MouseDeltaY(0.0f)
{
    InputManager::IM = this;
}

InputManager::~InputManager(void)
{
    Shutdown();
    InputManager::IM = NULL;
}


bool InputManager::Init(GLFWwindow *win)
{
    if (win != NULL)
    {
        MainWindow  = win;
        MouseDeltaX = 0.0f;
        MouseDeltaY = 0.0f;
        ConnectEvents = 0;
        DisconnectEvents = 0;
        memset(&CurrentState,  0, sizeof(input_snapshot_t));
        memset(&PreviousState, 0, sizeof(input_snapshot_t));
        input_attach(win);
        return true;
    }
    else return false;
}

void InputManager::Shutdown(void)
{
    input_detach(MainWindow);
}

bool InputManager::IsKeyUp(int key) const
{
    size_t   word = (key - INPUT_KEY_OFFSET) >> 5;
    uint32_t bitx = (key - INPUT_KEY_OFFSET) & 0x1F;
    uint32_t mask = (1  << bitx);
    return (CurrentState.KeyboardState[word] & mask) == 0;
}

bool InputManager::IsKeyDown(int key) const
{
    size_t   word = (key - INPUT_KEY_OFFSET) >> 5;
    uint32_t bitx = (key - INPUT_KEY_OFFSET) & 0x1F;
    uint32_t mask = (1  << bitx);
    return (CurrentState.KeyboardState[word] & mask) != 0;
}

bool InputManager::IsMouseButtonUp(int button) const
{
    uint32_t mask = (1 << button);
    return (CurrentState.MouseState & mask) == 0;
}

bool InputManager::IsMouseButtonDown(int button) const
{
    uint32_t mask = (1 << button);
    return (CurrentState.MouseState & mask) != 0;
}

bool InputManager::IsControllerConnected(int id) const
{
    int const *ids = CurrentState.ControllerIds;
    for (size_t  i = 0; i < CurrentState.ControllerCount; ++i)
    {
        if (ids[i] == id)
            return true;
    }
    return false;
}

bool InputManager::WasKeyPressed(int key) const
{
    size_t   word    = (key - INPUT_KEY_OFFSET) >> 5;
    uint32_t bitx    = (key - INPUT_KEY_OFFSET) & 0x1F;
    uint32_t mask    = (1  << bitx);
    uint32_t curr    = CurrentState.KeyboardState[word];
    uint32_t prev    = PreviousState.KeyboardState[word];
    uint32_t changes = (curr ^ prev);
    return ((changes & curr) & mask) != 0;
}

bool InputManager::WasKeyReleased(int key) const
{
    size_t   word    = (key - INPUT_KEY_OFFSET) >> 5;
    uint32_t bitx    = (key - INPUT_KEY_OFFSET) & 0x1F;
    uint32_t mask    = (1  << bitx);
    uint32_t curr    = CurrentState.KeyboardState[word];
    uint32_t prev    = PreviousState.KeyboardState[word];
    uint32_t changes = (curr  ^ prev);
    return ((changes & ~curr) & mask) != 0;
}

bool InputManager::WasMouseButtonPressed(int button) const
{
    uint32_t mask    = (1 << button);
    uint32_t curr    = CurrentState.MouseState;
    uint32_t prev    = PreviousState.MouseState;
    uint32_t changes = (curr ^ prev);
    return ((changes & curr) & mask) != 0;
}

bool InputManager::WasMouseButtonReleased(int button) const
{
    uint32_t mask    = (1 << button);
    uint32_t curr    = CurrentState.MouseState;
    uint32_t prev    = PreviousState.MouseState;
    uint32_t changes = (curr  ^ prev);
    return ((changes & ~curr) & mask) != 0;
}

bool InputManager::WasControllerConnected(int id) const
{
    uint32_t mask = (1 << id);
    return (ConnectEvents & mask) != 0;
}

bool InputManager::WasControllerDisconnected(int id) const
{
    uint32_t mask = (1 << id);
    return (DisconnectEvents & mask) != 0;
}

void InputManager::GetController(int id, controller_state_t *prev, controller_state_t *curr) const
{
    curr->ControllerId = id;
    curr->IsAttached   = false;
    curr->AxisCount    = 0;
    curr->ButtonCount  = 0;
    curr->AxisValues   = NULL;
    curr->ButtonValues = NULL;
    for (size_t i = 0; i < CurrentState.ControllerCount; ++i)
    {
        if (CurrentState.ControllerIds[i] == id)
        {
            curr->IsAttached   = true;
            curr->AxisCount    = CurrentState.ControllerAxisCount[i];
            curr->ButtonCount  = CurrentState.ControllerButtonCount[i];
            curr->AxisValues   = CurrentState.ControllerAxes[i];
            curr->ButtonValues = CurrentState.ControllerButtons[i];
        }
    }

    prev->ControllerId = id;
    prev->IsAttached   = false;
    prev->AxisCount    = 0;
    prev->ButtonCount  = 0;
    prev->AxisValues   = NULL;
    prev->ButtonValues = NULL;
    for (size_t i = 0; i < PreviousState.ControllerCount; ++i)
    {
        if (PreviousState.ControllerIds[i] == id)
        {
            prev->IsAttached   = true;
            prev->AxisCount    = PreviousState.ControllerAxisCount[i];
            prev->ButtonCount  = PreviousState.ControllerButtonCount[i];
            prev->AxisValues   = PreviousState.ControllerAxes[i];
            prev->ButtonValues = PreviousState.ControllerButtons[i];
        }
    }
}

void InputManager::Update(double currentTime, double elapsedTime)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);

    // current state becomes previous state, and the new current state is retrieved.
    memcpy(&PreviousState, &CurrentState, sizeof(input_snapshot_t));
    input_snapshot(&CurrentState, MainWindow);
    MouseDeltaX = CurrentState.MouseX - PreviousState.MouseX;
    MouseDeltaY = CurrentState.MouseY - PreviousState.MouseY;

    // determine whether any controllers have been connected or disconnected.
    uint32_t curr       = controller_bitmap(&CurrentState);
    uint32_t prev       = controller_bitmap(&PreviousState);
    uint32_t changes    = (curr    ^  prev);
    uint32_t connect    = (changes &  curr);
    uint32_t disconnect = (changes & ~curr);
    ConnectEvents       = connect;
    DisconnectEvents    = disconnect;
}
