#include "Renderer.hpp"
#include "Debug.hpp"

Renderer::Renderer() : shader("shaders/raster") { 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1, &VBO);
}

Renderer::~Renderer() {
	glDeleteBuffers(1,&VBO);
	glDeleteVertexArrays(1,&VAO);
}

Shader &Renderer::getShader() {
	shader.use();
	return shader;
}

void Renderer::draw(const glm::vec2 *data, int n, glm::vec4 color, GLenum mode, int size_or_width) {
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n * sizeof(glm::vec2), data, GL_DYNAMIC_DRAW);  
	
	GLint loc_pos = glGetAttribLocation(shader.getProgramId(), "vertexPosition"); 
	cg_assert(loc_pos!=-1,"Shader does not have vertexPosition attribute");
	glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	
	shader.setUniform("color",color);
	if (mode==GL_POINTS) glPointSize(size_or_width);
	else                 glLineWidth(size_or_width);
	glDrawArrays(mode, 0,n);
	
	glBindVertexArray(0);
}

