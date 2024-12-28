#include "Application.hpp"
#include "SDLGraphicsProgram.hpp"
#include "IObject.hpp"
#include "Triangle.hpp"
#include "Line.hpp"
#include "Circle.hpp"
#include "Simulation.hpp"
#include "FluidSimulation.hpp"

// Standard cpp libaries
#include <iostream>

// Constructor
Application::Application(SDLGraphicsProgram& prog)
    : program(prog) { 
    camera = new Camera();
}

// Destructor
Application::~Application() {

}

// Create default vbo
void CreateDefaultVBO(GLuint& vao, GLuint& vbo, std::vector<GLfloat> vertices) {
    // Create vertex array object
    glGenVertexArrays(1, &vao);
    // Bind the vertex array object
    glBindVertexArray(vao);

    // Create a vertex buffer object
    glGenBuffers(1, &vbo);
    // Bind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Give the vbo data
    glBufferData(GL_ARRAY_BUFFER,
                    vertices.size() * sizeof(GL_FLOAT),
                    vertices.data(),
                    GL_STATIC_DRAW);

    // Enable position mapping
    glEnableVertexAttribArray(0);
    // Tell GPU where the vertices are
    glVertexAttribPointer(0,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            sizeof(float) * 6,
                            (void*)0);
                        
    // Enable color mapping
    glEnableVertexAttribArray(1);
    // Tell GPU where the vertices are
    glVertexAttribPointer(1,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            sizeof(float) * 6,
                            (void*)(sizeof(float) * 3));
}

// Bind buffers with IBO
void CreateDefaultIBO(GLuint& vao, GLuint& vbo, std::vector<GLfloat> vertices, std::vector<GLint> indices) {
    // Create vertex array object
    glGenVertexArrays(1, &vao);
    // Bind the vertex array object
    glBindVertexArray(vao);

    // Create a vertex buffer object
    glGenBuffers(1, &vbo);
    // Bind the vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Give the vbo data
    glBufferData(GL_ARRAY_BUFFER,
                    vertices.size() * sizeof(GL_FLOAT),
                    vertices.data(),
                    GL_STATIC_DRAW);

    // Use EBO
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Enable position mapping
    glEnableVertexAttribArray(0);
    // Tell GPU where the vertices are
    glVertexAttribPointer(0,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            sizeof(GL_FLOAT) * 6,
                            (void*)0);
                        
    // Enable color mapping
    glEnableVertexAttribArray(1);
    // Tell GPU where the vertices are
    glVertexAttribPointer(1,
                            3,
                            GL_FLOAT,
                            GL_FALSE,
                            sizeof(GL_FLOAT) * 6,
                            (void*)(sizeof(GL_FLOAT) * 3));
}

// Converts a shader to a string to be interpreted
std::string ConvertShaderToString(const std::string& filepath) {
    // Resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filepath.c_str());

    if(myFile.is_open()){
        while(std::getline(myFile, line)){
            result += line + '\n';
        }
        myFile.close();

    }
    
    return result;
}

