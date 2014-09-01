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

#include "math.hpp"
#include "input.hpp"
#include "display.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "ll_sprite.hpp"
#include "ll_shader.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
#define GW_WINDOW_WIDTH    800
#define GW_WINDOW_HEIGHT   600
#define GW_WINDOW_TITLE    "Geometry Wars"
#define GW_MIN_TIMESTEP    0.000001
#define GW_MAX_TIMESTEP    0.25
#define GW_SIM_TIMESTEP    1.0 / 120.0

/*///////////////
//   Globals   //
///////////////*/
static EntityManager  *gEntityManager  = NULL;
static DisplayManager *gDisplayManager = NULL;
static InputManager   *gInputManager   = NULL;

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

/// @summary Executes all of the logic associated with game user input. This
/// is also where we would run user interface logic. Runs once per application tick.
/// @param currentTime The current absolute time, in seconds. This represents
/// the time at which the current tick started.
/// @param elapsedTime The time elapsed since the previous tick, in seconds.
static void input(double currentTime, double elapsedTime)
{
    gInputManager->Update(currentTime, elapsedTime);
    gEntityManager->Input(currentTime, elapsedTime, gInputManager);
}

/// @summary Executes a single game simulation tick to move all game entities.
/// Runs zero or more times per application tick at a fixed timestep.
/// @param currentTime The current absolute simulation time, in seconds. This
/// represents the time at which the current simulation tick started.
/// @param elapsedTime The time elapsed since the previous tick, in seconds.
static void simulate(double currentTime, double elapsedTime)
{
    gEntityManager->Update(currentTime, elapsedTime);
}

/// @summary Submits a single frame to the GPU for rendering. Runs once per
/// application tick at a variable timestep.
/// @param currentTime The current absolute time, in seconds. This represents
/// the time at which the current tick started.
/// @param elapsedTime The time elapsed since the previous tick, in seconds.
/// @param t A value in [0, 1] indicating how far into the current simulation
/// step we are at the time the frame is generated.
/// @param width The width of the default framebuffer, in pixels.
/// @param height The height of the default framebuffer, in pixels.
static void render(double currentTime, double elapsedTime, double t, int width, int height)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
    UNUSED_ARG(t);

    DisplayManager *dm    = gDisplayManager;
    SpriteBatch    *batch = dm->GetBatch();
    SpriteFont     *font  = dm->GetFont();
    float           rgba[]= {1.0f, 0.0f, 0.0f, 1.0f};

    dm->SetViewport(width, height);
    dm->Clear(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0);
    dm->BeginFrame();
    batch->SetBlendModeAlpha();
    font->Draw("Hello, world!", 0, 0, 1, rgba, 5.0f, 5.0f, batch);
    gEntityManager->Draw(currentTime, elapsedTime, dm);
    dm->EndFrame();
}

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

    // initialize global managers:
    gDisplayManager = new DisplayManager();
    gDisplayManager->Init(window);
    gInputManager = new InputManager();
    gInputManager->Init(window);

    Player *player = new Player(0);
    player->Init(gDisplayManager);
    gEntityManager = new EntityManager();
    gEntityManager->AddEntity(player);

    // game loop setup and run:
    const double   Step = GW_SIM_TIMESTEP;
    double previousTime = glfwGetTime();
    double currentTime  = previousTime;
    double elapsedTime  = 0.0;
    double accumulator  = 0.0;
    double simTime      = 0.0;
    double t            = 0.0;
    int    width        = 0;
    int    height       = 0;

    while (!glfwWindowShouldClose(window))
    {
        // retrieve the current framebuffer size, which
        // may be different from the current window size.
        glfwGetFramebufferSize(window, &width, &height);

        // update the main game clock.
        previousTime = currentTime;
        currentTime  = glfwGetTime();
        elapsedTime  = currentTime - previousTime;
        if (elapsedTime > GW_MAX_TIMESTEP)
        {
            elapsedTime = GW_MAX_TIMESTEP;
        }
        if (elapsedTime < GW_MIN_TIMESTEP)
        {
            elapsedTime = GW_MIN_TIMESTEP;
        }
        accumulator += elapsedTime;

        // process user input at the start of the frame.
        input(currentTime, elapsedTime);

        // execute the simulation zero or more times per-frame.
        // the simulation runs at a fixed timestep.
        while (accumulator >= Step)
        {
            // @todo: swap game state buffers here.
            // pass the current game state to simulate.
            simulate(simTime, Step);
            accumulator -= Step;
            simTime += Step;
        }

        // interpolate display state.
        t = accumulator / Step;
        // state = currentState * t + previousState * (1.0 - t);
        render(currentTime, elapsedTime, t, width, height);

        // now present the current frame and process OS events.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // teardown global managers.
    delete gEntityManager;
    delete gDisplayManager;
    delete gInputManager;

    // perform any top-level cleanup.
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
