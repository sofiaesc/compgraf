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
#include "SubDivMesh.hpp"
#include "SubDivMeshRenderer.hpp"

#define VERSION 20221013

// models and settings
std::vector<std::string> models_names = { "cubo", "icosahedron", "plano", "suzanne", "star" };
int current_model = 0;
bool fill = true, nodes = true, wireframe = true, smooth = false, 
	 reload_mesh = true, mesh_modified = false;

// extraa callbacks
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

SubDivMesh mesh;
void subdivide(SubDivMesh &mesh);

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	view_fov = 60.f;
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.8f,0.8f,0.9f,1.f);
	Shader shader_flat("shaders/flat"),
	       shader_smooth("shaders/smooth"),
		   shader_wireframe("shaders/wireframe");
	SubDivMeshRenderer renderer;
	
	// main loop
	Material material;
	material.ka = material.kd = glm::vec3{.8f,.4f,.4f};
	material.ks = glm::vec3{.5f,.5f,.5f};
	material.shininess = 50.f;
	
	FrameTimer timer;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		if (reload_mesh) {
			mesh = SubDivMesh("models/"+models_names[current_model]+".dat");
			reload_mesh = false; mesh_modified = true;
		}
		if (mesh_modified) {
			renderer = makeRenderer(mesh);
			mesh_modified = false;
		}
		
		if (nodes) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawPoints(shader_wireframe);
		}
		
		if (wireframe) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawLines(shader_wireframe);
		}
		
		if (fill) {
			Shader &shader = smooth ? shader_smooth : shader_flat;
			shader.use();
			setMatrixes(shader);
			shader.setLight(glm::vec4{2.f,1.f,5.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.25f);
			shader.setMaterial(material);
			renderer.drawTriangles(shader);
		}
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			if (ImGui::Combo(".dat (O)", &current_model,models_names)) reload_mesh = true;
			ImGui::Checkbox("Fill (F)",&fill);
			ImGui::Checkbox("Wireframe (W)",&wireframe);
			ImGui::Checkbox("Nodes (N)",&nodes);
			ImGui::Checkbox("Smooth Shading (S)",&smooth);
			if (ImGui::Button("Subdivide (D)")) { subdivide(mesh); mesh_modified = true; }
			if (ImGui::Button("Reset (R)")) reload_mesh = true;
			ImGui::Text("Nodes: %i, Elements: %i",mesh.n.size(),mesh.e.size());
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'D': subdivide(mesh); mesh_modified = true; break;
		case 'F': fill = !fill; break;
		case 'N': nodes = !nodes; break;
		case 'W': wireframe = !wireframe; break;
		case 'S': smooth = !smooth; break;
		case 'R': reload_mesh=true; break;
		case 'O': case 'M': current_model = (current_model+1)%models_names.size(); reload_mesh = true; break;
		}
	}
}

// La struct Arista guarda los dos indices de nodos de una arista
// Siempre pone primero el menor indice, para facilitar la búsqueda en lista ordenada;
//    es para usar con el Mapa de más abajo, para asociar un nodo nuevo a una arista vieja
struct Arista {
	int n[2];
	Arista(int n1, int n2) {
		n[0]=n1; n[1]=n2;
		if (n[0]>n[1]) std::swap(n[0],n[1]);
	}
	Arista(Elemento &e, int i) { // i-esima arista de un elemento
		n[0]=e[i]; n[1]=e[i+1];
		if (n[0]>n[1]) std::swap(n[0],n[1]); // pierde el orden del elemento
	}
	const bool operator<(const Arista &a) const {
		return (n[0]<a.n[0]||(n[0]==a.n[0]&&n[1]<a.n[1]));
	}
};

// Mapa sirve para guardar una asociación entre una arista y un indice de nodo (que no es de la arista)
using Mapa = std::map<Arista,int>;

