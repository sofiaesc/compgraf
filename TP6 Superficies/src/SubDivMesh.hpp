#ifndef SUBDIVMESH_HPP
#define SUBDIVMESH_HPP

#include <algorithm>
#include <map>
#include <vector>
#include <cmath>
#include <memory>
#include <glm/glm.hpp>

// Nodo o vértice: punto mas datos para usar en una malla
struct Nodo {
	glm::vec3 p;
	bool es_frontera = false;
	std::vector<int> e; // en que elementos esta este Punto
	Nodo(const glm::vec3 &pos) : p(pos) { }
};

// El Elemento guarda los indice de los 3/4 nodos de un triangulo/cuadrilatero 
//  y los indices de los 3/4 elementos vecinos
// El orden es importante pues indica la orientacion
// Notar que no conoce las posiciones, solo indices en listas de una malla que tampoco conoce
struct Elemento {
	int nv=0; // cantidad de vertices 3 o 4, me dice que tipo de elemento es
	int n[4]; // los tres/cuatro nodos
	int v[4]; // los tres/cuatro elementos vecinos por arista (-1 indica arista de frontera)
	Elemento(int n0=0, int n1=0, int n2=0, int n3=-1) {
		n[0]=n0; n[1]=n1; n[2]=n2; n[3]=n3; nv=n3<0?3:4;
	}
	void SetNodos(int n0=0, int n1=0, int n2=0, int n3=-1) {
		n[0]=n0; n[1]=n1; n[2]=n2; n[3]=n3; nv=n3<0?3:4;
	}
	inline int &operator[](int i) { 
		return n[(i+nv)%nv]; // para poder ciclar (anterior, posterior, diagonal)
	}
	const inline int operator[](int i) const { 
		return n[(i+nv)%nv]; // para poder ciclar (anterior, posterior, diagonal)
	}
	bool Tiene(int i) { // pregunta si el Elemento tiene al nodo i
		return n[0]==i||n[1]==i||n[2]==i||(nv==4&&n[3]==i);
	}
	int Indice(int i) { // devuelve en que posicion el elemento tiene al nodo i (o -1, si no)
		int j=nv-1; while (j>=0 && n[j]!=i) j--; return j;
	}
};

// Malla guarda principalmente una lista de nodos y elementos
struct SubDivMesh {
	std::vector<Nodo> n;
	std::vector<Elemento> e;
	
	SubDivMesh() = default;
	SubDivMesh(const std::string &fname);
	void makeVecinos();
	void agregarElemento(int n0, int n1, int n2, int n3=-1);
	void reemplazarElemento(int ie, int n0, int n1, int n2, int n3=-1);
};

#endif

