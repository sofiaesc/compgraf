#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include "Shaders.hpp"
#include "Debug.hpp"
#include "Misc.hpp"

static std::string getShaderSource(std::string file_path) {
	std::ifstream fs(file_path,std::ios::binary);
	cg_assert(fs.is_open(),"Could not open "+std::string(file_path));
	
	std::string folder = extractFolder(file_path);
	std::string full_content;
	for(std::string line; std::getline(fs,line); ) {
		fixEOL(line);
		if (startsWith(line,"#include ")) {
			auto p = line.find('\"');
			line.erase(0,p+1);
			p = line.find('\"');
			line.erase(p);
			line = getShaderSource(folder+line);
		}
		full_content += line + '\n';
	}
	
	return full_content;
}

static GLuint loadAndCompile(GLenum shader_type, const std::string &file_path) {
	GLuint shader_id = glCreateShader(shader_type);
	
	std::string shader_code = getShaderSource(file_path);
	
	cg_info("Compiling shader: " + file_path + "...");
	const char *shader_code_ptr = shader_code.c_str();
	glShaderSource(shader_id,1,&shader_code_ptr,nullptr);
	glCompileShader(shader_id);
	
	GLint result = GL_FALSE, log_len = 0;
	glGetShaderiv(shader_id,GL_COMPILE_STATUS,&result);
	glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&log_len);
	if (log_len) {
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader_id,log_len,nullptr,log.data());
		std::cerr << log.data() << std::endl;
	}
	cg_assert(result==GL_TRUE,"Failed to compile shader "+std::string(file_path));
	
	return shader_id;
}

Shader::Shader (const std::string &vertex_fname, const std::string &fragment_fname) {
	load(vertex_fname,fragment_fname);
}

Shader::Shader (const std::string &fname) {
	load(fname+".vert",fname+".frag");
}

Shader::Shader(Shader &&other) {
	*this = static_cast<const Shader&>(other);
	other = static_cast<const Shader&>(Shader());
}

Shader &Shader::operator=(Shader &&other) {
	*this = static_cast<const Shader&>(other);
	other = static_cast<const Shader&>(Shader());
	return *this;
}

void Shader::load(const std::string &vertex_fname, const std::string &fragment_fname) {
	cg_assert(program_id==0,"Shader already loaded");
	GLuint vertex_id = loadAndCompile(GL_VERTEX_SHADER,vertex_fname);
	GLuint fragment_id = loadAndCompile(GL_FRAGMENT_SHADER,fragment_fname);
	
	cg_info( "Linking shader program..." );
	program_id = glCreateProgram();
	glAttachShader(program_id,vertex_id);
	glAttachShader(program_id,fragment_id);
	glLinkProgram(program_id);
	
	GLint result = GL_FALSE, log_len = 0;
	glGetProgramiv(program_id,GL_LINK_STATUS,&result);
	glGetProgramiv(program_id,GL_INFO_LOG_LENGTH,&log_len);
	if (log_len) {
		std::vector<char> log(log_len);
		glGetProgramInfoLog(program_id,log_len,nullptr,log.data());
		std::cerr << log.data() << std::endl;
	}
	cg_assert(result==GL_TRUE,"Failed to link shader program");
	
	glDetachShader(program_id,vertex_id);
	glDetachShader(program_id,fragment_id);
	
	glDeleteShader(vertex_id);
	glDeleteShader(fragment_id);
}

void Shader::load(const std::string &fname) {
	load(fname+".vert",fname+".frag");
}


bool Shader::setBuffer (const char *name, GLuint id, GLenum type, int size, bool required) {
	glBindBuffer(GL_ARRAY_BUFFER,id); /// todo: no va type?
	GLint loc = glGetAttribLocation(program_id, name); 
	if (loc==-1 and (not required)) return false;
	cg_assert(loc!=-1,"Shader does not have required attribute");
	glVertexAttribPointer(loc, size, type, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc);
	return true;
}

void Shader::setBuffers (const GeometryRenderer & geo) {
	glBindVertexArray(geo.vertexArray());
	
	{ // positions
		glBindBuffer(GL_ARRAY_BUFFER,geo.positionsVBO());
		GLint loc_pos = glGetAttribLocation(program_id, "vertexPosition"); 
		cg_assert(loc_pos!=-1,"Shader does not have vertexPosition attribute");
		glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc_pos);
	}
	
	GLint loc_norm = glGetAttribLocation(program_id, "vertexNormal"); 
	if (loc_norm!=-1) { // normals
		cg_assert(geo.normalsVBO()!=0,"Geometry does not have normals");
		glBindBuffer(GL_ARRAY_BUFFER,geo.normalsVBO());
		glVertexAttribPointer(loc_norm, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc_norm);
	}
	
	GLint loc_tc = glGetAttribLocation(program_id, "vertexTexCoords"); 
	if (loc_tc!=-1) { // texture coords
		glBindBuffer(GL_ARRAY_BUFFER,geo.texCoordsVBO());
		cg_assert(geo.texCoordsVBO()!=0,"Geometry does not have texture coordinates");
		glVertexAttribPointer(loc_tc, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(loc_tc);
	}
	
}

bool Shader::setUniform(const char *name, float v) {
	GLint pos = glGetUniformLocation(program_id, name); 
	if (pos==-1) return false;
	glUniform1f(pos,v);
	return true;
}

bool Shader::setUniform(const char *name, const glm::vec3 &v) {
	GLint pos = glGetUniformLocation(program_id, name); 
	if (pos==-1) return false;
	glUniform3f(pos,v.x,v.y,v.z);
	return true;
}

bool Shader::setUniform(const char *name, const glm::vec4 &v) {
	GLint pos = glGetUniformLocation(program_id, name); 
	if (pos==-1) return false;
	glUniform4f(pos,v.x,v.y,v.z,v.w);
	return true;
}

bool Shader::setUniform(const char *name, const glm::mat4 &m) {
	GLint pos = glGetUniformLocation(program_id, name); 
	if (pos==-1) return false;
	glUniformMatrix4fv(pos, 1, GL_FALSE, &m[0][0]);
	return true;
}

void Shader::setMaterial (const Material &mat) {
	setUniform("diffuseColor", mat.kd);
	setUniform("specularColor", mat.ks);
	setUniform("ambientColor", mat.ka);
	setUniform("emissionColor", mat.ke);
	setUniform("opacity", mat.opacity);
	setUniform("shininess", mat.shininess);
}

Shader::~Shader ( ) {
	if (program_id!=0) glDeleteProgram(program_id);
}

void Shader::use() const {
	cg_assert(program_id!=0,"Shader not initialized");
	glUseProgram(program_id);
}

void Shader::setMatrixes (const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
	setUniform("modelMatrix",model);
	setUniform("viewMatrix",view);
	setUniform("projectionMatrix",projection);
}

void Shader::setLight (const glm::vec4 &lightPosition, const glm::vec3 &lightColor, float ambientStrength) {
	setUniform("lightPosition",lightPosition);
	setUniform("lightColor",lightColor);
	setUniform("ambientStrength",ambientStrength);
}

void Shader::setLightX(int i, const glm::vec4 &lightPosition, const glm::vec3 &lightColor, float ambientStrength) {
	static char cs_lp[] = "lightPositionX";
	static char cs_lc[] = "lightColorX";
	static char cs_as[] = "ambientStrengthX";
	cs_lp[13] = cs_lc[10] = cs_as[15] = '0'+i;
	setUniform(cs_lp,lightPosition);
	setUniform(cs_lc,lightColor);
	setUniform(cs_as,ambientStrength);
}

