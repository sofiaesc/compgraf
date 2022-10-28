#ifndef UTILS_H
#define UTILS_H

#include <glm/glm.hpp>
#include <array>

struct BoundingBox {
	glm::vec3 pmin, pmax;
	BoundingBox(glm::vec3 &p1, glm::vec3 &p2);
	bool contiene(glm::vec3 &p) const;
};

// interpolación afín
using Pesos = std::array<float,3>;
Pesos calcularPesos(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 &x);

#endif
