#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Shaders.hpp"

class Renderer {
public:
	Renderer();
	~Renderer();
	
	void drawPoint(glm::vec2 pt, glm::vec4 color, int point_size=1) {
		draw(&pt,1,color,GL_POINTS,point_size);
	}
	
	template<typename TVector>
	void drawPoints(const TVector &vpts, glm::vec4 color, int point_size=1) {
		draw(vpts.data(),vpts.size(),color,GL_POINTS,point_size);
	}
	
	template<typename TVector>
	void drawSegments(const TVector &vpts, glm::vec4 color, int line_width=1) {
		draw(vpts.data(),vpts.size(),color,GL_LINES,line_width);
	}
	
	template<typename TVector>
	void drawPolygon(const TVector &vpts, glm::vec4 color, int line_width=1) {
		draw(vpts.data(),vpts.size(),color,GL_LINE_LOOP,line_width);
	}
	
	Shader &getShader();
	
private:
	Shader shader;
	GLuint VAO=0, VBO=0;
	void draw(const glm::vec2 *data, int n, glm::vec4 color, GLenum mode, int size_or_with=1);
};

#endif

