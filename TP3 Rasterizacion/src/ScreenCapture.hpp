#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H
#include <glm/glm.hpp>
#include "Shaders.hpp"

class ScreenCapture {
public:
	ScreenCapture();
	void take(int x, int y, int w, int h, int factor);
	void draw();
	Shader &getShader();
	~ScreenCapture();
private:
	Shader shader;
	GLuint VAO, VBO[2], tex_id=0;
	GLint loc_pos, loc_tc;
	glm::vec3 verts[4];
	glm::vec2 tex_coords[4];
};

#endif

