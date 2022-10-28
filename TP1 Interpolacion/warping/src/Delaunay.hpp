#ifndef DELAUNAY_HPP
#define DELAUNAY_HPP

#include <algorithm>
#include <glm/glm.hpp>
#include <vector>
#include "utils.hpp"

struct Triangulo {
	int vertices[3];
	int vecinos[3] = {-1,-1,-1};
	int operator[](int i) const { return vertices[i]; }
	int &operator[](int i) { return vertices[i]; }
	int indiceVertice(int i) const {
		int k=2;
		while (k>=0 && vertices[k]!=i)
			--k;
		return k;
	}
	int indiceVecino(int v) const { 
		int k=2; 
		while(k>=0 and vecinos[k]!=v) 
			--k;
		return k;
	}
	void reemplazarVecino(int iv1, int iv2) {
		int k = indiceVecino(iv1);
		if (k!=-1) vecinos[k] = iv2;
	}
};


class Delaunay {
public:
	
	// define los limites de la triangulacion
	Delaunay(glm::vec3 punto1, glm::vec3 punto2, float tol=0.1);
	
	// agrega un punto a la triangulacion y devuelve el indice
	int agregarPunto(glm::vec3 punto);

	// mueve un punto en la triangulacion y devuelve el indice
	void moverPunto(int indice, glm::vec3 destino);
	
	const BoundingBox &getBoundingBox() const { return boundingBox; }
	
	// elimina un punto de la triangulacion
	void eliminarPunto(int indice);
	
	// funciones para obtener los datos de un punto, un triangulo, o las listas completas
	const std::vector<glm::vec3> &getPuntos() const { return puntos; }
	const std::vector<Triangulo> &getTriangulos() const { return triangulos; }
	
	// devuelve el indice del triangulo que contiene al punto
	int enQueTriangulo(glm::vec3 &p) const;
	
private:
	
	float error_tol;
	BoundingBox boundingBox;
	std::vector<glm::vec3> puntos;
	std::vector<Triangulo> triangulos;
	
	// desconecta un punto de la triangulacion pero sin sacar del vector de puntos
	void desconectarPunto(int indice);
	
	// conencta un punto del vector de puntos (que no deberia estar asociado a 
	// ningun triangulo) a la triangulacion
	int conectarPunto(int indice);
	
	// wrapper para la func calcularPesos global 
	Pesos calcularPesos(int i_triangulo, glm::vec3 p) const;
	
	// revisa que los triangulos marcados sean correcto segun la condicion de Delaunay
	// y corrige si no lo son
	void recuperarDelaunay(std::vector<int> tris_a_revisar);
	
	// intercambia las diagonales de dos triangulos y reacomoda sus atributos
	void intercambiarDiagonales(int itri1, int itri2);
	
	// verifica si se intersecan los dos segmentos formados por estos cuatro puntos
	bool seIntersecan(int ipunto11, int ipunto12, int ipunto21, int ipunto22) ;
	
	// verifica si la circunferencia de un triangulo contiene a un pto de otro
	bool circunferenciaContiene(const Triangulo &t, glm::vec3 p) const;
	
};

#endif
