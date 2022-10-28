# version 330 core

// propiedades del material
//uniform vec3 diffuseColor;
uniform float opacity;

in float colorDecay;

out vec4 fragColor;

void main() {
	
	fragColor = vec4(vec3(0.f,0.f,0.f),colorDecay);
}
