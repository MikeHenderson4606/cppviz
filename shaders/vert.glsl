# version 410 core

layout (location=0) in vec3 vertexPos;
layout (location=1) in vec3 vertexColor;

out vec3 fragmentColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 MVP;

void main() {
    vec4 pos = MVP * vec4(vertexPos, 1.0f);

    gl_Position = pos;

    fragmentColor = vertexColor;
}