// Compiles vertex and fragment shader
GLuint CompileShader(GLuint type, const std::string& source){
	// Compile our shaders
	GLuint shaderObject;

	// Based on the type passed in, we create a shader object specifically for that
	// type.
	if(type == GL_VERTEX_SHADER){
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}else if(type == GL_FRAGMENT_SHADER){
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	// The source of our shader
	glShaderSource(shaderObject, 1, &src, nullptr);
	// Now compile our shader
	glCompileShader(shaderObject);

	// Retrieve the result of our compilation
	int result;
	// Our goal with glGetShaderiv is to retrieve the compilation status
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

	if(result == GL_FALSE){
		int length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		char* errorMessages = new char[length]; // Could also use alloca here.
		glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

		if(type == GL_VERTEX_SHADER){
			std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
		}else if(type == GL_FRAGMENT_SHADER){
			std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
		}
		// Reclaim our memory
		delete[] errorMessages;

		// Delete our broken shader
		glDeleteShader(shaderObject);

		return 0;
	}

  return shaderObject;
}

// Creates a shader program from the given vertex and fragment shaders
GLuint CreateShaderProgram(const std::string& vertSource, const std::string& fragSource) {
    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile our shaders
    GLuint myVertexShader   = CompileShader(GL_VERTEX_SHADER, vertSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragSource);

    // Link our two shader programs together.
    // Consider this the equivalent of taking two .cpp files, and linking them into
    // one executable file.
    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    // Validate our program
    glValidateProgram(programObject);

    // Once our final program Object has been created, we can
    // detach and then delete our individual shaders.
    glDetachShader(programObject, myVertexShader);
    glDetachShader(programObject, myFragmentShader);
    // Delete the individual shaders once we are done
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

// Creates a compute shader for more efficient runtime computation
GLuint CreateComputeShader(const std::string& source) {
    // Create shader
    GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* sourceStr = source.c_str();

    // Compile shader
    glShaderSource(compute_shader, 1, &sourceStr, NULL);
    glCompileShader(compute_shader);

    // Check for failures stored in a success variable
    GLint success;
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(compute_shader, 512, nullptr, infoLog);
        std::cerr << "Compute Shader Compilation Failed:\n" << infoLog << std::endl;
        return 0;
    }

    // Create a new program to house the compute shader
    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, compute_shader);
    glLinkProgram(computeProgram);

    // Check for linking errors -- verify linking went according to plan
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        return 0;
    }

    // Detach and delete shader
    glDetachShader(computeProgram, compute_shader);
    glDeleteShader(compute_shader);

    return computeProgram;
}

// Create the default shader with position and color data
GLuint CreateDefaultShader() {
    std::string vert_src = ConvertShaderToString("./shaders/vert.glsl");
    std::string frag_src = ConvertShaderToString("./shaders/frag.glsl");

    GLuint shader = CreateShaderProgram(vert_src, frag_src);
    return shader;
}

// Returns a reference of the compute shader
GLuint& Application::getComputeShader() {
    return computeShader;
}

