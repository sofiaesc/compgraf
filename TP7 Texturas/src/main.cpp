#include <algorithm>
#include <stdexcept>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ObjMesh.hpp"
#include "Shaders.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Model.hpp"

#define VERSION 20221019
#include <iostream>
using namespace std;

std::vector<glm::vec2> generateTextureCoordinatesForBottle(const std::vector<glm::vec3> &v) {
	
	// ETAPA: Rasterización
	// Tengo información de los vertices, rasterizador los interpola y le pasa al fragment shader.
	
	// SOLUCIÓN: Hacer el ultimo rectangulito pequeñito hasta que se note muy poco. 
	// Hacer calculo directamente en el fragment shader, no hay nada entre frag y frag para q se
	// note el error. Sí hay entre vértice y vértice.
	
	std::vector<glm::vec2> v_mapeado(v.size());
	float ang, s, t;
	
	// MAPEO EN DOS PARTES: CILÍNDRICO 
	
	for(size_t i=0;i<v.size();i++) { 
		// obtengo angulo de coordenadas cilíndricas:
		ang = atan2(v[i].x,v[i].z);
		// s = ang, t = v[i].y, pero con regla de 3 para que vaya de 0 a 1:
		s = (ang + 3.14) / 3.14; 
		t = (v[i].y + 0.15) / 0.4;
		v_mapeado[i] = glm::vec2(s,t);
	}
	
	return v_mapeado;
}

std::vector<glm::vec2> generateTextureCoordinatesForLid(const std::vector<glm::vec3> &v) {
	std::vector<glm::vec2> v_mapeado(v.size());
	float s,t;
	
	// MAPEO PLANO
	/* El plano ax+by+cz+d=0 se define mediante coordenadas {a,b,c,d}. Los primeros
	tres valores especifican un vector normal al plano y el cuarto es función lineal
	de la distancia del plano al origen. A cada coordenada de textura se le asigna
	un plano, definido por la distancia del punto al plano. */
	
	// textura debe posicionarse en el plano xz, el plano y es normal a ella.
	
	for(size_t i=0; i<v.size(); i++){
		// tapa va a estar en el plano xz:
		auto plano_s =  glm::vec4{0.f,0.f,1,v[i].z};
		auto plano_t =  glm::vec4{1,0.f,0.f,v[i].x}; 
		auto punto_actual = glm::vec4(v[i].x*5.f,v[i].y*5.f,v[i].z*5.f,1.f);
		
		// producto punto: me da un número proporcional a la distancia entre el punto y el plano
		s = glm::dot(plano_s, punto_actual)+0.5; // 0.5 -> traslación en s
		t = glm::dot(plano_t, punto_actual)+0.5; // 0.5 -> traslación en t
		
		v_mapeado[i] = glm::vec2(s,t);
	}
	
	return v_mapeado;
}

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Texturas");
	setCommonCallbacks(window);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glEnable(GL_BLEND); glad_glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.6f,0.6f,0.8f,1.f);
	Shader shader("shaders/texture");
	
	// load model and assign texture
	auto models = Model::load("bottle",Model::fKeepGeometry);
	Model &bottle = models[0], &lid = models[1];
	bottle.buffers.updateTexCoords(generateTextureCoordinatesForBottle(bottle.geometry.positions),true);
	bottle.texture = Texture("models/label.png",true,false);
	lid.buffers.updateTexCoords(generateTextureCoordinatesForLid(lid.geometry.positions),true);
	lid.texture = Texture("models/lid.png",false,false);
	
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		setMatrixes(shader);
		shader.setLight(glm::vec4{2.f,-2.f,-4.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.15f);
		for(Model &mod : models) {
			mod.texture.bind();
			shader.setMaterial(mod.material);
			shader.setBuffers(mod.buffers);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			mod.buffers.draw();
		}
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

