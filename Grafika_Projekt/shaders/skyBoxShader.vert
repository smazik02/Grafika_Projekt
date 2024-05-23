#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 VP;
uniform mat4 View;
uniform mat4 Projection;

void main() {
    TexCoords = aPos;
    vec4 pos = Projection * View * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}