#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

float paralelogram_area(vec2 p1, vec2 p2, vec2 p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

bool left(vec2 p1, vec2 p2, vec2 p3) {
    return paralelogram_area(p1, p2, p3) > 0.0f;
}

bool leftOn(vec2 p1, vec2 p2, vec2 p3) {
    return paralelogram_area(p1, p2, p3) >= 0.0f;
}

void testLeft(vec2 p1, vec2 p2, vector<vec2> points) {
    cout << endl << "=============================================" << endl;
    cout << "p1p2: " << to_string(p2 - p1) << endl;

    for (vec2 p : points) {
        cout << endl << "p3: " << to_string(p) << endl;
        cout << "left: " << (left(p1, p2, p) ? "True" : "False") << endl;
        cout << "leftOn: " << (leftOn(p1, p2, p) ? "True" : "False") << endl;
    }
}

int main() {
    vec2 p1;
    vec2 p2;
    vector<vec2> points{
        vec2{0.0f, 0.0f},
        vec2{1.0f, 0.0f},
        vec2{1.0f, 1.0f},
        vec2{0.0f, 1.0f},
        vec2{-1.0f, 1.0f},
        vec2{-1.0f, 0.0f},
        vec2{-1.0f, -1.0f},
        vec2{0.0f, -1.0f},
        vec2{1.0f, -1.0f}
    };

    p1 = vec2{0.0f, 0.0f};
    p2 = vec2{1.0f, 0.0f};
    testLeft(p1, p2, points);

    p1 = vec2{0.0f, 0.0f};
    p2 = vec2{1.0f, 1.0f};
    testLeft(p1, p2, points);

    p1 = vec2{0.0f, 0.0f};
    p2 = vec2{0.0f, 1.0f};
    testLeft(p1, p2, points);

    p1 = vec2{0.0f, 0.0f};
    p2 = vec2{-1.0f, 1.0f};
    testLeft(p1, p2, points);
}