#include "euclidian.h"
#include <numeric>
#include <cmath>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

float innerProduct(vector<float> v1, vector<float> v2) {
    return inner_product(v1.begin(), v1.end(), v2.begin(), 0.0f);
}

vector<float> crossProduct(vector<float> v1, vector<float> v2) {
    float p0 = v1[1] * v2[2] - v1[2] * v2[1];
    float p1 = -(v1[0] * v2[2] - v1[2] * v2[0]);
    float p2 = v1[0] * v2[1] - v1[1] * v2[0];
    return vector<float>{ p0, p1, p2 };
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
    vector<float> ap{ p[0] - line_point[0], p[1] - line_point[1], p[2] - line_point[2] };
    float dist = sqrt(lenSquared(crossProduct(ap, line_vector)) / lenSquared(line_vector));
    return dist;
}

vector<vec2> to2DVector(float* in, int size) {
    vector<vec2> v;
    for (int i = 0; i < size; i++) {
        int in_offset = i * 3;
        int out_offset = i * 2;
        v.push_back(vec2(in[in_offset], in[in_offset + 1]));
    }
    return v;
}

void to3DPointer(vector<vec2> v, float* out, int* size, float z) {
    for (int i = 0; i < v.size(); i++) {
        int offset = i * 3;
        out[offset] = v[i].x;
        out[offset + 1] = v[i].y;
        out[offset + 2] = z;
    }
    *size = v.size();
}

vec2 intersect(vec2 p1, vec2 p2, vec2 p3, vec2 p4) {
    float num_x = (p1.x * p2.y - p1.y * p2.x) * (p3.x - p4.x) - (p1.x - p2.x) * (p3.x * p4.y - p3.y * p4.x);
    float num_y = (p1.x * p2.y - p1.y * p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x * p4.y - p3.y * p4.x);
    float den = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);
    return vec2(num_x / den, num_y / den);
}

float paralelogram_area(vec2 p1, vec2 p2, vec2 p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool leftOn(vec2 p1, vec2 p2, vec2 p3) {
    return paralelogram_area(p1, p2, p3) >= 0.0f;
}

void clip(vector<vec2>* poly, vec2 e_p1, vec2 e_p2) {
    vector<vec2> clipped;

    for (int i = 0; i < poly->size(); i++) {
        int k = (i + 1) % poly->size();
        cout << "poly i: " << i << ", k: " << k << endl;

        vec2 p1 = poly->at(i);
        vec2 p2 = poly->at(k);

        bool p1_in = leftOn(e_p1, e_p2, p1);
        bool p2_in = leftOn(e_p1, e_p2, p2);

        if (p1_in) {
            clipped.push_back(p1);
            cout << "added p1: " << to_string(p1) << endl;

            if (!p2_in) {
                vec2 intersection = intersect(p1, p2, e_p1, e_p2);
                clipped.push_back(intersection);
                cout << "added intersection: " << to_string(intersection) << endl;
            }
        } else {
            if (p2_in) {
                vec2 intersection = intersect(p1, p2, e_p1, e_p2);
                clipped.push_back(intersection);
                cout << "added intersection: " << to_string(intersection) << endl;
            }
        }
    }
    *poly = clipped;
}

vector<vec2> suthHodgClip(vector<vec2> poly, vector<vec2> clipper) {
    vector<vec2> clipped(poly.begin(), poly.end());

    for (int i = 0; i < clipper.size(); i++) {
        int k = (i + 1) % clipper.size();
        cout << "clipper i: " << i << ", k: " << k << endl;
        vec2 edge_p1 = clipper[i];
        vec2 edge_p2 = clipper[k];

        clip(&clipped, edge_p1, edge_p2);
    }

    return clipped;
}

// https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
vector<ivec2> midPointCircleDraw(ivec2 c, int r) {
    vector<ivec2> points;
    int x = r, y = 0;

    // Adding the initial point on the axes after translation
    points.push_back(ivec2(c.x + r, c.y));

    // When radius is zero only a single point will be added
    if (r > 0) {
        points.push_back(ivec2(c.x - r, c.y));
        points.push_back(ivec2(c.x, c.y + r));
        points.push_back(ivec2(c.x, c.y - r));
    }

    // p = (x — 0.5)^2 + (y + 1)^2 – r^2
    int p = 1 - r;
    while (x > y) {
        y++;

        if (p <= 0) {
            // Mid-point is inside or on the perimeter
            // p = p + 2(y + 1) + 1
            p = p + 2 * y + 1;

        } else {
            // Mid-point is outside the perimeter
            // p = p + 2(y + 1) – 2(x – 1) + 1
            x--;
            p = p + 2 * y - 2 * x + 1;
        }

        // All the perimeter points have already been added
        if (x < y) {
            break;
        }

        // Adding the generated point and its reflection
        // in the other octants after translation
        points.push_back(ivec2(c.x + x, c.y + y));
        points.push_back(ivec2(c.x - x, c.y + y));
        points.push_back(ivec2(c.x + x, c.y - y));
        points.push_back(ivec2(c.x - x, c.y - y));

        // If the generated point is on the line x = y then
        // the perimeter points have already been Added
        if (x != y) {
            points.push_back(ivec2(c.x + y, c.y + x));
            points.push_back(ivec2(c.x - y, c.y + x));
            points.push_back(ivec2(c.x + y, c.y - x));
            points.push_back(ivec2(c.x - y, c.y - x));
        }
    }

    return points;
}
