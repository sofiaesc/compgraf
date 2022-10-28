#include "Spline.hpp"

const static int degree = 3;

Spline::Spline (const std::vector<glm::vec3> & vp) {
	auto it = vp.begin();
	glm::vec3 p = *it, q = *it;;
	while(++it!=vp.end()) {
		glm::vec3 q = *it;
		pieces.push_back(Bezier<>({p,(p+p+q)/3.f,(p+q+q)/3.f,q}));
		p = q;
	}
	pieces.push_back(Bezier<>({p,(p+p+q)/3.f,(p+q+q)/3.f,q}));
}

void Spline::setControlPoint (int i, glm::vec3 p) {
	auto idxs = getIndexes(i);
	pieces[idxs.first][idxs.second] = p;
	if (idxs.second==0) {
		int psz = pieces.size();
		pieces[(idxs.first+psz-1)%psz][degree] = p;
	}
}

glm::vec3 Spline::getControlPoint (int i) const {
	auto p = getIndexes(i);
	return pieces[p.first][p.second];
}

glm::vec3 Spline::at (double t) const {
	auto p = mapT(t);
	return pieces[p.first%pieces.size()].at(p.second);
}

glm::vec3 Spline::at(double t, glm::vec3 & deriv) const {
	auto p = mapT(t);
	return pieces[p.first%pieces.size()].at(p.second,deriv);
}

std::pair<int,double> Spline::mapT (double t) const {
	double taux = t*pieces.size();
	int ipiece = static_cast<int>(taux);
	double tpiece = taux - static_cast<double>(ipiece);
	return {ipiece,tpiece};
}

std::pair<int,int> Spline::getIndexes (int i) const {
	if (i<0) i+=pieces.size()*degree;
	int icurva = (i/degree)%pieces.size();
	int ipt = i%degree;
	return {icurva,ipt};
}

int Spline::getControlPointsCount() const {
	return degree*pieces.size(); 
}

const std::vector<Bezier<>> &Spline::getPieces() const { 
	return pieces; 
}
