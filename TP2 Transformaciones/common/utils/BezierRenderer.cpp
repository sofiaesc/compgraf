#include "BezierRenderer.hpp"
#include "Debug.hpp"

BezierRenderer::BezierRenderer(int nsamples) : shader("shaders/curve") { 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	v_curve.resize(nsamples);
	v_poly.resize(4);
	glGenBuffers(2, VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, v_curve.size() * sizeof(glm::vec3), v_curve.data(), GL_DYNAMIC_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(glm::vec3), v_poly.data(), GL_DYNAMIC_DRAW);  
}

BezierRenderer::~BezierRenderer() {
	glDeleteBuffers(2,VBO);
	glDeleteVertexArrays(1,&VAO);
}

Shader &BezierRenderer::getShader() {
	shader.use();
	return shader;
}

void BezierRenderer::drawPoly() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO[1]);
	GLint loc_pos = glGetAttribLocation(shader.getProgramId(), "vertexPosition"); 
	cg_assert(loc_pos!=-1,"Shader does not have vertexPositon attribute");
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	shader.setUniform("color",color_poly);
	glDrawArrays(GL_LINES, 0,v_poly.size());
	shader.setUniform("color",color_points);
	glDrawArrays(GL_POINTS, 0,v_poly.size());
	glBindVertexArray(0);
}

void BezierRenderer::drawCurve() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO[0]);
	GLint loc_pos = glGetAttribLocation(shader.getProgramId(), "vertexPosition"); 
	cg_assert(loc_pos!=-1,"Shader does not have vertexPositon attribute");
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	shader.setUniform("color",color_curve);
	glDrawArrays(GL_POINTS, 0,v_curve.size());
	glBindVertexArray(0);
}
