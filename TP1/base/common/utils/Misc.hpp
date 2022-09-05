#ifndef MISC_HPP
#define MISC_HPP
#include <string>
#include <utility>
#include <vector>
#include <glm/glm.hpp>

std::string extractFolder(const std::string &filename);

void fixEOL(std::string &s);

bool startsWith(const std::string str, const char *con);

std::pair<glm::vec3,glm::vec3> getBoundingBox(const std::vector<glm::vec3> &v);

#endif

