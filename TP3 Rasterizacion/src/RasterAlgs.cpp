#include <forward_list>
#include <iostream>
#include <GL/gl.h>
#include <cmath>
#include "RasterAlgs.hpp"
#include <algorithm>
using namespace std;

void drawSegment(paintPixelFunction paintPixel, glm::vec2 p0, glm::vec2 p1) {
	// ALGORITMO DDA-SEGMENTOS: La componente dada por la tendencia de la recta aumenta en 1, 
	// mientras que la otra componente aumenta según la pendiente redondeada a número entero.

	float dx=p1.x-p0.x, dy=p1.y-p0.y; // obtengo las diferencias en x e y.
	
	// analizo los valores absolutos de las componentes de la pendiente para ver la tendencia de la recta 
	if (fabs(dx)>=fabs(dy)){ // tendencia horizontal
		if(dx<0){ 
			swap(p0,p1); // si la componente x de la pendiente es menor que 0, swappear
			dx=p1.x-p0.x; dy=p1.y-p0.y; // actualizo las diferencias en x e y
		}
		float x=round(p0.x), y=round(p0.y), m=dy/dx;
		paintPixel(glm::vec2(x,y));
		while((++x)<round(p1.x)){ y+=m; paintPixel(glm::vec2{x,round(y)});
		}
	} else { // tendencia vertical
		if(dy<0){
			swap(p0,p1); // si la componente y de la pendiente es menor que 0, swappear
			dx=p1.x-p0.x; dy=p1.y-p0.y; // actualizo las diferencias en x e y
		}
		float x=round(p0.x), y=round(p0.y), m=dx/dy;
		paintPixel(glm::vec2{x,y});
		while((++y)<round(p1.y)) { x+=m; paintPixel(glm::vec2{round(x),y});
		}
	}
}

void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve) {
	// ALGORITMO DDA-CURVA: Utilizo una variable aparte, en vez de x e y,
	// t = [t0,t1] con t0 = 0, t1 = 1. Aumento viene de serie de Taylor.
	
	float t = 0.f; // t = 0
	float dt = 0.f;
	
	while(t < 1.f){ // t < t1
		auto r = evalCurve(t);
		auto current_p = r.p; // punto actual
		auto current_d = r.d; // vector derivada actual
		
		// calculo mi dt
		if(fabs(current_d.x)<fabs(current_d.y)) dt = 1/fabs(current_d.y);
		else dt = 1/fabs(current_d.x);
		
		// obtengo el próximo punto
		auto next_p = evalCurve(t+dt).p;
		auto diff=round(next_p)-round(current_p);
		
		// si el próximo punto está muy lejos, decremento el dt en 0.5 hasta encontrar un punto limítrofe al actual
		while(fabs(diff.x)>1.f || fabs(diff.y)>1.f){
			dt = dt*0.5f;
			next_p = evalCurve(t+dt).p;
			diff=round(next_p) - round(current_p);
		}
		
		t+=dt;
		
		// verifico que no esté pintando en el mismo punto
		if(round(current_p) != round(next_p)) 
			paintPixel(round(current_p));
	}
}






