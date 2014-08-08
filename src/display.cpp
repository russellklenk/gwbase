/*/////////////////////////////////////////////////////////////////////////////
/// @summary
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include "display.hpp"

/*/////////////////
//   Constants   //
/////////////////*/

/*///////////////////////
//   Local Functions   //
///////////////////////*/

/*///////////////////////
//  Public Functions   //
///////////////////////*/
DisplayManager::DisplayManager(void)
    :
    main_window(NULL)
{
    /* empty */
}

DisplayManager::~DisplayManager(void)
{
    main_window = NULL;
}


bool DisplayManager::init(GLFWwindow *win)
{
    if (win != NULL)
    {
        main_window = win;
        return true;
    }
    else return false;
}

void DisplayManager::shutdown(void)
{
    main_window = NULL;
}
