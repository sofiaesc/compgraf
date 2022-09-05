#include <fstream>
#include <map>
#include <algorithm>
#include <glm/glm.hpp>
#include "ObjMesh.hpp"
#include "Debug.hpp"
#include "Misc.hpp"
#include <unordered_map>

namespace {

float readInt(const std::string &s, int &i) {
	char *p = const_cast<char*>(s.c_str())+i;
	int r = std::strtol(p,&p,10);
	i = p-s.c_str();
	return r;
}

float readFloat(const std::string &s, int &i) {
	char *p = const_cast<char*>(s.c_str())+i;
	float r = std::strtof(p,&p);
	i = p-s.c_str();
	return r;
}

float readFloat(const std::string &s, const int &i) {
	int j = i; return readFloat(s,j);
}

glm::vec3 readVec3(const std::string &s, int i) {
	glm::vec3 v;
	v.x = readFloat(s,i); ++i;
	v.y = readFloat(s,i); ++i;
	v.z = readFloat(s,i);
	return v;
}

glm::vec2 readVec2(const std::string &s, int i) {
	glm::vec2 v;
	v.x = readFloat(s,i); ++i;
	v.y = readFloat(s,i);
	return v;
}

std::map<std::string,Material> loadMaterialsLib(const std::string &path, const std::string &filename) {
	cg_info( "Reading mtl file: " + path+filename + "...");
	std::ifstream file(path+filename);
	cg_assert(file.is_open(),"Could not open mtl file");
	
	std::map<std::string,Material> lib;
	Material *current_material = nullptr;
	for(std::string line; std::getline(file,line); ) {
		if (line.empty() or line[0]=='#') continue;
		if (startsWith(line,"newmtl ")) {
			cg_assert(lib.count(line.substr(7))==0,"Duplicate material name");
			current_material = &lib[line.substr(7)];
		} else {
			cg_assert(current_material,"Material property before command newmtl");
			if (startsWith(line,"Ks ")) {
				current_material->ks = readVec3(line,3);
			} else if (startsWith(line,"Ka ")) {
				current_material->ka = readVec3(line,3);
			} else if (startsWith(line,"Kd ")) {
				current_material->kd = readVec3(line,3);
			} else if (startsWith(line,"Ke ")) {
				current_material->ke = readVec3(line,3);
			} else if (startsWith(line,"Ns ")) {
				current_material->shininess = readFloat(line,3);
			} else if (startsWith(line,"d ")) {
				current_material->opacity = readFloat(line,2);
			} else if (startsWith(line,"Tr ")) {
				current_material->opacity = 1.f-readFloat(line,3);				
			} else if (startsWith(line,"map_Kd ")) {
				current_material->texture = path+line.substr(7);
			}
		}
	}
	return lib;
}

}

ObjMesh readObj(const std::string &full_path) {
	cg_info( "Reading obj file: " + full_path + "..." );
	std::string path = extractFolder(full_path);
	std::ifstream file(full_path);
	cg_assert(file.is_open(),"Could not open obj file");
	
	ObjMesh meshes;
	ObjMesh::Part *current_part = nullptr;
	std::map<std::string,Material> materials_lib;
	std::string current_name;
	
	for(std::string line; std::getline(file,line); ) {
		if (line.empty() or line[0]=='#') continue;
		if (startsWith(line,"o ")) {
			meshes.parts.push_back({}); 
			current_part = &meshes.parts.back();
			current_name = current_part->name = line.substr(2);
		} else if (startsWith(line,"mtllib ")) {
			materials_lib = loadMaterialsLib(path,line.substr(7));
		} else {
			if (not current_part) {
				meshes.parts.push_back({});
				current_part = &meshes.parts.back();
			}
			if (startsWith(line,"v ")) {
				meshes.positions.push_back(readVec3(line,2));
			} else if (startsWith(line,"vn ")) {
				meshes.normals.push_back(readVec3(line,3));
			} else if (startsWith(line,"vt ")) {
				meshes.tex_coords.push_back(readVec2(line,3));
			} else if (startsWith(line,"f ")) {
				ObjMesh::Element e; 
				int is = 2, in = 0, l = line.size();
				while(is<l) {
					cg_assert(in<4,"Face with more than 4 vertexes are not supported yet");
					e.pos[in] = readInt(line,is)-1;
					if (line[is]=='/') {
						if (line[++is]=='/') {
							e.tcs[in] = -1;
							e.norms[in] = readInt(line,++is)-1;
						} else {
							e.tcs[in] = readInt(line,is)-1;
							if (line[is]=='/') {
								e.norms[in] = readInt(line,++is)-1;
							} else {
								e.norms[in] = -1;
							}
						}
					} else {
						e.tcs[in] = -1;
						e.norms[in] = -1;
					}
					++in;
				}
				cg_assert(in>2,"Face with less than 3 vertexes");
				if (in==3) e.pos[3] = e.norms[3] = e.tcs[3] = -1;
				current_part->elements.push_back(e);
			} else if (startsWith(line,"usemtl ")) {
				if (not current_part->elements.empty()) {
					meshes.parts.push_back({}); 
					current_part = &meshes.parts.back();
				}
				current_part->name = current_name+":"+line.substr(7);
				if  (line.substr(7)!="None") {
					cg_assert(materials_lib.count(line.substr(7)),"Material not found");
					current_part->material = materials_lib[line.substr(7)];
				}
			}
		}
	}
	cg_assert(not meshes.parts.empty(),"No mesh object found in file");
	
	return meshes;
}

