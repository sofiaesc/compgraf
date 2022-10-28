# version 330 core

// FRAGMENT SHADER

in vec3 fragNormal;
in vec3 fragPosition;
in vec4 lightVSPosition;
in vec2 fragTexCoords;  // le llega como dato del rasterizador

// propiedades del material
uniform sampler2D colorTexture;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 diffuseColor;
uniform vec3 emissionColor;
uniform float opacity;
uniform float shininess;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;

out vec4 fragColor;

#include "funcs/calcPhong.frag"

void main() {
	// ETAPA DEL PIPELINE: Procesamiento de fragmentos
	vec4 tex = texture(colorTexture,fragTexCoords);
	
	vec3 phong = calcPhong(lightVSPosition, lightColor,
						   mix(ambientColor,vec3(tex),tex.a),
						   mix(diffuseColor,vec3(tex),tex.a),
						   specularColor, shininess);
	// specular lo deja igual, hace un mix entre las componentes de phong y la textura para la ambiente y la difusa.
	// mix: (phong)*(1-tex.a)+vec3(tex)*tex.a
	// interpolación afin entre phong y la textura, con un alfa dado.
	
	fragColor = vec4(phong+emissionColor,opacity);
	// color del fragmento: phong calculada con el color que emite el propio cuerpo.
}

