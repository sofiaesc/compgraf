#ifndef BEZIERRENDERER_HPP
#define BEZIERRENDERER_HPP
#include <glad/glad.h>
#include "Shaders.hpp"

class BezierRenderer {
public:
	BezierRenderer(int nsamples=100);
	~BezierRenderer();
	Shader &getShader();
	template<typename Bezier>
	void update(Bezier &b);
	void drawPoly(bool full=true);
	void drawCurve();
private:
	Shader shader;
	GLuint VAO=0, VBO[2]={0,0};
	std::vector<glm::vec3> v_curve, v_poly;
	glm::vec3 color_curve = {1.f, 1.f, 1.f};
	glm::vec3 color_poly = {.0f, .0f, .0f};
	glm::vec3 color_points = {0.f, 0.f, 0.f};
};


template<typename Bezier>
void BezierRenderer::update(Bezier &b) {
	double n = double(v_curve.size())-1;
	for(size_t i=0;i<v_curve.size();++i)
		v_curve[i] = b.at(double(i)/n);
	glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, v_curve.size() * sizeof(glm::vec3), v_curve.data());
	v_poly[0] = b[0];
	v_poly[1] = b[1];
	v_poly[2] = b[b.degree()-1];
	v_poly[3] = b[b.degree()];
	glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, v_poly.size() * sizeof(glm::vec3), v_poly.data());
}

#endif

