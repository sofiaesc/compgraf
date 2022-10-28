#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <glad/glad.h>

class Texture {
public:
	Texture() = default;
	Texture(const std::string &fname, bool repeat_s=true, bool repeat_t=true);
	Texture(Texture &&t);
	Texture &operator=(Texture &&t);
	~Texture();
	void bind(int number=0) const;
	bool isOk() const { return channels!=-1; }
private:
	Texture &operator=(const Texture &t) = default;
	GLuint id = 0;
	int width=-1, height=-1, channels=-1;
	bool repeat_s=true, repeat_t=true;
};

#endif

