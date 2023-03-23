#include "euclidian.h"
#include <numeric>
#include <cmath>
#include <iostream>

using namespace std;

float innerProduct(vector<float> v1, vector<float> v2) {
    return inner_product(v1.begin(), v1.end(), v2.begin(), 0.0f);
}

vector<float> crossProduct(vector<float> v1, vector<float> v2) {
    float p0 = v1[1] * v2[2] - v1[2] * v2[1];
    float p1 = -(v1[0] * v2[2] - v1[2] * v2[0]);
    float p2 = v1[0] * v2[1] - v1[1] * v2[0];
    return vector<float>{p0, p1, p2};
}

float innerAngle(vector<float> v1, vector<float> v2) {
    float inner = innerProduct(v1, v2);
    float len_sq_v1 = lenSquared(v1);
    float len_sq_v2 = lenSquared(v2);

    if (len_sq_v1 == 0 || len_sq_v2 == 0) {
        return 0.0f;
    }

    return acos(inner / sqrt(len_sq_v1 * len_sq_v2));
}

float lenSquared(vector<float> v) {
    float len_sq = 0.0f;
    for (float e : v) {
        len_sq += e * e;
    }
    return len_sq;
}

float length(vector<float> v) {
    return sqrt(lenSquared(v));
}

float rad2dgr(float radian) {
    float pi = 3.14159;
    return radian * (180 / pi);
}

// font: https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line
float distPoint2Line(vector<float> p, vector<float> line_point, vector<float> line_vector) {
    vector<float> ap{p[0] - line_point[0], p[1] - line_point[1], p[2] - line_point[2]};
    float dist = sqrt(lenSquared(crossProduct(ap, line_vector)) / lenSquared(line_vector));
    return dist;
}