//Geometry toGeometry(const ObjMesh &obj, const ObjMesh::Part &part) {
//	Geometry g;
//	auto addVertex = [&g,&obj](const ObjMesh::Element &e, int inode) {
//		g.positions.push_back(obj.positions[e.pos[inode]]);
//		if (e.norms[inode]!=-1) g.normals.push_back(obj.normals[e.norms[inode]]);
//		if (e.tcs[inode]!=-1) g.tex_coords.push_back(obj.tex_coords[e.tcs[inode]]);
//	};
//	for(const ObjMesh::Element &e : part.elements) {
//		addVertex(e,0); addVertex(e,1); addVertex(e,2);
//		if (e.pos[3]==-1) continue;
//		addVertex(e,0); addVertex(e,2); addVertex(e,3);
//	}
//	return g;
//}

namespace std {
	
	template <> struct hash<std::tuple<int,int,int>> {
		std::size_t operator()(const std::tuple<int,int,int>& p) const {
			return ( ( hash<int>()(std::get<0>(p))
		               ^ (hash<int>()(std::get<1>(p)) << 1) ) >> 1)
				   ^ (hash<int>()(std::get<0>(p)) << 1);
		}
	};
	
}

Geometry toGeometry(const ObjMesh &obj, const ObjMesh::Part &part) {
	Geometry g;
	std::unordered_map<std::tuple<int,int,int>,int> map;
	auto addVertex = [&g,&obj,&map](const ObjMesh::Element &e, int inode) {
		auto t = std::make_tuple(e.pos[inode],e.norms[inode],e.tcs[inode]);
		auto p = map.insert({t,g.positions.size()});
		if (p.second) {
			g.positions.push_back(obj.positions[e.pos[inode]]);
			if (e.norms[inode]!=-1) g.normals.push_back(obj.normals[e.norms[inode]]);
			if (e.tcs[inode]!=-1) g.tex_coords.push_back(obj.tex_coords[e.tcs[inode]]);
		}
		g.triangles.push_back(p.first->second);
	};
	for(const ObjMesh::Element &e : part.elements) {
		addVertex(e,0); addVertex(e,1); addVertex(e,2);
		if (e.pos[3]==-1) continue;
		addVertex(e,0); addVertex(e,2); addVertex(e,3);
	}
	return g;
}

Geometry toGeometry(const ObjMesh &obj, int ipart) {
	return toGeometry(obj,obj.parts[ipart]);
}

Geometry toGeometry(const ObjMesh &obj, const std::string &name) {
	return toGeometry(obj,obj.getPart(name));
}

const ObjMesh::Part &ObjMesh::getPart(const std::string &name) const {
	auto it = std::find_if(parts.begin(),parts.end(),
						   [name](const ObjMesh::Part &p) {
							   return p.name==name; 
						   });
	cg_assert(it!=parts.end(),"Part name not found");
	return *it;
}

