# version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoords;
in vec4 lightVSPosition;

// propiedades del material
uniform sampler2D colorTexture; // ambient and diffuse components
uniform vec3 specularColor;
uniform float shininess;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;

out vec4 fragColor;

#include "funcs/calcPhong.frag"

void main() {
	
	vec4 tex = texture(colorTexture,fragTexCoords);
	vec3 phong = calcPhong(lightVSPosition, lightColor,
						   vec3(tex), vec3(tex), specularColor, shininess);
	fragColor = vec4(phong,tex.a);
}

