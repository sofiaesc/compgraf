#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPosition;
uniform float t;

out vec3 fragPosition;
out vec3 fragNormal;
out vec4 lightVSPosition;

void main() {
	vec4 pos = vec4(vertexPosition,1.f);
	pos.z += pow(cos( (pos.x+1.f)/2.f ),100)*.4 * sin(fract(t)*2*3.1415926538);
	pos.z += pow(cos( (1.f-pos.x)/2.f ),25)*.1 * sin(fract(t)*2*3.1415926538);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
	fragPosition = vec3(modelMatrix * pos);
	fragNormal = mat3(transpose(inverse(viewMatrix*modelMatrix))) * vertexNormal;
	lightVSPosition = viewMatrix * lightPosition;
}
