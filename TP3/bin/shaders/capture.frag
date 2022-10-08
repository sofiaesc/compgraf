# version 330 core

in vec3 fragPosition;
in vec2 fragTexCoords;

uniform float factor;
uniform float width;
uniform float height;
uniform float ate;

out vec4 fragColor;

uniform sampler2D capture;

void main() {
	float x = step( 1.0/factor, fract(gl_FragCoord.x/factor) );
	float y = step( 1.0/factor, fract(gl_FragCoord.y/factor) );
	fragColor = texture(capture,fragTexCoords)*max(x*y,ate);
	fragColor.a = 1.0;
}

