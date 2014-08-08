/*/////////////////////////////////////////////////////////////////////////////
/// @summary Implements the entry point of the application. This handles the
/// setup of our third party libraries and the creation of our main window and
/// rendering context, along with implementing the game loop.
/// @author Russell Klenk (contact@russellklenk.com)
///////////////////////////////////////////////////////////////////////////80*/

/*////////////////
//   Includes   //
////////////////*/
#include <stdio.h>
#include <stdlib.h>

#include "common.hpp"
#include "platform.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
#define GW_WINDOW_WIDTH    800
#define GW_WINDOW_HEIGHT   600
#define GW_WINDOW_TITLE    "Geometry Wars"

/*///////////////////////
//   Local Functions   //
///////////////////////*/
/// @summary Callback to handle a GLFW error. Prints the error information to stderr.
/// @param error_code The internal GLFW error code.
/// @param error_desc A textual description of the error.
static void glfw_error(int error_code, char const *error_desc)
{
    fprintf(stderr, "ERROR: (GLFW code 0x%08X): %s\n", error_code, error_desc);
}

#if GL_DEBUG_ENABLE
/// @summary Callback to handle output from the GL_ARB_debug_output extension,
/// which is of course not supported on OSX as of 10.9.
/// @param source
/// @param type
/// @param id
/// @param severity
/// @param length
/// @param message
/// @param context
static void gl_arb_debug(
    GLenum      source,
    GLenum      type,
    GLuint      id,
    GLenum      severity,
    GLsizei     length,
    char const *message,
    void       *context)
{
    UNUSED_ARG(source);
    UNUSED_ARG(type);
    UNUSED_ARG(id);
    UNUSED_ARG(severity);
    UNUSED_ARG(length);
    UNUSED_ARG(context);
    fprintf(stdout, "ARB_debug: %s\n", message);
}
#endif

/*///////////////////////
//  Public Functions   //
///////////////////////*/
int main(int argc, char **argv)
{
    GLFWwindow *window = NULL;

    UNUSED_ARG(argc);
    UNUSED_ARG(argv);

    // initialize GLFW, our platform abstraction library.
    glfwSetErrorCallback(glfw_error);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_VISIBLE,    GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE,  GL_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#if GL_DEBUG_ENABLE
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);
#endif

    // create the main application window and OpenGL context.
    window = glfwCreateWindow(GW_WINDOW_WIDTH, GW_WINDOW_HEIGHT, GW_WINDOW_TITLE, NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "ERROR: Cannot create primary GLFW window.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // now that we have an OpenGL context, load extensions provided by the platform.
    // note that glewExperimental is defined by the GLEW library and is required on
    // OSX or the glGenVertexArrays() call will cause a fault.
    glewExperimental = GL_TRUE;
    if (glewInit()  != GLEW_OK)
    {
        fprintf(stderr, "ERROR: Cannot initialize GLEW for the primary context.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // clear any OpenGL error status and configure debug output.
    glGetError();
#if GL_DEBUG_ENABLE
    if (GLEW_ARB_debug_output)
    {
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(gl_arb_debug, NULL);
    }
#endif

    // run the game loop.
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // perform any top-level cleanup.
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
