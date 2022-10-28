#ifndef TRACK_HPP
#define TRACK_HPP
#include <string>

class Track {

	public:
	Track(const std::string &mapa, int model_width, int model_height);
	int Width() const { return model_w; }
	int Height() const { return model_h; }
	bool isAsphalt(float x, float y) const;
	bool isFinishLine(float x, float y) const;
	~Track();
	
private:
	// datos del modelo
	int model_w, model_h= 100;
	// datos del "mapa"
	int map_w, map_h, map_c;
	unsigned char *map_data;
	int getOffset(float x, float y) const;
};

#endif

