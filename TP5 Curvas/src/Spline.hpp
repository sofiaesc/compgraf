#ifndef SPLINE_HPP
#define SPLINE_HPP

#include <initializer_list>
#include <vector>
#include <glm/glm.hpp>
#include "Bezier.hpp"

class Spline {
public:
	Spline(const std::vector<glm::vec3> &vp);
	void setControlPoint(int i, glm::vec3 p);
	glm::vec3 getControlPoint(int i) const;
	glm::vec3 at(double t) const;
	glm::vec3 at(double t, glm::vec3 &deriv) const;
	int getControlPointsCount() const;
	const std::vector<Bezier<>> &getPieces() const;
private:
	std::vector<Bezier<glm::vec3,3>> pieces;
	std::pair<int,double> mapT(double t) const;
	std::pair<int,int> getIndexes(int i) const;
};

#endif

