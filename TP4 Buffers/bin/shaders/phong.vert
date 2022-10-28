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
	mat4 vm = viewMatrix * modelMatrix;
	vec4 vmp = vm * vec4(vertexPosition,1.f);
	fragPosition = vec3(vmp);
	gl_Position = projectionMatrix * vmp;
	fragNormal = mat3(transpose(inverse(vm))) * vertexNormal;
	lightVSPosition = vm * lightPosition;
}
