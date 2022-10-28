#include <stb_image.h>
#include "Texture.hpp"
#include "Debug.hpp"

Texture::Texture (const std::string &fname, bool repeat_s, bool repeat_t) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id); 
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load(fname.c_str(), &width, &height, &channels, 0);
	cg_assert(data,"Could not load texture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, channels==3?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	this->repeat_s = repeat_s; this->repeat_t = repeat_t;
}

Texture::~Texture ( ) {
	glDeleteTextures(1,&id);
}

void Texture::bind (int number) const {
	cg_assert(id!=0,"texture not initialized");
	glActiveTexture(GL_TEXTURE0+number);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat_s?GL_REPEAT:GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat_t?GL_REPEAT:GL_CLAMP_TO_BORDER);
}

Texture::Texture (Texture &&t) {
	*this = static_cast<const Texture &>(t);
	t = static_cast<const Texture &>(Texture{});
}

Texture & Texture::operator=(Texture &&t) {
	*this = static_cast<const Texture &>(t);
	t = static_cast<const Texture &>(Texture{});
	return *this;
}

