#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <string>
#include <glm/vec3.hpp>

struct Material {
	glm::vec3 ka = {0.1f,0.1f,0.1f};
	glm::vec3 kd = {1.f,1.f,1.f};
	glm::vec3 ks = {1.f,1.f,1.f};
	glm::vec3 ke = {0.f,0.f,0.f};
	float shininess = 200.f;
	float opacity = 1.f;
	std::string texture;
	std::string normalmap;
	std::string parallax;
};

#endif