// Pre loop
void Application::PreLoop() {
    defaultShader = CreateDefaultShader();
    computeShader = CreateComputeShader(ConvertShaderToString("./shaders/force_compute.glsl"));

    // Debug shader creation
    GLint success;
    glGetProgramiv(defaultShader, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(defaultShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(defaultShader, logLength, &logLength, log.data());
        std::cerr << "Shader program linking failed: " << log.data() << std::endl;
        return;
    } else {
        std::cout << "Shader successfully created!" << std::endl;
    }

    // Debug shader creation
    glGetProgramiv(computeShader, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(computeShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<GLchar> log(logLength);
        glGetProgramInfoLog(computeShader, logLength, &logLength, log.data());
        std::cerr << "Shader program linking failed: " << log.data() << std::endl;
        return;
    } else {
        std::cout << "Shader successfully created!" << std::endl;
    }

    // Render all simulations' initial states
    for (Simulation* sim : simulations) {
        sim->Render();
    }
}

void Application::AddObject(std::shared_ptr<IObject> object) {
    if (std::shared_ptr<Triangle> triangle = std::dynamic_pointer_cast<Triangle>(object)) {
        // Add the triangle to the list
        triangles.push_back(triangle);
    }
    else if (std::shared_ptr<Line> line = std::dynamic_pointer_cast<Line>(object)) {
        // Add the line to the list
        lines.push_back(line);
    }
    else if (std::shared_ptr<Circle> circle = std::dynamic_pointer_cast<Circle>(object)) {
        // Add the circle to the list
        circles.push_back(circle);
    }
    else {
        std::cerr << "Error, object is not a valid IObject." << std::endl;
    }
}

void Application::AddSimulation(Simulation* sim) {
    simulations.push_back(sim);
}


void Application::HandleInput(SDL_Event e) {

    // Handle events on queue
	while(SDL_PollEvent( &e ) != 0){
		// User posts an event to quit
		// An example is hitting the "x" in the corner of the window.
		// Set our 'flag' to true so that we quit executing the Loop() function..
		if(e.type == SDL_QUIT){
            program.TerminateLoop();
		}

        const Uint8* state = SDL_GetKeyboardState(NULL);

        if(e.type == SDL_KEYDOWN) {
            if(state[SDL_SCANCODE_ESCAPE]) {
                program.TerminateLoop();
            }
        }

	} // End SDL_PollEvent loop.
}

void Application::Update() {
    for (Simulation* sim : simulations) {
        sim->Update();
    }
}

// Adjusts the given indices by the given factor
std::vector<GLuint> AdjustIndices(std::vector<GLuint> indices, int factor) {
    std::vector<GLuint> newIndices;
    for (GLuint index : indices) {
        newIndices.push_back(index + factor);
    }

    return newIndices;
}

void Application::Render() {
    // Draw all triangles at once
    // Compile all triangles' vertices
    for (int i = 0; i < triangles.size(); i++) {
        // Update vertices
        triangleVertices.insert(triangleVertices.end(), triangles.at(i)->vertices.begin(), triangles.at(i)->vertices.end());
        // Add the appropriate indices to the list
        std::vector<GLuint> t_indices = triangles.at(i)->ibo;
        t_indices = AdjustIndices(t_indices, i * (triangleVertices.size() / (6 * triangles.size())));
        // Update indices
        triangleIndices.insert(triangleIndices.end(), t_indices.begin(), t_indices.end());

    } // We do this so that when we update the triangle vertices dynamically it will update in real time
    CreateDefaultIBO(vao, vbo, triangleVertices, triangleIndices);
    Draw(triangleIndices);

    // Draw all lines at once
    // Compile all lines' vertices
    for (int i = 0; i < lines.size(); i++) {
        // Update vertices
        lineVertices.insert(lineVertices.end(), lines.at(i)->vertices.begin(), lines.at(i)->vertices.end());
        // Add the appropriate indices to the list
        std::vector<GLuint> l_indices = lines.at(i)->ibo;
        l_indices = AdjustIndices(l_indices, round(i * 4));
        // Update indices
        lineIndices.insert(lineIndices.end(), l_indices.begin(), l_indices.end());

    } // We do this so that when we update the triangle vertices dynamically it will update in real time
    CreateDefaultIBO(vao, vbo, lineVertices, lineIndices);
    Draw(lineIndices);

    // Draw all circles at once
    // Compile all lines' vertices
    int count = 0;
    for (const auto& circle: circles) {
        // Update vertices
        circleVertices.insert(circleVertices.end(), circle->vertices.begin(), circle->vertices.end());
        // Add the appropriate indices to the list
        std::vector<GLuint> c_indices = circle->ibo;
        c_indices = AdjustIndices(c_indices, count * 33);
        // Update indices
        circleIndices.insert(circleIndices.end(), c_indices.begin(), c_indices.end());
        count++;
    } // We do this so that when we update the triangle vertices dynamically it will update in real time
    CreateDefaultIBO(vao, vbo, circleVertices, circleIndices);
    Draw(circleIndices);

    // Clear the indices and vertices
    lineVertices.clear();
    lineIndices.clear();
    triangleVertices.clear();
    triangleIndices.clear();
    circleVertices.clear();
    circleIndices.clear();
}

void SendMVP(SDLGraphicsProgram &prog, GLuint &shader, Camera* camera) {
    // Create the perspective matrix
    glm::mat4 perspective = glm::perspective(
        glm::radians(45.0f),
        (float)prog.m_windowWidth/(float)prog.m_windowHeight,
        0.1f,
        20.0f
    );
    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Create view matrix
    glm::mat4 view = camera->GetViewMatrix();
    // Create the MVP matrix
    glm::mat4 MVP = perspective * view * model;
    // Find MVP's location in the shader
    GLint mvpLocation = glGetUniformLocation(shader, "MVP");
    if(mvpLocation >= 0){
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &MVP[0][0]);
    }else{
        std::cout << "Could not find MVP, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
}

void Application::Draw(std::vector<GLint> &indices) {
    glUseProgram(defaultShader);

    SendMVP(program, defaultShader, camera);

    glDrawElements(
        GL_TRIANGLES,
        indices.size(),
        GL_UNSIGNED_INT,
        0
    );

    // Unbind the vao
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // Stop using graphics pipeline
    glUseProgram(0);
}

void Destroy() {
    
}