#ifndef DELAUNAYRENDERER_HPP
#define DELAUNAYRENDERER_HPP
#include "Shaders.hpp"
#include "Delaunay.hpp"

class DelaunayRenderer {
public:
	DelaunayRenderer();
	~DelaunayRenderer();
	void draw(const std::vector<glm::vec3> &vpts, const std::vector<Triangulo> &vtris, int sel);
	Shader &getShader();
private:
	Shader shader;
	GLuint VAO=0, VBO=0;
	std::vector<glm::vec3> v_curve, v_poly;
	glm::vec3 color_triangles = {0.5f, 0.5f, 0.5f};
	glm::vec3 color_points = {1.f, 1.f, 1.f};
	glm::vec3 color_selection = {1.f, 0.f, 0.f};
};

#endif

