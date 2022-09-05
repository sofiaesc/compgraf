#include <unordered_set>
#include "Delaunay.hpp"
#include "Debug.hpp"

//static void verificarIntegridad(const Delaunay &d) {
//	for(size_t i_tri=0;i_tri<d.getTriangulos().size();++i_tri) { 
//		const Triangulo &t = d.getTriangulo(i_tri);
//		for(int k=0;k<3;++k) {
//			cg_assert(t[k]>=0 && t[k]<d.getPuntos().size(),"indice de triangulo no valido");
//			if (t.vecinos[k]==-1) continue;
//			cg_assert(t.vecinos[k]>=0 && t.vecinos[k]<d.getTriangulos().size(),"indice de vecino no valido");
//			cg_assert(d.getTriangulo(t.vecinos[k]).indiceVecino(i_tri)!=-1,"la vecindad no es reciproca");
//		}
//	}
//}

Delaunay::Delaunay(glm::vec3 punto1, glm::vec3 punto2, float tol)
	: error_tol(tol), boundingBox(punto1, punto2)
{
	// registrar esos cuatro puntos
	puntos.push_back({boundingBox.pmax.x,boundingBox.pmax.y,0.f});
	puntos.push_back({boundingBox.pmin.x,boundingBox.pmax.y,0.f});
	puntos.push_back({boundingBox.pmax.x,boundingBox.pmin.y,0.f});
	puntos.push_back({boundingBox.pmin.x,boundingBox.pmin.y,0.f});
	
	// agregar los triangulos a la lista
	triangulos.push_back({{1,3,2}});
	triangulos.push_back({{2,0,1}});
	triangulos[0].vecinos[1] = 1;
	triangulos[1].vecinos[1] = 0;
}

void Delaunay::intercambiarDiagonales(int i_tri1, int i_tri2) {
	
	Triangulo &tri1 = triangulos[i_tri1];
	Triangulo &tri2 = triangulos[i_tri2];
	
	// averigua en que punto (0, 1, o 2) estan como vecinos
	int indice1 = tri1.indiceVecino(i_tri2);
	int indice2 = tri2.indiceVecino(i_tri1);
	
	// cambiar los puntos de lugar para armar la nueva diagonal
	tri1.vertices[(indice1+2)%3] = tri2.vertices[indice2];
	tri2.vertices[(indice2+2)%3] = tri1.vertices[indice1];
	
	// reacomodar los vecinos de los triangulos 1 y 2
	int vecino1 = tri1.vecinos[(indice1+1)%3];
	int vecino2 = tri2.vecinos[(indice2+1)%3];
	tri1.vecinos[indice1] = vecino2;
	tri1.vecinos[(indice1+1)%3] = i_tri2;
	tri2.vecinos[indice2] = vecino1;
	tri2.vecinos[(indice2+1)%3] = i_tri1;
	
	// reacomodar los vecinos de los vecinos de los triangulos 1 y 2
	if (vecino1!=-1) triangulos[vecino1].reemplazarVecino(i_tri1,i_tri2);
	if (vecino2!=-1) triangulos[vecino2].reemplazarVecino(i_tri2,i_tri1);
	
}


int Delaunay::agregarPunto(glm::vec3 punto) {
	if (!boundingBox.contiene(punto)) return -1;
	int indice = puntos.size();
	puntos.push_back(punto);
	conectarPunto(indice);
	return indice;
}
	
