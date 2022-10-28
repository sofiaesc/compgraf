#version 330 core

in vec2 vertexPosition;
in vec2 vertexTexCoords;

out vec2 fragTexCoords;

uniform mat4 matrix;

void main() {
	gl_Position = matrix * vec4(vertexPosition,0.f,1.f);
	fragTexCoords = vertexTexCoords;
}
