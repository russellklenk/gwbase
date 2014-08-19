/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements a rectangle packer using an (unbalanced) binary tree
/// subdividing the space in the master rectangle. The algorithm is described
/// at http://www.blackpawn.com/texts/lightmaps.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "ll_input.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
#define MAX_INPUT_CONTEXTS    4U

/*///////////////
//   Globals   //
///////////////*/
/// @summary A global table of all input contexts. Nasty, but GLFW doesn't
/// allow us to pass through a context pointer in the callbacks, and we don't
/// want to take over the user pointer on the window.
static input_context_t gContextList[MAX_INPUT_CONTEXTS];

/// @summary A global table of GLFW window handles associated with input contexts.
static GLFWwindow*     gWindowList[MAX_INPUT_CONTEXTS];

/// @summary The number of valid input context records.
static size_t          gContextCount = 0;

/*///////////////////////
//   Local Functions   //
///////////////////////*/
/// @summary Allocates a new input context for the specified window.
/// @param win The window associated with the input context.
/// @return The input context record allocated to the window, or NULL.
static input_context_t* alloc_context(GLFWwindow *win)
{
    if (gContextCount == MAX_INPUT_CONTEXTS)
        return NULL;

    input_context_t *ctx = &gContextList[gContextCount];
    ctx->Window            = win;
    ctx->MouseX            = 0;
    ctx->MouseY            = 0;
    ctx->MouseState        = 0;
    ctx->MouseModifiers    = 0;
    ctx->KeyboardModifiers = 0;
    for (size_t i = 0; i < INPUT_KEY_WORDS; ++i)
        ctx->KeyboardState[i] = 0;

    gWindowList[gContextCount++] = win;
    return ctx;
}

/// @summary Finds the input context attached to a given window.
/// @param win The window associated with the input context.
/// @return The input context record allocated to the window, or NULL.
static input_context_t* find_context(GLFWwindow *win)
{
    for (size_t i = 0; i < gContextCount; ++i)
    {
        if (gWindowList[i] == win)
            return &gContextList[i];
    }
    return NULL;
}

/// @summary Releases the input context associated with a given window.
/// @param win The window associated with the input context.
/// @return true if a context was associated with win.
static bool free_context(GLFWwindow *win)
{
    for (size_t i = 0; i < gContextCount; ++i)
    {
        if (gWindowList[i] == win)
        {
            if (i == (gContextCount - 1))
            {
                // poppiing the context from the back of the list.
                gContextCount--;
                return true;
            }
            else
            {
                gWindowList [i] = gWindowList [gContextCount-1];
                gContextList[i] = gContextList[gContextCount-1];
                gContextCount--;
                return true;
            }
        }
    }
    return false;
}

/// @summary Callback invoked by GLFW to report the current cursor position.
/// @param win The window reporting with the event.
/// @param x The current position of the mouse cursor, relative to the window client area.
/// @param y The current position of the mouse cursor, relative to the window client area.
static void glfw_cursor(GLFWwindow *win, double x, double y)
{
    input_context_t *ctx = find_context(win);
    if (ctx)
    {
        ctx->MouseX = float(x);
        ctx->MouseY = float(y);
    }
}

/// @summary Callback invoked by GLFW to report that a mouse button was pressed or released.
/// @param win The window reporting the event.
/// @param button The button identifier.
/// @param action One of GLFW_PRESS or GLFW_RELEASE.
/// @param modifiers A combination of GLFW_MOD_x flags, or zero, indicating which modifiers are active.
static void glfw_button(GLFWwindow *win, int button, int action, int modifiers)
{
    input_context_t *ctx = find_context(win);
    if (ctx)
    {
        if (action == GLFW_PRESS)
        {
            ctx->MouseState    |= (1 << button);
            ctx->MouseModifiers = modifiers;
        }
        else
        {
            ctx->MouseState    &=~(1 << button);
            ctx->MouseModifiers = 0;
        }
    }
}