int Delaunay::conectarPunto(int i_pto) {
	// buscar que triangulo dividir
	int i_triangulote = enQueTriangulo(puntos[i_pto]); 
	Triangulo triangulote = triangulos[i_triangulote];
	
	// crear los tres triangulitos que reemplazaran a triangulote
	// (al 3ro ponerlo directamente donde estaba triangulote)
	int i_triangulito1 = triangulos.size();
	triangulos.push_back({{i_pto,triangulote[2],triangulote[0]}});
	int i_triangulito2 = triangulos.size();
	triangulos.push_back({{i_pto,triangulote[1],triangulote[2]}});
	int i_triangulito3 = i_triangulote;
	triangulos[i_triangulote] = {{i_pto,triangulote[0],triangulote[1]}};
	Triangulo &triangulito1 = triangulos[i_triangulito1];
	Triangulo &triangulito2 = triangulos[i_triangulito2];
	Triangulo &triangulito3 = triangulos[i_triangulito3];
	
	// acomodar los vecinos de los nuevos triangulitos
	triangulito1.vecinos[0]=triangulote.vecinos[1];
	triangulito1.vecinos[1]=i_triangulito3;
	triangulito1.vecinos[2]=i_triangulito2;
	triangulito2.vecinos[0]=triangulote.vecinos[0];
	triangulito2.vecinos[1]=i_triangulito1;
	triangulito2.vecinos[2]=i_triangulito3;
	triangulito3.vecinos[0]=triangulote.vecinos[2];
	triangulito3.vecinos[1]=i_triangulito2;
	triangulito3.vecinos[2]=i_triangulito1;
	
	// acomodar los vecinos de los vecinos de triangulote
	auto reemplazar_vecino = [this](int itv, int ifrom, int ito) {
		if (itv!=-1) triangulos[itv].reemplazarVecino(ifrom,ito);
	};
	reemplazar_vecino(triangulote.vecinos[0],i_triangulote,i_triangulito2);
	reemplazar_vecino(triangulote.vecinos[1],i_triangulote,i_triangulito1);
	reemplazar_vecino(triangulote.vecinos[2],i_triangulote,i_triangulito3);
	
	// retriangular correctamente
	recuperarDelaunay({i_triangulito1,i_triangulito2,i_triangulito3});
	
	return puntos.size()-1;
}

void Delaunay::moverPunto(int indice, glm::vec3 destino){
	cg_assert(indice>=0 and indice<puntos.size(),"indice de punto no valido");
	if (!boundingBox.contiene(destino)) return;
	desconectarPunto(indice);
	puntos[indice] = destino;
	conectarPunto(indice);
}

// quita un punto de la triangulacion y repone Delaunay
void Delaunay::eliminarPunto(int indice) {
	cg_assert(indice>=0 and indice<puntos.size(),"indice de punto no valido");
	desconectarPunto(indice);
	// quitar de la lista el pto y los dos triangulos que ya no sirven
	std::swap(puntos[indice],puntos.back());
	puntos.pop_back();
	int iback = puntos.size();
	if (iback!=indice) {
		for(Triangulo &t : triangulos) {
			for(int k=0;k<3;++k) { 
				if (t[k]==iback) 
					t[k] = indice;
			}
		}
	}
}

