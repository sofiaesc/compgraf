#include "Misc.hpp"
#include "Debug.hpp"

std::string extractFolder(const std::string &filename) {
	int i = static_cast<int>(filename.size())-1;
	std::string path;
	while(i>0 and filename[i]!='\\' and filename[i]!='/' and filename[i]!=':') 
		--i;
	return filename.substr(0,i+1);
}

bool startsWith(const std::string str, const char *con) {
	int i=0, l=str.size();
	for(;con[i] && i<l;++i)
		if (con[i]!=str[i]) return false;
	return con[i]=='\0';
}

void fixEOL(std::string &s) {
	if ((not s.empty()) and s[s.size()-1]=='\r') 
		s.erase(s.size()-1);
}

std::pair<glm::vec3,glm::vec3> getBoundingBox(const std::vector<glm::vec3> &v) {
	cg_assert(not v.empty(),"Cannot generate Bounding Box for an empty vector");
	glm::vec3 pmin = v[0], pmax = v[0];
	for(glm::vec3 p : v) {
		for(int j=0;j<3;++j) {
			pmin[j] = std::min(pmin[j],p[j]);
			pmax[j] = std::max(pmax[j],p[j]);
		}
	}
	return {pmin,pmax};
}
