#include "DelaunayRenderer.hpp"
#include "Delaunay.hpp"
#include "Debug.hpp"

DelaunayRenderer::DelaunayRenderer() : shader("shaders/delaunay") { 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
}

DelaunayRenderer::~DelaunayRenderer() {
	glDeleteBuffers(1,&VBO);
	glDeleteVertexArrays(1,&VAO);
}

Shader &DelaunayRenderer::getShader() {
	shader.use();
	return shader;
}

void DelaunayRenderer::draw(const std::vector<glm::vec3> &vpts, const std::vector<Triangulo> &vtris, int sel) {
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vpts.size() * sizeof(vpts[0]), vpts.data(), GL_DYNAMIC_DRAW);  
	
	GLint loc_pos = glGetAttribLocation(shader.getProgramId(), "vertexPosition"); 
	cg_assert(loc_pos!=-1,"Shader does not have vertexPositon attribute");
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	
	static std::vector<GLuint> vidxs;
	vidxs.clear(); 
	for(auto &t : vtris)
		for(int k : t.vertices)
			vidxs.push_back(k);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	shader.setUniform("color",color_triangles);
	glDrawElements(GL_TRIANGLES,vidxs.size(),GL_UNSIGNED_INT,vidxs.data());
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	
	glPointSize(3);
	shader.setUniform("color",color_points);
	glDrawArrays(GL_POINTS, 0,vpts.size());
	
	if (sel>=0 and sel<vpts.size()) {
		glPointSize(7);
		shader.setUniform("color",color_selection);
		glDrawElements(GL_POINTS,1,GL_UNSIGNED_INT,&sel);
	}
	
	glBindVertexArray(0);
}