void subdivide(SubDivMesh &mesh) {
	
	Mapa mapita;
	int n_n = mesh.n.size(); // número de puntos originales
	int n_e = mesh.e.size(); // número de elementos originales

	// 1) Agregar los centroides:
	for(int i=0; i<n_e; i++)  { // por cada elemento
		Elemento e = mesh.e[i];
		glm::vec3 suma = glm::vec3(0.f,0.f,0.f);
		for(int j=0; j<e.nv; j++){
			Nodo n = mesh.n[e.n[j]]; 
			suma += n.p; // voy sumando los nodos
		}
		Nodo centroide = suma/float(e.nv);
		mesh.n.push_back(centroide);
	}
	
	// 2) Agregar los nodos asociados por cada arista
	for(int i=0; i<n_e; i++) { // por cada elemento
		Elemento e = mesh.e[i];
		glm::vec3 nuevo;
		
		for(int j=0;j<e.nv;j++){ // por cada nodo del elemento armo arista
			Arista arista(e[j],e[j+1]); 
			
			if (mapita.find(arista) == mapita.end()){
				if(e.v[j] != -1){ // NODOS INTERIORES: tiene un vecino v[j] con la arista j
					glm::vec3 centroide1 = mesh.n[n_n+i].p; // centroide del elemento actual
					glm::vec3 centroide2 = mesh.n[n_n+e.v[j]].p; // centroide del vecino
					nuevo = (mesh.n[e[j]].p+mesh.n[e[j+1]].p+centroide1+centroide2)/4.f; // promedio nodos arista y centroides
				} else { // NODOS FRONTERA: no tiene vecino v[j] en la arista j
					nuevo = (mesh.n[e[j]].p+mesh.n[e[j+1]].p)/2.f; // promedio nodos arista
				}
				
				mesh.n.push_back(nuevo);
				mapita[arista]=mesh.n.size()-1; 
			}
		}
	}
	
	// 3) Armar los elementos nuevos
	for(int i = 0; i < n_e; i++) { // por cada elemento original
		Elemento e = mesh.e[i];
		int centroide = n_n + i;
		
		for(int j=0; j<e.nv; j++){ // con cada nodo vecino armo elementos nuevos
			int nodo0 = e[j];
			int nodo1 = mapita[Arista(e[j],e[j+1])]; // mapita me devuelve indice de nodo
			int nodo2 = mapita[Arista(e[j],e[j-1])];
			
			// reemplazo por el primero, agrego los otros 2 o 3
			if(j==0) mesh.reemplazarElemento(i,centroide,nodo2,nodo0,nodo1);
			else mesh.agregarElemento(centroide,nodo2,nodo0,nodo1);
		}
	}
	mesh.makeVecinos(); // actualizo vecinos
	
	//  4) Calcular las nuevas posiciones de los nodos originales
	// Recorremos las aristas y, a los dos nodos de cada arista, les acumulamos su r en otro
	// mapa auxiliar en los casos pertinentes (cuando el nodo no es frontera o cuando sí es 
	// frontera pero el otro nodo de la arista también).
	std::map<int,glm::vec3> rmap;
	for(auto it=mapita.begin();it!=mapita.end();it++) { 
		if((mesh.n[(it->first).n[0]].es_frontera == mesh.n[(it->first).n[1]].es_frontera)){
			rmap[(it->first).n[1]]+= mesh.n[it->second].p;
			rmap[(it->first).n[0]]+= mesh.n[it->second].p;
		} else {
			if(mesh.n[(it->first).n[0]].es_frontera) rmap[(it->first).n[1]]+= mesh.n[it->second].p;
			else rmap[(it->first).n[0]]+= mesh.n[it->second].p;
		}
	}
	
	for(int i=0;i<n_n;i++) { 
		Nodo nodoActual= mesh.n[i];//nodo actual	
		auto p= nodoActual.p;
		glm::vec3 nuevaPos= glm::vec3(0.f,0.f,0.f);
		glm::vec3 r= rmap[i];
		
		if(nodoActual.es_frontera){
			r = r/2.f; 
			nuevaPos= (r+p)/2.f;
		} else {
			r = r/float(nodoActual.e.size());
			glm::vec3 f= glm::vec3(0.f,0.f,0.f);
			
			for(int j=0;j<nodoActual.e.size();j++) { 
				Elemento e = mesh.e[nodoActual.e[j]];
				f+=mesh.n[e[0]].p; // habíamos puesto los centroides en el [0] en el paso 3
			}
			auto n= float(nodoActual.e.size());	// n = numero de centroides = numero de elementos
			f=f/n;
			nuevaPos= (4.f*r-f+(n-3.f)*p)/n;
		}
		mesh.n[i].p=nuevaPos;
	}
}
