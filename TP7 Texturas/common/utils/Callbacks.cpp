#include "Callbacks.hpp"

#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Window.hpp"
#include "Shaders.hpp"

// window and view
int win_width = 800, win_height = 600;
glm::vec3 view_target = {0.f,0.f,0.f}, view_pos = {0.f,0.f,3.f};
float model_angle = 0.f, view_angle = 0.3f, view_fov = 45.f;
bool use_perspective = true;

namespace common_callbacks {
	
// mouse state
enum class MouseAction { None, Rotate, Zoom, Pan };
MouseAction mouse_action = MouseAction::None;
glm::vec2 last_mouse_pos;

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
	glm::vec2 current_mouse_pos(xpos,ypos);
	glm::vec2 delta = current_mouse_pos - last_mouse_pos;
	switch(mouse_action) {
	case MouseAction::Pan:
		view_target.x -= delta.x/win_width*2.f;
		view_target.y += delta.y/win_height*2.f;
		view_target.x = std::min(1.f,std::max(-1.f,view_target.x));
		view_target.y = std::min(1.f,std::max(-1.f,view_target.y));
	break;
	case MouseAction::Zoom:
		view_fov += (-delta.x/win_width+delta.y/win_height)*50.f;
		view_fov = std::min(90.f,std::max(5.f,view_fov));
	break;
	case MouseAction::Rotate:
		model_angle += 5.f*delta.x/win_width;
		view_angle += 3.f*delta.y/win_height;
	break;
	default:
		;
	}
	last_mouse_pos = current_mouse_pos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (Window::IsImGuiEnabled(window) and ImGui::GetIO().WantCaptureMouse) return;
	if (button==GLFW_MOUSE_BUTTON_LEFT) {
		if(action == GLFW_RELEASE) mouse_action = MouseAction::None;
		else {
			if      (mods&GLFW_MOD_SHIFT)   mouse_action = MouseAction::Zoom;
			else if (mods&GLFW_MOD_CONTROL) mouse_action = MouseAction::Pan;
			else if (not mods)              mouse_action = MouseAction::Rotate;
		}
	}
}

void viewResizeCallback(GLFWwindow *, int w, int h) { 
	glViewport(0,0,w,h); win_width = w; win_height = h;
}

std::array<glm::mat4,3> getMatrixes() {
	return { glm::rotate(glm::mat4(1.f),view_angle,glm::vec3{1.f,0.f,0.f}) * glm::rotate(glm::mat4(1.f),model_angle,glm::vec3{0.f,1.f,0.f}),
	         glm::lookAt( view_pos, view_target, glm::vec3{0.f,1.f,0.f} ),
		     use_perspective
			   ? glm::perspective( glm::radians(view_fov), float(win_width)/float(win_height), 0.1f, 100.f )
			   : glm::ortho(-1.5f,+1.5f,-1.5f,+1.5f,-100.f,+100.f)
	       };
}

} // namespace

void setCommonCallbacks(GLFWwindow * window) {
	glfwSetFramebufferSizeCallback(window,common_callbacks::viewResizeCallback);
	glfwSetCursorPosCallback(window, common_callbacks::mouseMoveCallback);
	glfwSetMouseButtonCallback(window, common_callbacks::mouseButtonCallback);
}

void setMatrixes(Shader &shader) {
	auto ms = common_callbacks::getMatrixes();
	shader.setMatrixes(ms[0],ms[1],ms[2]);
}

