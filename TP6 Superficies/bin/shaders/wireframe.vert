#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out float colorDecay;

void main() {
	vec3 fragNormal = mat3(transpose(inverse(viewMatrix*modelMatrix))) * vertexNormal;
	colorDecay = fragNormal.z<0.f ? .5f : 1.f;
	vec4 offset = vec4(fragNormal*vec3(0.f,0.f,-0.0001f),0.f);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition,1.f) + offset;
}
