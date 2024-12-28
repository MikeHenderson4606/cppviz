#include "SDLGraphicsProgram.hpp"


// Initialization function
SDLGraphicsProgram::SDLGraphicsProgram(int windowWidth, int windowHeight, int contextType):m_windowWidth(windowWidth),m_windowHeight(windowHeight){
    // Create a regular SDL window
    if(0==contextType){
        SDLCreateWindow();
    }else if(1==contextType){
        SDLCreateOpenGLWindow();
    }else{
        SDL_Log("unsupported contextType for now...");
        exit(1);
    }
}

// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point gWindow to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}

void SDLGraphicsProgram::SDLCreateWindow(){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = nullptr;

	// Initialize SDL2
    // Specifically, we are initializing the video system, although there
    // are other systems we may want to initialize (e.g. audio).
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Create a regular SDL window
		m_window = SDL_CreateWindow( "Mathematical Visualizer",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_windowWidth,
                                m_windowHeight,
                                SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == nullptr ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL errors detected during initialization\n\n");
        m_SDLInitialized = true;
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
}

void SDLGraphicsProgram::SDLCreateOpenGLWindow(){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = nullptr;

	// Initialize SDL2
    // Specifically, we are initializing the video system, although there
    // are other systems we may want to initialize (e.g. audio).
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{

        // Setup attributes for our OpenGL Window
        // Note: These need to be setup before we create an SDL window that
        // uses opengl.
		// Use OpenGL 4.1 core (Note: Mac maximum is 4.1 so that is our upper bound)
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        // Enable multisampling (4x)
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // 1 buffer
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);  // 4x samples

		//Create an SDL window that is compatible with OpenGL
		m_window = SDL_CreateWindow( "Mathematical Visualizer",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_windowWidth,
                                m_windowHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == nullptr ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL errors detected during initialization\n\n");
        m_SDLInitialized = true;
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
    // Call a helper function to InitGL()
    InitGL();
}

// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	//Create an OpenGL Graphics Context
	if(m_window==nullptr){
		errorStream << "InitGL: Error, SDL Window was not previously initialized before initializing OpenGL";
	}

    // Create our OpenGL context and set it to our window
	m_OpenGLContext = SDL_GL_CreateContext( m_window );
	if( m_OpenGLContext == nullptr){
		errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}

	// Initialize GLAD Library
    // This is essentially setting up a bunch of function pointers
    // to all of the OpenGL functions.
	if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
		errorStream << "Failed to iniitalize GLAD\n";
		success = false;
	}

    // If initialization did not work, then print out a list of errors in the constructor.
    // NOTE: Windows users sometimes cannot see the output message for some reason.
    //       Try instead: std::cout << errors.c_str() << std::endl; if there are errors.
    //       or otherwise: std::cout << "SDLGraphicsProgram::InitGL - GLAD and OpenGL has been initialized\n\n";
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
		exit(1);
    }else{
        SDL_Log("SDLGraphicsProgram::InitGL - GLAD and OpenGL has been initialized\n\n");
        m_OpenGLInitialized = true;
    }

	return success;
}


// Helper function to setup some default options for rendering
// in OpenGL.
void SDLGraphicsProgram::SetDefaultOpenGLState(){
    if(m_OpenGLInitialized){
        // Sets up the screen coordinates for our window in 
        // from the normalized device coordinates to window
        // coordinates. In other words, in OpenGL we are going to
        // have a coordinate system that ranges from say -1.0 to 1.0 on
        // the x and the y axis. The glViewport command translates those
        // coordinates to 'window coordinates' which we are more use to 
        // working with (e.g. a 640x480, 800x600, 1280x800, etc. pixels).
        // Most of the time, this will simply be the size of our window.
        // So to recap, in 3D graphics land what was once '-1,1' in the
        // x-axis, will now map to 0 to 1280, and the same for the 
        // y-axis.
        // More info: http://docs.gl/gl4/glViewport
        glViewport(0, 0, m_windowWidth, m_windowHeight);
        // Setup our OpenGL State machine
        // What we are doing, is telling OpenGL to create a depth(or Z-buffer) 
        // for us that is stored every frame.
        // A Z-buffer helps us in determining which pixel is closest to our
        // camera, and thus should be rendered.
        glEnable(GL_DEPTH_TEST);
        // What we are doing, is telling OpenGL to enable textures.
        glEnable(GL_TEXTURE_2D); 
        // Enable multisampling
        glEnable(GL_MULTISAMPLE);
        // Set the clear color
        // This is the color the back of the screen will be cleared to.
        // The range is between 0.0f and 1.0f for the 
        // red, green, blue, and alpha channels.
        glClearColor( m_redChannelBackground,
                      m_greenChannelBackground,
                      m_blueChannelBackground,
                      m_alphaChannelBackground);
        // Clear color buffer and Depth Buffer
        // Remember that the 'depth buffer' is our
        // z-buffer that figures out how far away items are every frame
        // and we have to do this every frame!
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
}


// Terminates the main program loop
void SDLGraphicsProgram::TerminateLoop(){
    m_quitLoop = true;
}


// Sets the function pointer for the
// function that will be called exactly once
// before the main loop
void SDLGraphicsProgram::SetPreLoopCallback(void (*preLoopFunc)(void)){
    pfn_preLoopFunc = preLoopFunc;
}

// Calls the input function that will be called
// each time in the main loop.
void SDLGraphicsProgram::SetInputCallback(void   (*inputFunc)(void)){
    pfn_inputFunc = inputFunc;
}

// Sets the update function that will be
// each time in the main loop.
void SDLGraphicsProgram::SetUpdateCallback(void (*updateFunc)(void)){
    pfn_updateFunc = updateFunc;
}

// Call the render function that will be
// each time in the main loop.
void SDLGraphicsProgram::SetRenderCallback(void (*renderFunc)(void)){
    pfn_renderFunc = renderFunc;
}

//Loops forever!
void SDLGraphicsProgram::Loop(){
	// Calling this function starts the main loop.
	m_quitLoop = false;
    
    // Call the pre-loop function once
    pfn_preLoopFunc();

    // While application is running
    while(!m_quitLoop){
     	
		// Handle any input from the user
        // (Calls the function pointer)
        pfn_inputFunc();
		// Update our scene (i.e. any geometry or 3D updates)
        // (Calls the function pointer)
        pfn_updateFunc();
		// Render using OpenGL the actual geometry
        // (Calls the function pointer)
        pfn_renderFunc();
      	// Update screen of our specified window with the final
        // graphics scene.
        if(m_OpenGLInitialized){
      	    SDL_GL_SwapWindow(GetSDLWindowPointer());
        }else{
            
        }
    }
}

void SDLGraphicsProgram::SetClearColor(float r, float g, float b, float a){
    m_redChannelBackground      = r;
    m_greenChannelBackground    = g;
    m_blueChannelBackground     = b;
    m_alphaChannelBackground    = a;
}


// Helper function to clear the contents being rendered
// into the window
void SDLGraphicsProgram::ClearWindow(){

}

// Retrieve the pointer to the SDL Window
SDL_Window* SDLGraphicsProgram::GetSDLWindowPointer(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
    if(m_OpenGLInitialized){
        SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
        SDL_Log("(Note: Let instructor know if you do not have at least OpenGL 4.3)");
        SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
        SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
        SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
        SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
}