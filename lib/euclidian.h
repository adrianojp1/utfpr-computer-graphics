#include <vector>
#include <glm/glm.hpp>

float innerProduct(std::vector<float> v1, std::vector<float> v2);

std::vector<float> crossProduct(std::vector<float> v1, std::vector<float> v2);

float innerAngle(std::vector<float> v1, std::vector<float> v2);

float lenSquared(std::vector<float> v);

float length(std::vector<float> v) ;

float rad2dgr(float angle);

float distPoint2Line(std::vector<float> p, std::vector<float> line_point, std::vector<float> line_vector);

std::vector<glm::vec2> to2DVector(float* in, int size);
void to3DPointer(std::vector<glm::vec2> v, float* out, int* size, float z);

std::vector<glm::vec2> suthHodgClip(std::vector<glm::vec2> poly, std::vector<glm::vec2> clipper);
