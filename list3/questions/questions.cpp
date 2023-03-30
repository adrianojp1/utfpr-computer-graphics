#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

void question1() {
    mat4 Rx = rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
    mat4 Rz = rotate(mat4(1.0f), radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    
    mat4 Rxz = Rx*Rz;
    mat4 Rzx = Rx*Rx;

    cout << "Rxz: " << to_string(transpose(Rxz)) << endl;
    cout << "Rzx: " << to_string(transpose(Rzx)) << endl << endl;
}

void question4() {
    mat4 S = scale(mat4(1.0f), vec3(2.0f, 4.0f, 1.0f/3));
    mat4 T = translate(mat4(1.0f), vec3(1.0f, -2.0f, 7.0f));
    mat4 M = T*S;
    vec4 p1 (0.0f, 0.0f, 0.0f, 1.0f);
    vec4 p2 (1.0f, 1.0f, 1.0f, 1.0f);

    vec4 t_p1 = M * p1;
    vec4 t_p2 = M * p2;

    mat4 M_inv = inverse(M);

    cout << "M: " << to_string(transpose(M)) << endl;
    cout << "t_p1: " << to_string(t_p1) << endl;
    cout << "t_p2: " << to_string(t_p2) << endl;
    cout << "M_inv: " << to_string(transpose(M_inv)) << endl << endl;
}

void question5() {
    mat4 R = rotate(mat4(1.0f), radians(90.0f), vec3(1.0f, 1.0f, 1.0f));
    cout << "R: " << to_string(transpose(R)) << endl << endl;
}

void question6() {
    mat4 R = rotate(mat4(1.0f), radians(30.0f), vec3(0.0f, 1.0f, 0.0f));

    vec3 fixed_p = vec3(3.0f, 0.0f, 6.0f);
    mat4 T = translate(mat4(1.0f), fixed_p);
    mat4 T_inv = inverse(T);
    mat4 with_fixed_p = T*R*T_inv;

    cout << "R: " << to_string(transpose(R)) << endl;
    cout << "with_fixed_p: " << to_string(transpose(with_fixed_p)) << endl;
}


int main() {
    question1();
    question4();
    question5();
    question6();
}