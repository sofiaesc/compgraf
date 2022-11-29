#include <tuple>
#include <cmath>
#include <algorithm>
#include "Model.hpp"
#include "Debug.hpp"

#include "Misc.hpp"



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

