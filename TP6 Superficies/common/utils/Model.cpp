#include <tuple>
#include <cmath>
#include <algorithm>
#include "Model.hpp"
#include "Debug.hpp"
#include "ObjMesh.hpp"
#include "Misc.hpp"

Model Model::loadSingle(const std::string &name, int flags) {
	ObjMesh obj = readObj("models/"+name+".obj");
	if (!(flags&fDontFit)) centerAndResize(obj.positions);
	Geometry geometry = toGeometry(obj,0);
	if (flags&fRegenerateNormals or geometry.normals.empty()) geometry.generateNormals();
	if (flags&fNoTextures) obj.parts[0].material.texture.clear();
	return Model(std::move(geometry), obj.parts[0].material, flags&fKeepGeometry);
}

std::vector<Model> Model::load(const std::string &name, int flags) {
	auto obj = readObj("models/"+name+".obj");
	if (!(flags&fDontFit)) centerAndResize(obj.positions);
	
	std::vector<Model> vret; vret.reserve(obj.parts.size());
	for (auto &part : obj.parts) {
		Geometry geometry = toGeometry(obj,part);
		if (flags&fRegenerateNormals or geometry.normals.empty()) geometry.generateNormals();
		if (flags&fNoTextures) part.material.texture.clear();
		vret.emplace_back(std::move(geometry), part.material, flags&fKeepGeometry);
	}
	return vret;
}

void centerAndResize(std::vector<glm::vec3> &v) {
	// get global bb
	glm::vec3 pmin, pmax;
	std::tie(pmin,pmax) = getBoundingBox(v);
	
	// center on 0,0,0
	glm::vec3 center = (pmax+pmin)/2.f;
	for(glm::vec3 &p : v) 
		p -= center;
	
	// scale to fit in [-1;+1]^3
	float dmax = std::fabs(pmin.x);
	for(int j=0;j<3;++j)
		dmax = std::max(dmax, (pmax[j]-pmin[j])/2);
	for(glm::vec3 &p : v)
		p /= dmax;
}

