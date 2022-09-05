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

// models and settings
std::vector<std::string> models_names = { "suzanne", "chookity", "teapot" };
int current_model = 0;
bool rotate = true, wireframe = false, enable_texture = true;

// extraa callbacks
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glClearColor(0.3f,0.3f,0.4f,1.f);
	Shader shader_texture("shaders/texture"),
		   shader_phong("shaders/phong"),
		   shader_wire("shaders/wireframe");
	
	// main loop
	Model model;
	int loaded_model = -1;
	FrameTimer ftime;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		// reload model if necessary
		if (loaded_model!=current_model) { 
			model = Model::loadSingle(models_names[current_model]);
			loaded_model = current_model;
		}
		
		// auto-rotate
		double dt = ftime.newFrame();
		if (rotate) model_angle += static_cast<float>(1.f*dt);
		
		// select a shader
		Shader &shader = [&]()->Shader&{
			if (wireframe) return shader_wire;
			if (enable_texture and model.texture.isOk()) {
				model.texture.bind();
				return shader_texture;
			}
			return shader_phong;
		}();
		shader.use();
		setMatrixes(shader);
		
		// setup light and material
		shader.setLight(glm::vec4{-1.f,1.f,4.f,1.f}, glm::vec3{1.f,1.f,1.f}, 0.35f);
		shader.setMaterial(model.material);
		
		// send geometry
		shader.setBuffers(model.buffers);
		glPolygonMode(GL_FRONT_AND_BACK,wireframe?GL_LINE:GL_FILL);
		model.buffers.draw();
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			ImGui::Combo(".obj (O)", &current_model,models_names);		
			ImGui::Checkbox("Auto-rotate (R)",&rotate);
			ImGui::Checkbox("Wireframe (W)",&wireframe);
			if (model.texture.isOk())
				ImGui::Checkbox("Use textures (T)",&enable_texture);
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'R': rotate = !rotate; break;
		case 'W': wireframe = !wireframe; break;
		case 'T': enable_texture = !enable_texture; break;
		case 'O': case 'M': current_model = (current_model+1)%models_names.size(); break;
		}
	}
}
