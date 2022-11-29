#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPosition;
uniform vec3 aTangent;
uniform vec3 aBitangent;
uniform vec3 bTangent;
uniform vec3 bBitangent;



out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec4 lightVSPosition;
out mat3 TBN;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

void main() {
	
	
	mat4 vm = viewMatrix * modelMatrix;
	vec4 vmp = vm * vec4(vertexPosition,1.f);
	gl_Position = projectionMatrix * vmp;
	fragPosition = vec3(vmp);
	fragNormal = mat3(transpose(inverse(vm))) * vertexNormal;
	lightVSPosition = viewMatrix * lightPosition;
	fragTexCoords = vertexTexCoords;
	
	vec3 T = normalize(vec3(modelMatrix * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(modelMatrix * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(modelMatrix * vec4(fragNormal, 0.0)));
	TBN = transpose(mat3(T,B,N));
	
	TangentLightPos = TBN * vec3(lightPosition);
	TangentViewPos  = TBN * normalize(-fragPosition);
	TangentFragPos  = TBN * fragPosition;
}
