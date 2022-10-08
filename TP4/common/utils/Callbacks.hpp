#ifndef CALLBACKS_HPP
#define CALLBACKS_HPP

#include <array>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// window and view
extern int win_width, win_height;
extern glm::vec3 view_target, view_pos;
extern float model_angle, view_angle, view_fov;
extern bool use_perspective;

namespace common_callbacks {
	
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void viewResizeCallback(GLFWwindow *, int w, int h);

std::array<glm::mat4,3> getMatrixes();

} // anonymous namespace

void setCommonCallbacks(GLFWwindow* window);

class Shader;
void setMatrixes(Shader &shader);

#endif