void Delaunay::desconectarPunto(int indice_del) {
	
	// armar la lista de triangulos que contienen al punto
	std::vector<int> lista, para_revisar;
	for (int i_tri=0;i_tri<triangulos.size();i_tri++)
		if (triangulos[i_tri].indiceVertice(indice_del)!=-1)
			lista.push_back(i_tri);
	
	// borrar triangulos hasta que queden tres
	while (lista.size()>3) {
		// recuperar un triangulo y buscar un vecino que comparta el punto
		int i_tri1 = lista.back();
		Triangulo &tri1 = triangulos[i_tri1];
		int aux = tri1.indiceVertice(indice_del);
		int i_tri2 = tri1.vecinos[(aux+1)%3];
		Triangulo &tri2 = triangulos[i_tri2];
		
		// encontrar los puntos para las diagonales
		int indice1 = tri1.indiceVecino(i_tri2);
		int indice2 = tri2.indiceVecino(i_tri1);
		// si se intersecan intercambiar diagonales y marcar para revisar
		if (seIntersecan(tri1[(indice1+1)%3], tri1[(indice1+2)%3],
						 tri1[indice1],       tri2[indice2]      )) 
		{
			intercambiarDiagonales(i_tri1,i_tri2);
			// borrar de la lista el triangulito que ya no contiene al punto
			if (tri1.indiceVertice(indice_del)!=-1) {
				para_revisar.push_back(i_tri2);
				lista.erase(find(lista.begin(),lista.end(),i_tri2));
			} else {
				para_revisar.push_back(i_tri1);
				lista.pop_back();
			}
		} else {
			// si no se intercambio la diagonal, intercambiar los triangulos para no hacer otra vez la misma comparacion
			std::swap(lista.back(),*find(lista.begin(),lista.end(),i_tri2));
		}
	}
	
	// estirar uno de los triangulos(triangulote) y borrar los otros dos (triangulitos 1 y 2)
	int i_triangulote = lista[0], i_triangulito1=lista[1], i_triangulito2=lista[2];
	Triangulo &triangulote = triangulos[i_triangulote];
	Triangulo &triangulito1 = triangulos[i_triangulito1];
	Triangulo &triangulito2 = triangulos[i_triangulito2];
	
	// obtener el punto clickeado en cada triangulo
	int indice0 = triangulote.indiceVertice(indice_del);
	int indice1 = triangulito1.indiceVertice(indice_del);
	int indice2 = triangulito2.indiceVertice(indice_del);
	
	// modificar el punto del triangulote
	triangulote[indice0] = triangulito1[triangulito1.indiceVecino(i_triangulote)];
	
	// arreglar los vecinos de triangulote
	triangulote.vecinos[triangulote.indiceVecino(i_triangulito1)] = triangulito1.vecinos[indice1];
	triangulote.vecinos[triangulote.indiceVecino(i_triangulito2)] = triangulito2.vecinos[indice2];
	
	// arreglar los vecinos de los vecinos triangulote
	int i_vecino1 = triangulito1.vecinos[indice1];
	if (i_vecino1!=-1) {
		Triangulo &vecino1 = triangulos[i_vecino1];
		vecino1.vecinos[vecino1.indiceVecino(i_triangulito1)] = i_triangulote;
	}
	int i_vecino2 = triangulito2.vecinos[indice2];
	if (i_vecino2!=-1) {
		Triangulo &vecino2 = triangulos[i_vecino2];
		vecino2.vecinos[vecino2.indiceVecino(i_triangulito2)] = i_triangulote;
	}
	
	// mandar a revisar la triangulacion nueva
	para_revisar.push_back(i_triangulote);
	recuperarDelaunay(para_revisar);
	
	for(int i=0;i<2;++i) { 
		int i_tri = i==(i_triangulito1<i_triangulito2)?i_triangulito1:i_triangulito2;
		std::swap(triangulos[i_tri],triangulos.back());
		triangulos.pop_back();
		int itri_back = triangulos.size();
		if (i_tri==itri_back) continue;
		for(Triangulo &t : triangulos) {
			for(int k=0;k<3;++k) {
				if (t.vecinos[k]==itri_back) 
					t.vecinos[k] = i_tri;
			}
		}
	}	
}

Pesos Delaunay::calcularPesos(int i_triangulo, glm::vec3 p) const {
	const auto &t = triangulos[i_triangulo];
	return ::calcularPesos(puntos[t[0]],puntos[t[1]],puntos[t[2]],p);
}

int Delaunay::enQueTriangulo(glm::vec3 &punto) const {
	int i_tri = rand()%triangulos.size(); // elige uno de los triangulos al azar para empezar a buscar.
	while (i_tri!=-1) { // si hago click fuera del cuadrado, i_tri = -1.
		Pesos ff = calcularPesos(i_tri,punto); // calcula los pesos de los vertices de uno de los vecinos.
		int imin;
		/* Busca el índice del triángulo vecino más cercano al punto,
		   tal que el vértice con menor peso va a ser el índice del 
		   vecino (los índices de los vecinos se corresponden con el
		   vértice opuesto a ellos) */
		if (ff[0] < ff[2]) {
			if (ff[0] < ff[1]) imin=0; // imin va a ser el vértice con menor peso.
			else               imin=1;  
		} else {
			if (ff[2] < ff[1]) imin=2; 
			else               imin=1; 
		}
		if (ff[imin]>=0) break; // si el punto está dentro del triángulo, corta la iteración. 
		i_tri = triangulos[i_tri].vecinos[imin]; // busca el vecino del triángulo actual con el índice encontrado.
	}
	return i_tri; // si no tiene vecinos o si no hay triángulos, devuelve -1.
}