/// @summary Callback invoked by GLFW to report a keyboard event.
/// @param win The window reporting the event.
/// @param key The GLFW key identifier.
/// @param scancode The raw key scancode reported by the system.
/// @param action One of GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT (ignored).
/// @param modifiers A combination of GLFW_MOD_x flags, or zero, indicating which modifiers are active.
static void glfw_key(GLFWwindow *win, int key, int scancode, int action, int modifiers)
{
    UNUSED_ARG(scancode);
    input_context_t *ctx = find_context(win);
    if (ctx)
    {
        size_t   word = (key - INPUT_KEY_OFFSET) >> 5;
        uint32_t bitx = (key - INPUT_KEY_OFFSET) & 0x1F;

        if (action == GLFW_PRESS)
        {
            ctx->KeyboardState[word] |= (1 << bitx);
            ctx->KeyboardModifiers    = modifiers;
        }
        else if (action == GLFW_RELEASE)
        {
            ctx->KeyboardState[word] &=~(1 << bitx);
            ctx->KeyboardModifiers    = 0;
        }
    }
}

/*///////////////////////
//  Public Functions   //
///////////////////////*/
bool input_attach(GLFWwindow *win)
{
    input_context_t *ctx = find_context(win);
    if (ctx)
    {
        // already attached to this window.
        return true;
    }

    ctx = alloc_context(win);
    if (ctx)
    {
        glfwSetKeyCallback(win, glfw_key);
        glfwSetCursorPosCallback(win, glfw_cursor);
        glfwSetMouseButtonCallback(win, glfw_button);
        return true;
    }
    else return false;
}

void input_detach(GLFWwindow *win)
{
    if (free_context(win))
    {
        glfwSetKeyCallback(win, NULL);
        glfwSetCursorPosCallback(win, NULL);
        glfwSetMouseButtonCallback(win, NULL);
    }
}

void input_snapshot(input_snapshot_t *dst, GLFWwindow *win)
{
    input_context_t *ctx = find_context(win);
    if (ctx)
    {
        // copy event-driven attributes from the context.
        dst->Window            = ctx->Window;
        dst->MouseX            = ctx->MouseX;
        dst->MouseY            = ctx->MouseY;
        dst->MouseState        = ctx->MouseState;
        dst->MouseModifiers    = ctx->MouseModifiers;
        dst->KeyboardModifiers = ctx->KeyboardModifiers;
        for (size_t i = 0; i < INPUT_KEY_WORDS; ++i)
            dst->KeyboardState[i] = ctx->KeyboardState[i];

        // poll for the current joystick state.
        size_t  ncontrollers = 0;
        dst->ControllerCount = 0;
        for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i)
        {
            if (glfwJoystickPresent(i) == GL_TRUE)
            {
                int naxes    = 0;
                int nbuttons = 0;
                float const *axes = glfwGetJoystickAxes(i, &naxes);
                unsigned char const *buttons = glfwGetJoystickButtons(i, &nbuttons);

                if (naxes > INPUT_MAX_CONTROLLER_AXES)
                    naxes = INPUT_MAX_CONTROLLER_AXES;

                if (nbuttons > INPUT_MAX_CONTROLLER_BUTTONS)
                    nbuttons = INPUT_MAX_CONTROLLER_BUTTONS;

                dst->ControllerIds[ncontrollers] = i;
                dst->ControllerAxisCount[ncontrollers] = size_t(naxes);
                dst->ControllerButtonCount[ncontrollers] = size_t(nbuttons);

                for (int j = 0; j < naxes; ++j)
                {
                    dst->ControllerAxes[ncontrollers][j] = axes[j];
                }
                for (int j = 0; j < nbuttons; ++j)
                {
                    dst->ControllerButtons[ncontrollers][j] = buttons[j];
                }
                ncontrollers++;

                if (ncontrollers == INPUT_MAX_CONTROLLERS)
                    break;
            }
        }
        dst->ControllerCount = ncontrollers;
    }
}
