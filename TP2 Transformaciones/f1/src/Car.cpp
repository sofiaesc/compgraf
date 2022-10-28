#include <cmath>
#include "Car.hpp"

constexpr float PI = 3.14159265359f;
constexpr float G2R = PI/180.f;

void Car::Move(const Track &track, float acel, float dir, bool analog) {
	// frenar si se sale de la pista
	if ((not track.isAsphalt(x,y)) && vel>top_speed/4) {
		acel = -1; dir /= 2;
	}
	// aplicar los controles
	double target_dir = dir*0.006*float(10*vel+70*(top_speed-vel))/top_speed;
	rang1 = analog ? target_dir : ((8*rang1+target_dir)/9);
	if (!acel) acel = -.03;
	// mover el auto
	vel += acel*.60-.10;
	if (vel<0) vel=0;
	else if (vel>top_speed) vel=top_speed;
	x += vel*std::cos(ang)/100;
	y += vel*std::sin(ang)/100;
	// la pista es ciclica
	if (x<-track.Width()) x += track.Width()*2;
	else if (x>track.Width()) x -= track.Width()*2;
	if (y<-track.Height()) y += track.Height()*2;
	else if (y>track.Height()) y -= track.Height()*2;
	ang += rang1*vel/150;
	rang2 += vel/10;
}


