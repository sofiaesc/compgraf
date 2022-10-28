#ifndef BEZIER_HPP
#define BEZIER_HPP
#include <initializer_list>
#include <iterator>
#include <glm/glm.hpp>
#include "Debug.hpp"

template<typename VEC, int D> 
struct DecastImpl {
	static VEC eval(const VEC p[], float t) {
		return DecastImpl<VEC,D-1>::eval(p,t)*(1-t) + DecastImpl<VEC,D-1>::eval(p+1,t)*t;
	}
};

template<typename VEC>
struct DecastImpl<VEC,0> {
	static VEC eval(const VEC p[], float t) { return *p; }
};

template<typename VEC, int D>
VEC Decast(const VEC p[], float t) {
	return DecastImpl<VEC,D>::eval(p,t);
}

template<typename VEC = glm::vec3, int DEGREE=3>
class Bezier {
	VEC p[DEGREE+1];
public:
	Bezier() = default;
	Bezier(const std::initializer_list<VEC> &l) {
		cg_assert(std::distance(l.begin(),l.end())==DEGREE+1,"wrong number of control points");
		auto it = l.begin();
		for(int j=0;j<=DEGREE;++j,++it) p[j] = *it;
	}
	VEC &operator[](int i) { return p[i]; }
	const VEC &operator[](int i) const { return p[i]; }
	VEC at(float t) const {
		return Decast<VEC,DEGREE>(p,t);
	}
	VEC at(float t, VEC &deriv) const {
		VEC x[2] = { Decast<VEC,DEGREE-1>(p,t), Decast<VEC,DEGREE-1>(p+1,t) };
		deriv = (x[1]-x[0])*float(DEGREE);
		return Decast<VEC,1>(x,t);
	}
	int degree() const { return DEGREE; }
	VEC *data() { return p; }
	const VEC *data() const { return p; }
	const int size() const { return DEGREE+1; }
};

#endif
