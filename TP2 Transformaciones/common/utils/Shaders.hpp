#ifndef SHADERS_H
#define SHADERS_H
#include <string>
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include "Material.hpp"
#include "Geometry.hpp"

class Shader {
public:
	Shader() = default;
	Shader(Shader &&other);
	Shader &operator=(Shader &&other);
	
	Shader(const std::string &fname);
	Shader(const std::string &vertex_fname, const std::string &fragment_fname);
	
	void load(const std::string &fname);
	void load(const std::string &vertex_fname, const std::string &fragment_fname);
	
	bool setBuffer (const char *name, GLuint buffer_id, GLenum type, int size, bool required=true);
	void setBuffers(const GeometryRenderer &geo);
	void setMaterial(const Material &mat);
	void setMatrixes(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);
	void setLight(const glm::vec4 &lightPosition, const glm::vec3 &lightColor, float ambientStrength);
	
	bool setUniform(const char *name, float v);
	bool setUniform(const char *name, const glm::vec3 &v);
	bool setUniform(const char *name, const glm::vec4 &v);
	bool setUniform(const char *name, const glm::mat4 &v);
	
	GLuint getProgramId() const { return program_id; }
	
	void use() const;
	~Shader();
private:
	Shader &operator=(const Shader &) = default;
	GLuint program_id = 0;
};

GLuint loadShader(GLenum shader_type, const std::string &file_path);

GLuint loadShaders(const std::string &vertex_path, const std::string &fragment_path);

#endif

