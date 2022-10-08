#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Callbacks.hpp"
#include "Stencil.hpp"

static glm::vec4 hsv2rgb(float h, float s, float v, float a) {
	
	if(h >= 360.f) h = 0.f; else h /= 60.f;
	
	int i = (int)h;
	float f = h - i;
	float p = v * (1.f - s);
	float q = v * (1.f - (s * f));
	float t = v * (1.f - (s * (1.f - f)));
	
	switch(i) {
	case 0:  return {v,t,p,a};
	case 1:  return {q,v,p,a};
	case 2:  return {p,v,t,a};
	case 3:  return {p,q,v,a};
	case 4:  return {t,p,v,a};
	default: return {v,p,q,a};
	}
}

static glm::vec4 getColor(int i) {
	constexpr float golden_ration = 1.61803398875;
	float hue = std::fmod(i*360.f/golden_ration,360.f);
	return hsv2rgb(hue,.7f,1.f,0.75f);
}

ShowStencil::ShowStencil() : shader("shaders/stencil") {
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	std::vector<glm::vec3> vpos = {
		{-1.f,-1.f,0.f},
		{+1.f,-1.f,0.f},
		{+1.f,+1.f,0.f},
		{-1.f,+1.f,0.f} };
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vpos.size()*sizeof(glm::vec3), vpos.data(), GL_STATIC_DRAW);
	int loc_pos = 0;
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	glBindVertexArray(0);
}
void ShowStencil::draw(int max) {
	glBindVertexArray(VAO);
	shader.use();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
	for(int ref=0;ref<max;++ref) {
		shader.setUniform("color",getColor(ref));
		glStencilFunc(GL_EQUAL,ref,255);
		glDrawArrays(GL_TRIANGLE_FAN,0,4);
	}
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
	glBindVertexArray(0);
}

ShowStencil::~ShowStencil() {
	glDeleteBuffers(1,&VBO);
	glDeleteVertexArrays(1,&VAO);
}

int getStencilValueUnderMouseCursor(GLFWwindow *window) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	unsigned char s;
	glReadPixels(static_cast<int>(x),win_height-static_cast<int>(y),1,1,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,&s);
	return s;
}
