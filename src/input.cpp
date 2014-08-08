/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "input.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
InputManager::InputManager(void)
    :
    main_window(NULL)
{
    /* empty */
}

InputManager::~InputManager(void)
{
    main_window = NULL;
}


bool InputManager::init(GLFWwindow *win)
{
    if (win != NULL)
    {
        main_window = win;
        return true;
    }
    else return false;
}

void InputManager::shutdown(void)
{
    main_window = NULL;
}
