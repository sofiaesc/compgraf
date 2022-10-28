#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "ObjMesh.hpp"
#include "Material.hpp"
#include "Geometry.hpp"

struct ObjMesh {
	
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> tex_coords;
	
	struct Element {
		int pos[4];
		int tcs[4];
		int norms[4];
	};
	struct Part {
		std::string name;
		Material material;
		std::vector<Element> elements;
	};
	std::vector<Part> parts;
	
	const Part &getPart(const std::string &name) const;
	
};

ObjMesh readObj(const std::string &full_path);

Geometry toGeometry(const ObjMesh &obj, const ObjMesh::Part &part);
Geometry toGeometry(const ObjMesh &obj, int ipart=0);
Geometry toGeometry(const ObjMesh &obj, const std::string &name);

#endif
