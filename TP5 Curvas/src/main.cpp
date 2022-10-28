#include <algorithm>
#include <stdexcept>
#include <vector>
#include <limits>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"
#include "BezierRenderer.hpp"
#include "Spline.hpp"

#define VERSION 20221004

// settings
bool show_axis = false, show_fish = false, show_spline = true, show_poly = true, animate = true;

// curva
static const int degree = 3;
int ctrl_pt = -1, cant_pts = 6; // pto de control seleccionado (para el evento de arrastre)
Spline spline( { {-1.f,0.f,0.f}, {0.f,0.f,-1.f}, {1.f,0.f,0.f}, {0.f,0.f,1.f} } );

void updateControlPointsAround(Spline &spline, int ctrl_pt) {
	
	// método de overhausser sin corrección de extremos
	// porque se trabaja con curvas cerradas: cuando
	// termina el vector de nodos, vuelve al principio.
	
	// obtengo puntos interpolados
	glm::vec3 p = spline.getControlPoint(ctrl_pt);
	glm::vec3 p_ant = spline.getControlPoint(ctrl_pt-3);
	glm::vec3 p_pos = spline.getControlPoint(ctrl_pt+3);
	
	// obtengo velocidad media en cada tramo
	glm::vec3 v_ant = (p - p_ant);
	glm::vec3 v_pos = (p_pos - p);

	// normalizo las velocidades medias
	glm::vec3 vi_menos = glm::normalize(v_ant);
	glm::vec3 vi_mas = glm::normalize(v_pos);
	
	// obtengo la dirección común
	glm::vec3 vi = (glm::length(v_ant)*vi_menos + glm::length(v_ant)*vi_mas)/glm::length(v_ant+v_pos);
	
	// obtengo puntos de control no interpolados
	p_ant = p - (glm::length(v_ant)*vi)/3.f;
	p_pos = p + (glm::length(v_pos)*vi)/3.f;
	
	// actualizo
	spline.setControlPoint(ctrl_pt+1,p_pos);
	spline.setControlPoint(ctrl_pt-1,p_ant);
	
	// FORMA DE LA CURVA:
	/* 	De acuerdo al método de Overhausser desarrollado,
		la curva a veces hace recorridos extra innecesarios
		para lograr la suavidad. Sin embargo, se ve suave
		y se puede formar un círculo sin que parezca polígono.
		Tiene continuidad geométrica.
		¿OVERSHOOTING?
	*/
	// VARIACIONES DE VELOCIDAD:
	/*  La curva se ve suave pero no se recorre suavemente,
		hay variaciones de velocidad entre puntos interpolados
		con diferentes distancias. Cuando los puntos están
		muy lejos, el pez va rápido. Cuando los puntos están
		muy cerca, va lento. No tiene continuidad paramétrica.
		Esto se debe a que, en cada tramo, la velocidad está
		dada por la distancia entre los puntos anterior y
		posterior. La velocidad de ctrl_pt-1 está dada por
		la distancia entre ctrl_pt-3 y ctrl_pt, mientras que
		la de ctrl_pt+1 por la de ctrl_pt+3 y ctrl_pt.
		Para evitar estas variaciones de velocidad, podría
		comparar las distancias al punto ctrl_pt, y quedarme
		con el punto calculado con la menor distancia para
		definir ambos.
	*/
}

// callbacks
void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);
void characterCallback(GLFWwindow* glfw_win, unsigned int code);

glm::mat4 getTransform(const Spline &spline, double t) {
	// obtengo punto y derivada en el t actual
	glm::vec3 deriv;
	glm::vec3 p = spline.at(t,deriv);
	
	// obtengo ángulo de rotación con el arcotangente de la derivada
	// atan2(z,x) me da el ángulo que forma (x,z) con el eje.
	float ang = atan2(deriv.z,deriv.x);
	
	// armo matriz de transformación del pez: 
	/* ROTACIÓN
	( cos(ang),  0.f, sin(ang),
	  0.f,	    1.f, 0.f,
	  -sin(ang), 0.f, cos(ang) ) */
	/* ESCALADO
	( 0.33, 0.f,  0.f,
	  0.f,  0.33, 0.f,
	  0.f,  0.f,  0.33 ) */
	// multiplico ambas para armar los 3 vectores base
	glm::vec3 e_x(0.33*cos(ang),0.f,0.33*sin(ang));
	glm::vec3 e_y(0.f,0.33,0.f);
	glm::vec3 e_z(-sin(ang)*0.33,0.f,cos(ang)*0.33);
	// posición del pez
	glm::vec3 pos(p.x,p.y,p.z);
	
	// armar la matriz
	glm::mat4 m(1.f);
	for(int k=0;k<3;++k) { 
		m[0][k] = e_x[k];
		m[1][k] = e_y[k];
		m[2][k] = e_z[k];
		m[3][k] = pos[k];
	}
	return m;
}

