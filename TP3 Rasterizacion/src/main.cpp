#include <glad/glad.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <array>
#include "Window.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"
#include "Bezier.hpp"
#include "Renderer.hpp"
#include "ScreenCapture.hpp"
#include "RasterAlgs.hpp"

#define VERSION 20220909.1

// model data
Bezier<glm::vec2,3> curve = { {60.5f,60.5f}, {100.5f,310.5f}, {200.5f,70.5f}, {498.5f,150.5f} };
std::vector<glm::vec2> segments;

static const glm::vec4 color_segs_int  = { 1.0f, 0.2f, 0.2f, 0.5f };
static const glm::vec4 color_segs_ext  = { 0.8f, 0.0f, 0.0f, 1.0f };
static const glm::vec4 color_segs_ref  = { 0.0f, 0.7f, 0.0f, 1.0f };
static const glm::vec4 color_curve_int = { 0.2f, 0.2f, 1.0f, 0.5f };
static const glm::vec4 color_curve_ext = { 0.2f, 0.0f, 0.8f, 1.0f };
static const glm::vec4 color_curve_pol = { 0.0f, 0.0f, 0.0f, 0.2f };
static const glm::vec4 color_zoom_area = { 0,0,0,.35 };


struct MainWin {
	int width = 800, height = 600;
	Window ptr;
	glm::mat4 matrix;
} main_win;
struct ZoomWinInfo {
	Window ptr;
	int width = 380, height = 600;
	glm::mat4 matrix;
	glm::vec2 p0 = {75.f,140.f};
	int factor = 10;
} zoom_win;


void viewResizeCallback(GLFWwindow *ptr, int w, int h) {
	glfwMakeContextCurrent(ptr);
	glViewport(0,0,w,h); 
	if (ptr==main_win.ptr) {
		main_win.width = w; main_win.height = h;
		main_win.matrix = glm::ortho<float>(0,w,0,h);
	} else {
		zoom_win.width = w; zoom_win.height = h;
		zoom_win.matrix = glm::ortho<float>(0,w,0,h);
	}
};

float redon(float x) { return int(x)+.5f; }
glm::vec2 redon(glm::vec2 p) { return {redon(p.x),redon(p.y)}; }

// callbacks
void mainMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void mainMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void zoomMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void zoomMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void zoomScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

std::vector<glm::vec2> fragments;
void paintPixel(glm::vec2 p) { fragments.push_back(p); }
curveRetVal evalCurve(float t) { curveRetVal ret; ret.p = curve.at(t,ret.d); return ret; }

