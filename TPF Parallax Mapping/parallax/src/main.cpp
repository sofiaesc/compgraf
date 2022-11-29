#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"

#define VERSION 20220816

std::vector<std::string> models_names = { "Model 1", "Model 2", "Model 3" };
bool doNormalMap = true, doSteepParallax=true, doOcclusionParallax=false;
float height_scale= 0.2f, numLayers= 100.f;
int current_model = 0;
glm::vec4 lightpos= {0.f,0.f,1.f,1.f};
// extraa callbacks
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);
int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glClearColor(0.0f,0.0f,0.0f,1.f);
	Shader shader("shaders/parallax");
	
	//set up model
	std::vector<glm::vec3>  positions = {{-0.5f, -0.5f,  0.f},{0.5f, -0.5f, 0.f},{0.5f,0.5f, 0.f},{-0.5f, 0.5f,  0.f}};
	std::vector<glm::vec3>  normals = {{0.f, 0.0f, 1.f},{0.f, 0.0f,  1.f},{0.f, 0.0f,  1.f},{0.f, 0.0f,  1.f}};
	std::vector<glm::vec2>  tex_coords = {{0.0f, 0.0f},{1.0f, 0.0f},{1.0f, 1.0f},{0.0f, 1.0f}};

	std::vector<int> triangles= {0,1,2,0,3,2,0};
	Geometry plane= {positions,normals,tex_coords,triangles};

	Material material;
	Model model= Model(plane,material);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //nose si esto esta cambiando algo la vdd
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	model.texture = Texture("models/parallax/base_case_1.png",false,false);
	model.normalmap = Texture("models/parallax/normal_case_1.png",false,false);
	model.parallax = Texture("models/parallax/bw_case_1.png",false,false);
	
	//calculate tangents and bitangents of both triangles
	
	//triangulo 1
	
	glm::vec3 edge1 = positions[0]-positions[3];
	glm::vec3 edge2 = positions[1]-positions[3];
	glm::vec2 deltaUV1 = tex_coords[0]-tex_coords[3];
	glm::vec2 deltaUV2 = tex_coords[1]-tex_coords[3];
	
	float f = 1.0f/(deltaUV1.x*deltaUV2.y-deltaUV2.x*deltaUV1.y);//determinante de la matriz (para conseguir la inverwsa)
	glm::vec3 tangent1;
	glm::vec3 bitangent1;
	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	
	
	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	
std::string tex= "models/parallax/base_case_";
std::string norm= "models/parallax/normal_case_";
std::string depth= "models/parallax/bw_case_";
	int loaded_model = -1;
	// main loop
	do {
		

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		// reload model if necessary
			if (loaded_model!=current_model) { 
			loaded_model=current_model;			
			std::string i= std::to_string(loaded_model+1);
			
			model.texture = Texture(tex+ i+".png",false,false);
			model.normalmap = Texture(norm+i+".png",false,false);
			model.parallax = Texture(depth+i+".png",false,false);
		}

		// auto-rotate
		shader.use();
		setMatrixes(shader);

		// setup light and material
		shader.setLight(lightpos, glm::vec3{1.f,1.f,1.f}, 0.35f);
		shader.setMaterial(model.material);
		// send geometry
		model.texture.bind(0);
		model.normalmap.bind(1);
		model.parallax.bind(2);
		
		shader.setUniform("colorTexture",0);
		shader.setUniform("normal0",1);
		shader.setUniform("parallax",2);
		
		shader.setUniform("aTangent",tangent1);
		shader.setUniform("aBitangent",bitangent1);
		
		shader.setUniform("height_scale",height_scale); // 0.f = desactiva parallax
		shader.setUniform("doSteepParallax",doSteepParallax); //Hacer Steep Parllax (dividir en capaz para calcular mejor p
		shader.setUniform("numLayers",numLayers); // layers (divisiones) en el parallax <recomendado algo mayor a 100>
		shader.setUniform("doNormalMap",doNormalMap); 	// activar o desactivar normalmap
		shader.setUniform("doOcclusionParallax",doOcclusionParallax); 	// activar o desactivar normalmap
		shader.setMaterial(model.material);
		shader.setBuffers(model.buffers);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		model.buffers.draw();
		// settings sub-window
		window.ImGuiDialog("CG Parallax",[&](){
			ImGui::Combo("Textura", &current_model,models_names);	
			ImGui::Checkbox("NormalMap",&doNormalMap);
			ImGui::SliderFloat("P. Scale", &height_scale,0.f,.2f);
			ImGui::Checkbox("Steep Parallax",&doSteepParallax);
			ImGui::SliderFloat("P. Layers", &numLayers,0.f,200.f,"%.f",0);
			ImGui::Checkbox("Occlusion Parallax",&doOcclusionParallax);
			ImGui::SliderFloat("Light X", &lightpos.x,-5.f,5.f,"%.1f",0);
			ImGui::SliderFloat("Light Y", &lightpos.y,-5.f,5.f,"%.1f",0);
			ImGui::SliderFloat("Light Z", &lightpos.z,-5.f,5.f,"%.1f",0);
		});
			
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'N': doNormalMap = !doNormalMap; break;
		
		case 'O': case 'M': current_model = (current_model+1)%models_names.size(); break;
		}
	}
}
