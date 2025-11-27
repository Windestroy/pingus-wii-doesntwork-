// Pingus Wii Main Entry Point
// This file provides Wii-specific initialization and calls the main Pingus code

#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

// Wii-specific video setup
static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

// Wii controller state tracking
static u32 wii_buttons_prev = 0;
static expansion_t wii_expansion_prev;

// Pingus thread
static SDL_Thread *pingus_thread = NULL;
static int pingus_result = 0;
static bool pingus_running = false;

// Thread parameters structure
struct ThreadParams {
    int argc;
    char** argv;
};

// Forward declaration of Pingus main
extern "C" int pingus_main(int argc, char** argv);

// Wii initialization
static void Wii_Init()
{
    // Initialize video system
    VIDEO_Init();

    // Get preferred video mode
    rmode = VIDEO_GetPreferredMode(NULL);

    // Allocate framebuffer
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    // Initialize console for debugging (optional)
    console_init(xfb, 20, 20, rmode->fbWidth - 20, rmode->xfbHeight - 20,
                 rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    // Configure video
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_ClearFrameBuffer(rmode, xfb, COLOR_BLACK);
    VIDEO_SetBlack(false);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

    // Initialize controllers
    WPAD_Init();

    // Initialize SD card for data access
    fatInitDefault();

    // Initialize SDL for Wii
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

    // Set SDL video mode to match Wii framebuffer
    SDL_SetVideoMode(rmode->fbWidth, rmode->efbHeight, 24, 0);
}

// Thread function to run Pingus
static int PingusThread(void *ptr)
{
    ThreadParams *params = (ThreadParams*)ptr;

    pingus_result = pingus_main(params->argc, params->argv);
    pingus_running = false;
    return pingus_result;
}

// Wii controller input handling - converts Wii inputs to SDL events
static void Wii_HandleInput()
{
    WPAD_ScanPads();

    u32 wii_buttons = WPAD_ButtonsDown(0);
    expansion_t expansion;
    WPAD_Expansion(0, &expansion);

    SDL_Event event;

    // Handle D-pad as arrow keys
    // Up arrow
    if ((wii_buttons & WPAD_BUTTON_UP) && !(wii_buttons_prev & WPAD_BUTTON_UP)) {
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_UP;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_UP) && (wii_buttons_prev & WPAD_BUTTON_UP)) {
        event.type = SDL_KEYUP;
        event.key.keysym.sym = SDLK_UP;
        SDL_PushEvent(&event);
    }

    // Down arrow
    if ((wii_buttons & WPAD_BUTTON_DOWN) && !(wii_buttons_prev & WPAD_BUTTON_DOWN)) {
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_DOWN;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_DOWN) && (wii_buttons_prev & WPAD_BUTTON_DOWN)) {
        event.type = SDL_KEYUP;
        event.key.keysym.sym = SDLK_DOWN;
        SDL_PushEvent(&event);
    }

    // Left arrow
    if ((wii_buttons & WPAD_BUTTON_LEFT) && !(wii_buttons_prev & WPAD_BUTTON_LEFT)) {
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_LEFT;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_LEFT) && (wii_buttons_prev & WPAD_BUTTON_LEFT)) {
        event.type = SDL_KEYUP;
        event.key.keysym.sym = SDLK_LEFT;
        SDL_PushEvent(&event);
    }

    // Right arrow
    if ((wii_buttons & WPAD_BUTTON_RIGHT) && !(wii_buttons_prev & WPAD_BUTTON_RIGHT)) {
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_RIGHT;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_RIGHT) && (wii_buttons_prev & WPAD_BUTTON_RIGHT)) {
        event.type = SDL_KEYUP;
        event.key.keysym.sym = SDLK_RIGHT;
        SDL_PushEvent(&event);
    }

    // A button as left mouse click
    if ((wii_buttons & WPAD_BUTTON_A) && !(wii_buttons_prev & WPAD_BUTTON_A)) {
        event.type = SDL_MOUSEBUTTONDOWN;
        event.button.button = SDL_BUTTON_LEFT;
        event.button.x = 320; // Center of screen
        event.button.y = 240;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_A) && (wii_buttons_prev & WPAD_BUTTON_A)) {
        event.type = SDL_MOUSEBUTTONUP;
        event.button.button = SDL_BUTTON_LEFT;
        event.button.x = 320;
        event.button.y = 240;
        SDL_PushEvent(&event);
    }

    // Minus button as Escape key
    if ((wii_buttons & WPAD_BUTTON_MINUS) && !(wii_buttons_prev & WPAD_BUTTON_MINUS)) {
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_ESCAPE;
        SDL_PushEvent(&event);
    } else if (!(wii_buttons & WPAD_BUTTON_MINUS) && (wii_buttons_prev & WPAD_BUTTON_MINUS)) {
        event.type = SDL_KEYUP;
        event.key.keysym.sym = SDLK_ESCAPE;
        SDL_PushEvent(&event);
    }

    // Update previous state
    wii_buttons_prev = wii_buttons;
    wii_expansion_prev = expansion;
}

// Wii cleanup
static void Wii_Deinit()
{
    SDL_Quit();
    WPAD_Shutdown();
}

// Main entry point
extern "C" int main(int argc, char **argv)
{
    // Initialize Wii hardware
    Wii_Init();

    // Set up arguments for Pingus
    const char* pingus_argv[] = {
        "pingus",
        "--datadir", "sd:/apps/pingus/data",
        NULL
    };
    int pingus_argc = 3;

    // Prepare thread parameters
    ThreadParams params = { pingus_argc, const_cast<char**>(pingus_argv) };

    // Start Pingus in a separate thread
    pingus_running = true;
    pingus_thread = SDL_CreateThread(PingusThread, &params);

    if (pingus_thread == NULL) {
        printf("Unable to create Pingus thread: %s\n", SDL_GetError());
        Wii_Deinit();
        return 1;
    }

    // Main Wii input handling loop
    while (pingus_running) {
        Wii_HandleInput();

        // Check for HOME button to exit
        u32 wii_buttons = WPAD_ButtonsHeld(0);
        if (wii_buttons & WPAD_BUTTON_HOME) {
            pingus_running = false;
        }

        // Small delay to prevent excessive CPU usage
        SDL_Delay(16); // ~60 FPS
    }

    // Wait for Pingus thread to finish
    SDL_WaitThread(pingus_thread, NULL);

    // Cleanup
    Wii_Deinit();

    return pingus_result;
}
