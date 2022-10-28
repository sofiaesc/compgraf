#ifndef SUBDIVMESHRENDERER_HPP
#define SUBDIVMESHRENDERER_HPP

#include "SubDivMesh.hpp"
#include "Shaders.hpp"

class SubDivMeshRenderer {
public:
	SubDivMeshRenderer() = default;
	SubDivMeshRenderer(const std::vector<glm::vec3> &pos, 
					   const std::vector<glm::vec3> &norms,
					   const std::vector<int> &lines,
					   const std::vector<int> &tris);
	SubDivMeshRenderer(SubDivMeshRenderer &&o);
	SubDivMeshRenderer &operator=(SubDivMeshRenderer &&o);
	void drawPoints(Shader &shader) const;
	void drawLines(Shader &shader) const;
	void drawTriangles(Shader &shader) const;
	int GetNumberOfPoints() const { return npoints; }
	~SubDivMeshRenderer();
private:
	void freeResources();
	SubDivMeshRenderer(const SubDivMeshRenderer &) = delete;
	SubDivMeshRenderer &operator=(const SubDivMeshRenderer &) = default;
	GLuint VAO=0, XBO[4]; // XBO = { VBO_pos,VBO_normals,EBO_lines,EBO_triangles }
	int npoints=0, nlines=0, ntris=0;
};

SubDivMeshRenderer makeRenderer(SubDivMesh &m);


#endif