// cuando cambia la cant de tramos, regenerar la spline
void remapSpline(Spline &spline, int cant_pts) {
	if (cant_pts<3) return;
	if (static_cast<int>(spline.getPieces().size()) == cant_pts) return;
	std::vector<glm::vec3> vp;
	double dt = 1.0/cant_pts;
	for(int i=0;i<cant_pts;++i)
		vp.push_back(spline.at(i*dt));
	spline = Spline(vp);
	for(int i=0;i<spline.getControlPointsCount();i+=degree) 
		updateControlPointsAround(spline,i);
}

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	glfwSetFramebufferSizeCallback(window, common_callbacks::viewResizeCallback);
	glfwSetCursorPosCallback(window, mouseMoveCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCharCallback(window, characterCallback);
	
	// setup OpenGL state and load the model
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glClearColor(0.4f,0.4f,0.7f,1.f);
	Shader shader_fish("shaders/fish");
	Shader shader_phong("shaders/phong");
	auto fish = Model::load("fish",Model::fKeepGeometry|Model::fDynamic);
	auto axis = Model::load("axis",Model::fDontFit);
	BezierRenderer bezier_renderer(500);
	model_angle = .33; view_angle = .85;
	
	// main loop
	FrameTimer ftime;
	float t = 0.f, speed = .05f;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		remapSpline(spline,cant_pts);
		
		// draw models and curve
		float dt = ftime.newFrame();
		if (animate) {
			t += dt*speed; while (t>1.f) t-=1.f; 
		}
		if (show_fish) {
			shader_fish.use();
			shader_fish.setLight(glm::vec4{-2.f,-2.f,-4.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.15f);
			shader_fish.setUniform("t",t*20);
			glm::mat4 m = getTransform(spline, t);
			auto mats = common_callbacks::getMatrixes();
			for(Model &model : fish) {
				shader_fish.setMatrixes(mats[0]*m,mats[1],mats[2]);
				shader_fish.setBuffers(model.buffers);
				shader_fish.setMaterial(model.material);
				model.buffers.draw();
			}
		}
		
		if (show_spline or show_poly) {
			setMatrixes(bezier_renderer.getShader());
			for(const auto & curve : spline.getPieces()) {
				bezier_renderer.update(curve);
				glPointSize(1);
				if (show_spline) bezier_renderer.drawCurve();
				glPointSize(5);
				if (show_poly) bezier_renderer.drawPoly();
			}
		}
		
		if (show_axis) {
			shader_phong.use();
			shader_phong.setLight(glm::vec4{-2.f,-2.f,-4.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.15f);
			setMatrixes(shader_phong);
			glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
			for(const Model &model : axis) {
				shader_phong.setBuffers(model.buffers);
				shader_phong.setMaterial(model.material);
				model.buffers.draw();
			}
		}
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			ImGui::Checkbox("Pez (P)",&show_fish);
			ImGui::Checkbox("Spline (S)",&show_spline);
			ImGui::Checkbox("Pol. Ctrl. (C)",&show_poly);
			ImGui::Checkbox("Ejes (J)",&show_axis);
			ImGui::Checkbox("Animar (A)",&animate);
			ImGui::SliderFloat("Velocidad",&speed,0.005f,0.5f);
			ImGui::SliderFloat("T",&t,0.f,1.f);
			if (ImGui::InputInt("Cant. Pts.",&cant_pts,1,1))
				if (cant_pts<3) cant_pts=3;
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void characterCallback(GLFWwindow* glfw_win, unsigned int code) {
	switch (code) {
		case 'a': case 'A':animate = !animate; break;
		case 's': case 'S':show_spline = !show_spline; break;
		case 'p': case 'P':show_fish = !show_fish; break;
		case 'j': case 'J':show_axis = !show_axis; break;
		case 'c': case 'C':show_poly = !show_poly; break;
		case '+': ++cant_pts; break;
		case '-': --cant_pts; break;
	}
}

glm::vec3 viewportToPlane(double xpos, double ypos) {
	auto ms = common_callbacks::getMatrixes(); // { model, view, projection }
	auto inv_matrix = glm::inverse(ms[2]*ms[1]*ms[0]); // ndc->world
	auto pa = inv_matrix * glm::vec4{ float(xpos)/win_width*2.f-1.f,
		(1.f-float(ypos)/win_height)*2.f-1.f,
		0.f, 1.f }; // point on near
	auto pb = inv_matrix * glm::vec4{ float(xpos)/win_width*2.f-1.f,
		(1.f-float(ypos)/win_height)*2.f-1.f,
		1.f, 1.f }; // point on far
	float alpha = pa[1]/(pa[1]-pb[1]);
	auto p = pa*(1-alpha) + pb*alpha; // point on plane
	return {p[0]/p[3],0.f,p[2]/p[3]};
}

void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
	if (ctrl_pt==-1) common_callbacks::mouseMoveCallback(window,xpos,ypos);
	else {
		spline.setControlPoint(ctrl_pt,viewportToPlane(xpos,ypos));
		if (ctrl_pt%degree==0) {
			updateControlPointsAround(spline,ctrl_pt);
			updateControlPointsAround(spline,ctrl_pt+degree);
			updateControlPointsAround(spline,ctrl_pt-degree);
		}
	}
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (ImGui::GetIO().WantCaptureMouse) return;
	ctrl_pt = -1;
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		auto p = viewportToPlane(xpos,ypos);
		float dmin = .1f;
		for(int i=0;i<spline.getControlPointsCount(); ++i) {
			double aux = glm::distance(p,spline.getControlPoint(i));
			if (aux < dmin) { dmin = aux; ctrl_pt = i; }
		}
	}
	if (ctrl_pt==-1) common_callbacks::mouseButtonCallback(window,button,action,mods);
}

