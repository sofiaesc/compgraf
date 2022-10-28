# version 330 core

uniform sampler2D colorTexture;
in vec2 fragTexCoords;
out vec4 fragColor;

void main() {
	fragColor = texture(colorTexture,fragTexCoords);
}

