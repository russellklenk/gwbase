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

#include "display.hpp"
#include "glsprite.hpp"
#include "glshader.hpp"
#include "math.hpp"

/*/////////////////
//   Constants   //
/////////////////*/
#define GW_WINDOW_WIDTH    800
#define GW_WINDOW_HEIGHT   600
#define GW_WINDOW_TITLE    "Geometry Wars"
#define GW_MIN_TIMESTEP    0.000001
#define GW_MAX_TIMESTEP    0.25
#define GW_SIM_TIMESTEP    1.0 / 120.0

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
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
}

/// @summary Executes a single game simulation tick to move all game entities.
/// Runs zero or more times per application tick at a fixed timestep.
/// @param currentTime The current absolute simulation time, in seconds. This
/// represents the time at which the current simulation tick started.
/// @param elapsedTime The time elapsed since the previous tick, in seconds.
static void simulate(double currentTime, double elapsedTime)
{
    UNUSED_ARG(currentTime);
    UNUSED_ARG(elapsedTime);
}

//// TEST CODE

static Texture *gTEX = NULL;
static Texture *gFTEX = NULL;
static SpriteBatch *gBATCH = NULL;
static SpriteFont *gFONT = NULL;

/*static GLuint            gPROGRAM;
static shader_desc_t     gSHADER;
static attribute_desc_t *aPTX = NULL;
static attribute_desc_t *aCLR = NULL;
static sampler_desc_t   *sTEX = NULL;
static uniform_desc_t   *uMSS = NULL;

static sprite_effect_t gEFFECT;
static sprite_batch_t  gBATCH;*/

static float DEGREES_PER_SEC = 180.0f;

/*static char const *gVSS =
    "#version 330\n"
    "uniform mat4 uMSS;\n"
    "layout (location = 0) in vec4 aPTX;\n"
    "layout (location = 1) in vec4 aCLR;\n"
    "out vec4 vCLR;\n"
    "out vec2 vTEX;\n"
    "void main() {\n"
    "    vCLR = aCLR;\n"
    "    vTEX = vec2(aPTX.z, aPTX.w);\n"
    "    gl_Position = uMSS * vec4(aPTX.x, aPTX.y, 0, 1);\n"
    "}\n";

static char const *gFSS =
    "#version 330\n"
    "uniform sampler2D sTEX;\n"
    "in  vec2 vTEX;\n"
    "in  vec4 vCLR;\n"
    "out vec4 oCLR;\n"
    "void main() {\n"
    "    oCLR = texture(sTEX, vTEX) * vCLR;\n"
    "}\n";

static void effect_setup(sprite_effect_t *effect, void *context)
{
    UNUSED_ARG(context);
    glUseProgram(gPROGRAM);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
    sprite_effect_bind_buffers(effect);
    sprite_effect_apply_blendstate(effect);
    set_uniform(uMSS, effect->Projection, false);
}

static void effect_apply_state(sprite_effect_t *effect, uint32_t state, void *context)
{
    UNUSED_ARG(effect);
    UNUSED_ARG(context);
    set_sampler(sTEX, GLuint(state));
}*/

//// TEST CODE

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

    //// TEST CODE

    glViewport(0, 0, width, height);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    rect_t src   = { 0, 0, gTEX->GetWidth(), gTEX->GetHeight() };
    float rgba[] = { 1.0f, 1.0f, 1.0f, 0.5f };

    gBATCH->SetBlendModeNone();
    gBATCH->SetViewport(width, height);

    gBATCH->Add(1, gTEX, width * 0.5f, height * 0.5f, src, rgba,
        rad(currentTime * DEGREES_PER_SEC),
        gTEX->GetWidth() * 0.5f, gTEX->GetHeight() * 0.5f,
        1.0f, 1.0f);

    gBATCH->SetBlendModeAlpha();
    float rgba2[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    gFONT->Draw("Hello, world!", 0, 0, 1, rgba2, gBATCH);

    gBATCH->Flush();

    /*flush_sprite_batch(&gBATCH);

    sprite_t sprite;
    sprite.ScreenX       = width  * 0.5f;
    sprite.ScreenY       = height * 0.5f;
    sprite.OriginX       = gTEX->GetWidth()  * 0.5f;
    sprite.OriginY       = gTEX->GetHeight() * 0.5f;
    sprite.ScaleX        = 1.0f;
    sprite.ScaleY        = 1.0f;
    sprite.Orientation   = rad(currentTime * DEGREES_PER_SEC);
    sprite.TintColor     = 0xFFFFFFFFU;
    sprite.ImageX        = 0;
    sprite.ImageY        = 0;
    sprite.ImageWidth    = gTEX->GetWidth();
    sprite.ImageHeight   = gTEX->GetHeight();
    sprite.TextureWidth  = gTEX->GetWidth();
    sprite.TextureHeight = gTEX->GetHeight();
    sprite.LayerDepth    = 1;
    sprite.RenderState   = uint32_t(gTEX->GetId());

    ensure_sprite_batch(&gBATCH, 1);
    generate_quads(gBATCH.Quads, gBATCH.State, gBATCH.Order, 0, &sprite, 0, 1);
    gBATCH.Count = 1;

    sprite_effect_set_viewport(&gEFFECT, width, height);
    sprite_effect_apply_t fxfuncs = {
        effect_setup,
        effect_apply_state
    };
    sprite_effect_draw_batch_ptc(&gEFFECT, &gBATCH, &fxfuncs, NULL);*/

    //// TEST CODE
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


    //// TEST CODE

    gTEX = new Texture();
    if (gTEX->LoadFromFile("assets/test_bg.tga"))
    {
        printf("Texture loaded successfully.\n");
    }
    else printf("Texture failed to load.\n");

    gFTEX = new Texture();
    if (gFTEX->LoadFromFile("assets/font.tga"))
    {
        printf("Font texture loaded successfully.\n");
    }
    else printf("Font texture failed to load.\n");

    gFONT = new SpriteFont();
    gFONT->SetSource(gFTEX, 8, 12, 6, 10, ' ', '~');

    gBATCH = new SpriteBatch(1024);

    /*shader_source_t     sources;
    shader_source_init(&sources);
    shader_source_add (&sources, GL_VERTEX_SHADER,   (char**) &gVSS, 1);
    shader_source_add (&sources, GL_FRAGMENT_SHADER, (char**) &gFSS, 1);
    if (build_shader(&sources, &gSHADER, &gPROGRAM))
    {
        printf("Shader Code compiled successfully.\n");
        aPTX = find_attribute(&gSHADER, "aPTX");
        aCLR = find_attribute(&gSHADER, "aCLR");
        sTEX = find_sampler  (&gSHADER, "sTEX");
        uMSS = find_uniform  (&gSHADER, "uMSS");
    }
    else
    {
        printf("Shader Code failed to compile.\n");
    }
    if (create_sprite_effect(&gEFFECT, 1024, sizeof(sprite_vertex_ptc_t), sizeof(uint16_t)))
    {
        sprite_effect_setup_vao_ptc(&gEFFECT);
        printf("Created sprite effect.\n");
    }
    else
    {
        printf("Failed to create sprite effect.\n");
    }
    create_sprite_batch(&gBATCH, 1);*/

    //// TEST CODE

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

    //// TEST CODE

    /*delete_sprite_batch(&gBATCH);
    delete_sprite_effect(&gEFFECT);
    shader_desc_free(&gSHADER);
    glDeleteProgram(gPROGRAM);*/

    delete gBATCH;
    delete gFONT;
    delete gFTEX;
    delete gTEX;

    //// TEST CODE

    // perform any top-level cleanup.
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
