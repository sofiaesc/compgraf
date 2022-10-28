#ifndef BEZIER_HPP
#define BEZIER_HPP
#include <initializer_list>
#include <iterator>
#include <glm/glm.hpp>

template<int D>
glm::vec3 Decast(const glm::vec3 p[], float t);

template<>
inline glm::vec3 Decast<0>(const glm::vec3 p[], float t) { return *p; }

template<int D> 
glm::vec3 Decast(const glm::vec3 p[], float t) {
  return Decast<D-1>(p,t)*(1-t) + Decast<D-1>(p+1,t)*t;
}

template<int DEGREE=3>
class Bezier {
  glm::vec3 p[DEGREE+1];
public:
	Bezier() = default;
	Bezier(const std::initializer_list<glm::vec3> &l) {
		cg_assert(std::distance(l.begin(),l.end())==DEGREE+1,"wrong number of control points");
		auto it = l.begin();
		for(int j=0;j<=DEGREE;++j,++it) p[j] = *it;
	}
  glm::vec3 &operator[](int i) { return p[i]; }
  const glm::vec3 &operator[](int i) const { return p[i]; }
  glm::vec3 at(float t) const {
    return Decast<DEGREE>(p,t);
  }
  glm::vec3 at(float t, glm::vec3 &deriv) const {
    glm::vec3 x[2] = { Decast<DEGREE-1>(p,t), Decast<DEGREE-1>(p+1,t) };
    deriv = (x[1]-x[0])*float(DEGREE);
    return Decast<1>(x,t);
  }
  int degree() const { return DEGREE; }
};

#endif
