#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPosition;

out vec3 fragPosition;
out vec3 fragNormal;
out vec4 lightVSPosition;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition,1.f);
	fragPosition = vec3(viewMatrix * modelMatrix * vec4(vertexPosition,1.f));
	fragNormal = mat3(transpose(inverse(viewMatrix*modelMatrix))) * vertexNormal;
	lightVSPosition = viewMatrix * lightPosition;
}
