/*/////////////////////////////////////////////////////////////////////////////
/// @summary Include the correct OpenGL headers based on the target platform.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

#ifndef GW_PLATFORM_HPP
#define GW_PLATFORM_HPP

/*////////////////
//   Includes   //
////////////////*/
// we include GLEW with the distribution; include it if requested. if it is
// detected that GLEW is being used, #define GL_EXTERNAL so that platform GL
// headers are not #included in the block of code below.
#if !defined(__glew_h__) && !defined(__GLEW_H__)
    #ifdef GL_USE_GLEW
        #include "GL/glew.h"
        #ifndef  GL_EXTERNAL
        #define  GL_EXTERNAL
        #endif
    #endif
#else
    #ifndef GL_EXTERNAL
    #define GL_EXTERNAL
    #endif
#endif

// the user may be employing a third-party extension library, like GLEW,
// that may forbid direct inclusion of gl.h. in that case, #define GL_EXTERNAL
// to 1, and the replacement headers should be included prior to this header.
#ifndef GL_EXTERNAL
    #if defined(__APPLE__)
        #include <OpenGL/OpenGL.h>
        #include <OpenGL/gl3.h>
        #include <OpenGL/gl3ext.h>
    #elif defined(WIN32) || defined(WIN64) || defined(_WINDOWS)
        #define  GL_GLEXT_PROTOTYPES
        #include <windows.h>
        #include <GL/gl.h>
        #include "GL/glext.h"
    #else
        #define  GL_GLEXT_PROTOTYPES
        #include <GL/gl.h>
        #include "GL/glext.h"
    #endif
#endif

// pull in GLFW3 to abstract window creation, user input, and timing.
// GLFW should be included after the system OpenGL headers.
#include "GLFW/glfw3.h"

// pull in platform OpenAL headers for sound support.
#if defined(__APPLE__)
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
#else
    #include <AL/al.h>
    #include <AL/alc.h>
#endif

#endif /* !defined(GW_PLATFORM_HPP) */
