#ifndef STENCIL_H
#define STENCIL_H

#include "Shaders.hpp"

class GLFWwindow;

class ShowStencil {
public:
	ShowStencil();
	void draw(int max);
	~ShowStencil();
private:
	GLuint VAO=0, VBO=0;
	Shader shader;
};

int getStencilValueUnderMouseCursor(GLFWwindow *window);

#endif

