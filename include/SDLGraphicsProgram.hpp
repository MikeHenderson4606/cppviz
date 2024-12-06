#ifndef SDLGRAPHICSPROGRAM_HPP
#define SDLGRAPHICSPROGRAM_HPP

// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif

// Third party libraries
#include <glad/glad.h> // The glad library helps setup OpenGL extensions.

// C++ Standard Libraries
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Any of our user libraries
// TBD (e.g. #include "someheader.hpp")

// Purpose:
// This class sets up a full graphics program using SDL.
// We also initialize OpenGL in this class.
// 
// Finally, this is where our inifinite loop of our program
// will take place, constantly updating and rendering our 2D
// and 3D objects in the 'Loop' function. 
class SDLGraphicsProgram{
public:

    // Constructor
    // Set the dimensions of the window.
    // The context type tells us what type of graphics program that we
    // are trying to build.
    // Currently:
    //           - 0 = SDL window
    //           - 1 = OpenGL window
    //           - 2 and beyond reserved for Vulkan, Metal, etc.
    SDLGraphicsProgram(int windowWidth, int windowHeight, int contextType);
    // Desctructor
    ~SDLGraphicsProgram();
    // Create a regular SDL window and will use SDL Software Renderer. 
    void SDLCreateWindow();
    // Creates a SDL window setup for OpenGL.
    void SDLCreateOpenGLWindow();

    // These function pointers setup the functions that will be called
    // in the main loop of our program.
    // They take in functions that are 'void' and take in no arguments(void argument
    void SetPreLoopCallback(void (*preLoopfunc)(void)); // Called once before the main loop
    void SetInputCallback(void   (*inputFunc)(void));   // Handles keyboard, mouse, joystick, etc. events.
    void SetUpdateCallback(void (*updateFunc)(void));   // Called every frame and handles updates (e.g. movement)
    void SetRenderCallback(void (*renderFunc)(void));   // Called every frame and draws the graphics

    // loop that runs forever
    // The Loop function calls Input, then update, and then render
    // in an infinite loop. 
    // Depending on if you are using OpenGL as a renderer, the correct
    // functions will also be called to 'swap' the window.
    void Loop();
    // Terminates the main program loop
    void TerminateLoop();
    // Get Pointer to Window
    SDL_Window* GetSDLWindowPointer();
    // Helper Function to Query OpenGL information.
    void GetOpenGLVersionInfo(); 
    // Helper function to setup some default options for rendering
    // in OpenGL.
    void SetDefaultOpenGLState();
    // Helper function to set the background clear color
    void SetClearColor(float r, float g, float b, float a);
    // Helper function to clear the contents being rendered
    // into the window
    void ClearWindow();
private: 
    // Setup OpenGL
	// This sets up the 'OpenGL' context. The OpenGL context keeps track
	// of OpenGLs state machine and the functionality that it is allowed
    // to use.
    bool InitGL(); // Should only be called internally from SDLCreateOpenGLWindow()

public:
    // Window dimensions
    int m_windowWidth;
    int m_windowHeight;

private:
    // Function pointers to the for functions that are called.
    // The way these work is that you set them in another function
    //
    // Note: pfn_ is a common prefix for a 'pointer(p) to a function(fn)'
    void (*pfn_preLoopFunc)(void);
    void (*pfn_inputFunc)(void);
    void (*pfn_updateFunc)(void);
    void (*pfn_renderFunc)(void);
    
    // The SDL window we'll be rendering to
    SDL_Window* m_window ;
    // OpenGL context
    SDL_GLContext m_OpenGLContext;

    // Flags to tell us the state of our program
    // Main loop flag
    // If m_quitLoop == 'true' then the Loop function terminates.
    bool m_quitLoop = false;
    bool m_SDLInitialized = false; // Flag that tells us if SDL was initialized
    bool m_OpenGLInitialized= false; // Flag that tells us if OpenGL was initialized

    // Some attributes about our window 
    float m_redChannelBackground = 0.2f;
    float m_greenChannelBackground = 0.2f;
    float m_blueChannelBackground = 0.2f;
    float m_alphaChannelBackground= 0.2f;
};

#endif