// devuelve verdadero si el punto esta contenido (estrictamente) en la circunferencia formada por los tres vertices
bool Delaunay::circunferenciaContiene(const Triangulo &t, glm::vec3 p) const {
	
	// traer el triangulo al origen (vertice0 en 0,0)
	glm::vec3 punto1 = puntos[t[1]]-puntos[t[0]];
	glm::vec3 punto2 = puntos[t[2]]-puntos[t[0]];
	// desplazar igualmente el punto a averiguar si esta contenido
	glm::vec3 puntoDesp = p-puntos[t[0]];
	
	// calcular el doble del area con el producto vectorial
	float area4 = 2*(glm::cross(punto1,punto2).z);
	if (area4==0) return false;
	
	float mod2Punto1 = glm::dot(punto1,punto1),
		  mod2Punto2 = glm::dot(punto2,punto2);
	
	// calcula el punto del centro (en la version desplazada)
	glm::vec3 centro( (mod2Punto1*punto2.y-mod2Punto2*punto1.y)/area4,
				      (punto1.x*mod2Punto2-punto2.x*mod2Punto1)/area4, 0.f );
	
	// compara la distancia con el radio
	glm::vec3 vdist = puntoDesp-centro;
	return glm::dot(centro,centro) - glm::dot(vdist,vdist) > error_tol;
}

void Delaunay::recuperarDelaunay(std::vector<int> tris_a_revisar){
	
	std::unordered_set<int> tris_ya_revisados;
	// mientras haya triangulos por revisar
	while (not tris_a_revisar.empty()){
		
		// sacar el ultimo de el contenedor (simil pop())
		int i_tri = tris_a_revisar.back(); tris_a_revisar.pop_back();
		// si ya fue revisado porque estaba dos veces en la lista, saltearlo
		if (!tris_ya_revisados.insert(i_tri).second) continue;
		
		// comprobar si no cumple la condicion para cada vecino
		for(int k=0;k<3;++k) {
			// obtener el vecino del triangulo a revisar
			int i_vec = triangulos[i_tri].vecinos[k];
			if (i_vec==-1) continue; // si no tiene vecino (triangulo del borde), no hacer nada
			
			// si no cumple la condicion de Delaunay
			if (circunferenciaContiene(triangulos[i_vec],puntos[triangulos[i_tri][k]])) {
				// intercambiar diagonales
				intercambiarDiagonales(i_tri,i_vec);
				// poner a revisar otra vez a ambos
				tris_a_revisar.push_back(i_tri);
				tris_a_revisar.push_back(i_vec);
				tris_ya_revisados.erase(i_tri);
				tris_ya_revisados.erase(i_vec);
				break;
			}
		}
	}
}

bool Delaunay::seIntersecan(int ipunto11, int ipunto12, int ipunto21, int ipunto22) {
	// definir lineas para intersecar
	float x11 = puntos[ipunto11].x, y11 = puntos[ipunto11].y,
	      x12 = puntos[ipunto12].x, y12 = puntos[ipunto12].y,
	      x21 = puntos[ipunto21].x, y21 = puntos[ipunto21].y,
	      x22 = puntos[ipunto22].x, y22 = puntos[ipunto22].y;
	// ver si se intersecan
	float a1 = x12-x11, b1 = x21-x22, c1 = x21-x11,
		  a2 = y12-y11, b2 = y21-y22, c2 = y21-y11;
	float w = a1*b2-a2*b1;
	if (w==0.f) return false;
	float t1 = (c1*b2-c2*b1)/w;
	float t2 = (a1*c2-a2*c1)/w;
	// ¿se intersecan?
	return (t1>0 && t1<1 && t2>0 && t2<1);
}
//
//bool Delaunay::estaEnElBoundingBox(glm::vec3 &punto){
//  return boundingBox.contiene(punto);
//}
