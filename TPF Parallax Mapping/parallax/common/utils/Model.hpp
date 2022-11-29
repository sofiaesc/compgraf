#ifndef MODEL_HPP
#define MODEL_HPP
#include <vector>
#include "Geometry.hpp"
#include "Material.hpp"
#include "Texture.hpp"

// auxiliar struct for loading all model-related data
struct Model {
	Geometry geometry;
	GeometryRenderer buffers;
	Material material;
	Texture texture;
	Texture normalmap;
	Texture parallax;
	
	Model() = default;
	Model(const Geometry &g, const Material &m) 
		: buffers(g), material(m), 
		  texture(m.texture.empty() ? Texture() : Texture(m.texture)),
		  normalmap(m.normalmap.empty() ? Texture() : Texture(m.normalmap)),
		parallax(m.parallax.empty() ? Texture() : Texture(m.parallax))
	{
		
	}
	Model(Geometry &&g, const Material &m, bool keep_geometry=false) 
		: buffers(g), material(m), 
		  texture(m.texture.empty() ? Texture() : Texture(m.texture)),
		normalmap(m.normalmap.empty() ? Texture() : Texture(m.normalmap)),
		parallax(m.parallax.empty() ? Texture() : Texture(m.parallax))
	{
		if (keep_geometry) geometry = std::move(g);
	}
	
	enum Flags { fNone=0, fDontFit=1, fKeepGeometry=2, 
				 fRegenerateNormals=4, fDynamic=8 };
	static std::vector<Model> load(const std::string &name, int flags = 0);
	static Model loadSingle(const std::string &name, int flags = 0);
};

void centerAndResize(std::vector<glm::vec3> &v);

#endif