int main() {
	
	// initialize main window and setup callbacks
	main_win.ptr = Window(main_win.width+zoom_win.width,main_win.height,"Area de trabajo",0);
	glfwSetFramebufferSizeCallback(main_win.ptr, viewResizeCallback);
	glfwSetCursorPosCallback(main_win.ptr, mainMouseMoveCallback);
	glfwSetMouseButtonCallback(main_win.ptr, mainMouseButtonCallback);
	glfwSetWindowSize(main_win.ptr,main_win.width,main_win.height);
	viewResizeCallback(main_win.ptr,main_win.width,main_win.height);
	
	// auxiliars for rendering
	Renderer renderer;
	constexpr float pi = 4.f*atan(1), r = 17;
	glm::vec2 c = {450.5f, 380.5f};
	for(int i=0;i<10;i++) {
		float ang = i*(2*pi)/10;
		glm::vec2 p = {std::cos(ang), std::sin(ang)};
		segments .push_back(redon(c+r*p));
		segments .push_back(redon(c+r*p*10.f));
	}
	std::array<glm::vec2,4> zoom_rect;
	
	
	// initialize zoom window and setup callbacks
	zoom_win.ptr = Window(zoom_win.width,zoom_win.height,"Zoom",0,main_win.ptr);
	viewResizeCallback(zoom_win.ptr,zoom_win.width,zoom_win.height);
	glfwSetFramebufferSizeCallback(zoom_win.ptr, viewResizeCallback);
	glfwSetCursorPosCallback(zoom_win.ptr, zoomMouseMoveCallback);
	glfwSetMouseButtonCallback(zoom_win.ptr, zoomMouseButtonCallback);
	glfwSetScrollCallback(zoom_win.ptr, zoomScrollCallback);
	int main_x, main_y;
	glfwGetWindowPos(main_win.ptr, &main_x, &main_y);
	glfwSetWindowPos(zoom_win.ptr,main_x+main_win.width,main_y);
	
	// auxiliars for capturing and zooming main windows content
	ScreenCapture capture;
	
	
	// main loop
	do {
		
		// --- main window ---
		glfwMakeContextCurrent(main_win.ptr);
		glClearColor(1.f,1.f,1.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		renderer.getShader().setUniform("matrix",main_win.matrix);
		
		// control points and control polygon
		renderer.drawPoints(curve,color_curve_ext,3);
		renderer.drawSegments(curve,color_curve_pol,1);
		renderer.drawPoints(segments,color_segs_ext,3);
		
		// rasterized segments
		fragments.clear();
		for(size_t i=0;i<segments.size();i+=2)
			drawSegment(paintPixel,segments[i],segments[i+1]);
		renderer.drawPoints(fragments,color_segs_int,1);
		
		// rasterized lines
		fragments.clear();
		drawCurve(paintPixel,evalCurve);
		renderer.drawPoints(fragments,color_curve_int,1);
		
		// capturar el contenido de la ventana principal
		int zw = zoom_win.width/zoom_win.factor+1, 
			zh = zoom_win.height/zoom_win.factor+1;
		capture.take(zoom_win.p0.x,zoom_win.p0.y,zw,zh,zoom_win.factor);
		
		// zoom area
		// marcar el area del zoom en la ventana principal
		zoom_rect[0] = zoom_win.p0;
		zoom_rect[1] = zoom_win.p0+glm::vec2{zw,0.f};
		zoom_rect[2] = zoom_win.p0+glm::vec2{zw,zh};
		zoom_rect[3] = zoom_win.p0+glm::vec2{0.f,zh};;
		renderer.drawPolygon(zoom_rect,color_zoom_area);
		renderer.drawPoint(zoom_win.p0,color_zoom_area,5);
		
		glfwSwapBuffers(main_win.ptr);
		
		// --- zoom window ---
		glfwMakeContextCurrent(zoom_win.ptr);
		
		// mostrar la captura aumentada en la ventana de zoom con su grilla
		// (a la grilla la hace el shader con estos parametros que le pasamos)
		auto &shader = capture.getShader();
		shader.setUniform("matrix",zoom_win.matrix);
		shader.setUniform("width",float(zoom_win.width));
		shader.setUniform("height",float(zoom_win.height));
		shader.setUniform("factor",float(zoom_win.factor));
		shader.setUniform("ate",.6f+0.4f*std::max(0.f,std::min(10-(zoom_win.factor-3),10)*.1f) );
		capture.draw();
		
		glfwSwapBuffers(zoom_win.ptr);
		
		// --- finish frame ---
		
		glfwPollEvents();
		
	} while( glfwGetKey(main_win.ptr,GLFW_KEY_ESCAPE)!=GLFW_PRESS and (not glfwWindowShouldClose(main_win.ptr)) and
	         glfwGetKey(zoom_win.ptr,GLFW_KEY_ESCAPE)!=GLFW_PRESS and (not glfwWindowShouldClose(zoom_win.ptr)) );
}

// en la ventana principal: drag=mover un vertice/pto de control
glm::vec2 *current_selection = nullptr;
glm::vec2 *closestPoint(glm::vec2 p) {
	float min_d2 = std::numeric_limits<float>::max();
	glm::vec2 *sel;
	auto process = [&](glm::vec2 *v, int n) {
		for(size_t i=0;i<n;++i) {
			auto aux_v = v[i]-p;
			float aux_d2 = glm::dot(aux_v,aux_v);
			if (aux_d2<min_d2) {
				sel = v+i;
				min_d2 = aux_d2;
			}
		}
	};
	process(curve.data(),curve.size());
	process(segments.data(),segments.size());
	process(&zoom_win.p0,1);
	if (min_d2>100) sel = nullptr;
	return sel;
}
void mainMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		// pasar de coords de la ventana a coords del modelo
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		// buscar punto cercano al click
		current_selection = closestPoint(glm::vec2{xpos,main_win.height-ypos});
	} else {
		current_selection = nullptr; // soltar el pto al soltar el boton
	}
}
void mainMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
	if (current_selection)
		*current_selection = glm::vec2{xpos,main_win.height-ypos};
}

// en la ventana del zoom: drag=mover vista; scroll=cambiar nivel de zoom
glm::vec2 zoom_drag_p0; bool zoom_dragging = false;
void zoomMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		zoom_dragging = true;
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		zoom_drag_p0.x = xpos;
		zoom_drag_p0.y = zoom_win.height-ypos;
	} else {
		zoom_dragging = false;
	}
}
void zoomMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
	if (not zoom_dragging) return;
	zoom_win.p0 -= (glm::vec2(xpos,zoom_win.height-ypos)-zoom_drag_p0)/float(zoom_win.factor);
	zoom_drag_p0.x = xpos; zoom_drag_p0.y = zoom_win.height-ypos;
}
void zoomScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	zoom_win.factor = std::max(3,zoom_win.factor+int(yoffset));
}

