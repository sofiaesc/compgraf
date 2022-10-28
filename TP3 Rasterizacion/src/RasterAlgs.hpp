#ifndef RASTER_ARGS_HPP
#define RASTER_ARGS_HPP

#include <glm/glm.hpp>

using paintPixelFunction = void(*)(glm::vec2);

// rasterizar segmento entre dos puntos
void drawSegment(paintPixelFunction paintPixel, glm::vec2 p0, glm::vec2 p1);

struct curveRetVal { glm::vec2 p; glm::vec2 d; };
using curveEvalFunction = curveRetVal(*)(float);

// rasterizar curva
void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve);

#endif

