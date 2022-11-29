#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <vector>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Geometry {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<int> triangles;
	void generateNormals();
	
};

class GeometryRenderer {
public:
	GeometryRenderer() = default;
	GeometryRenderer(const Geometry &geo, bool dynamic=false);
	GeometryRenderer(GeometryRenderer &&geo);
	GeometryRenderer &operator=(GeometryRenderer &&geo);
	void draw() const;
	GLuint vertexArray() const { return VAO; }
	GLuint positionsVBO() const { return VBO_pos; }
	GLuint normalsVBO() const { return VBO_norms; }
	GLuint texCoordsVBO() const { return VBO_tcs; }
	
	void updateTexCoords(const std::vector<glm::vec2> &vtc, bool realloc=false, bool dynamic=false);
	void updatePositions(const std::vector<glm::vec3> &vp, bool realloc=false, bool dynamic=false);
	void updateNormals(const std::vector<glm::vec3> &vn, bool realloc=false, bool dynamic=false);
	void updateElements(const std::vector<int> &ve, bool realloc=false, bool dynamic=false);
	
	~GeometryRenderer();
private:
	GeometryRenderer(const GeometryRenderer &) = delete;
	GeometryRenderer &operator=(const GeometryRenderer &) = default;
	void freeResources();
	GLuint VAO=0, VBO_pos=0, VBO_tcs=0, VBO_norms=0, EBO=0;
	int count = 0;
};

#endif

