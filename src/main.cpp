/** @file main.cpp
 *  @brief Entry point into our program.
 *         Support Code written by Michael D. Shah
 *         Last Updated: 5/07/21
 *         Please do not redistribute without asking permission.
 *
 *  @author Michae Henderson
 *  @bug No known bugs.
 */

// Functionality that we created
#include "SDLGraphicsProgram.hpp"
#include "Application.hpp"
#include "FluidSimulation.hpp"
#include <iostream>

// Create an instance of an object for a SDLGraphicsProgram
// Note: The 'g' prefix indicates that this is a
//       global variable.
// Note: 'globals' generally are bad as they can clutter
//       the program. Havin '1' is sometimes okay.
SDLGraphicsProgram gSDLGraphicsProgram(1280,720,1);
Application gApplication(gSDLGraphicsProgram);

// Called once before the main loop
// Useful for any 'setup' that you need to do.
void PreLoop(void){
    std::cout << "Called before the loop one time!\n";
    gApplication.PreLoop();
}

// Handle any 'user input' here.
void Input(void){
	// Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    // SDL_StartTextInput();
	gApplication.HandleInput(e);
}

// Per frame update called in the main loop.
void Update(void){
    gApplication.Update();
}

// Per frame render called in the main loop
// Used for rendering geometry/graphics to the screen.
void Render(void){
    // Sets up the default rendering state in
    // OpenGL
    gSDLGraphicsProgram.SetDefaultOpenGLState();
    gApplication.Render();
}


// Default entry point into any C++ program
int main(int argc, char* argv[]){
    // Starting program
    std::cout << "Entry Point to Program\n";
    // Confirm our OpenGL Version Number
    gSDLGraphicsProgram.GetOpenGLVersionInfo();
    // Setup each of your function pointers before
    // the main loop.
    gSDLGraphicsProgram.SetPreLoopCallback(PreLoop);
    gSDLGraphicsProgram.SetInputCallback(Input);
    gSDLGraphicsProgram.SetUpdateCallback(Update);
    gSDLGraphicsProgram.SetRenderCallback(Render);
    // Run our program forever in an infinite loop
    // Your 'infinite loop' will calle each of the
    // callsbacks in the order of Input->Update->Render
    // (PreLoopCallback is called once before the loop)


    /*
    
    This is where all the dynamic operations happen
    --------------------------------------------------------------------------------------- */


    IObject* circle = new Circle(
        glm::vec3(1.0f, 1.0f, 0.0f),
        0.1f
    );
    IObject* circle2 = new Circle(
        glm::vec3(-0.5f, 0.5f, 0.0f),
        0.1f
    );
    IObject* circle3 = new Circle(
        glm::vec3(-1.0f, 1.0f, 0.0f),
        0.1f
    );
    IObject* circle4 = new Circle(
        glm::vec3(-0.5f, -0.5f, 0.0f),
        0.1f
    );
    IObject* circle5 = new Circle(
        glm::vec3(-1.0f, -1.0f, 0.0f),
        0.1f,
        glm::vec3(0.2, 0.0, 0.6)
    );
    IObject* line = new Line(
        glm::vec3(-3.0f, 0.0f, 0.0f),
        glm::vec3(3.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        0.05f
    );
    IObject* line2 = new Line(
        glm::vec3(0.0f, 3.0f, 0.0f),
        glm::vec3(0.0f, -3.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        0.05f
    );
    // gApplication.AddObject(line);
    // gApplication.AddObject(line2);
    // gApplication.AddObject(circle);
    // gApplication.AddObject(circle2);
    // gApplication.AddObject(circle3);
    // gApplication.AddObject(circle4);
    // gApplication.AddObject(circle5);
    Simulation* fluidSim = new FluidSimulation(2.4, 2.4, 0.5);
    fluidSim->AddApplicationToSimulation(&gApplication);
    gApplication.AddSimulation(fluidSim);


    /* ---------------------------------------------------------------------------------------
    Keep them between these lines

    */

    gSDLGraphicsProgram.Loop();
    // When our program ends, it will exit scope, the
    // destructor will then be called and clean up the program.

    // delete circle;
    // delete circle2;
    // delete line;
    // delete line2;
    return 0;
}