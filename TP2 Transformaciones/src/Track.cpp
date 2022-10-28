#include <stb_image.h>
#include <algorithm>
#include "Track.hpp"

Track::Track(const std::string &mapa, int model_width, int model_height) 
	: model_w(model_width), model_h(model_height)
{
	map_data = stbi_load("models/mapa.png", &map_w, &map_h, &map_c, 0);
}

int Track::getOffset (float x, float y) const {
	int j = std::min(int((model_w-x)/(2*model_w)*map_w),map_w-1);
	int i = std::min(int((model_h-y)/(2*model_h)*map_h),map_h-1);
	return (i*map_w+j)*map_c;
}

bool Track::isAsphalt (float x, float y) const {
	return map_data[getOffset(x,y)]!=0;
}

bool Track::isFinishLine (float x, float y) const {
	int o = getOffset(x,y);
	return map_data[o]!=map_data[o+1];
}

Track::~Track ( ) {
	stbi_image_free(map_data);
}

