#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "utils.hpp"
#include "Debug.hpp"

BoundingBox::BoundingBox(glm::vec3 &p1, glm::vec3 &p2) 
	: pmin({std::min(p1.x,p2.x),std::min(p1.y,p2.y),std::min(p1.z,p2.z)}),
      pmax({std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z)}) 
{
	
}
	
bool BoundingBox::contiene(glm::vec3 &p) const {
	return p.x>=pmin.x && p.x<=pmax.x &&
		p.y>=pmin.y && p.y<=pmax.y &&
		p.z>=pmin.z && p.z<=pmax.z;
}

Pesos calcularPesos(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 &x) {
	
	glm::vec3 area_0 = cross((x1-x),(x2-x));
	glm::vec3 area_1 = cross((x2-x),(x0-x));
	glm::vec3 area_2 = cross((x0-x),(x1-x));
	
	glm::vec3 area_t = cross((x1-x0),(x2-x0));
	// glm::vec3 area_t = area_0 + area_1 + area_2;
	
	float alpha_0 = dot(area_0,area_t)/dot(area_t,area_t);
	float alpha_1 = dot(area_1,area_t)/dot(area_t,area_t);
	float alpha_2 = dot(area_2,area_t)/dot(area_t,area_t);
	
	return {alpha_0,alpha_1,alpha_2};
}
