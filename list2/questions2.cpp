#include <iostream>
#include <vector>
#include "../lib/euclidian.h"

using namespace std;

void printVector(vector<float> v) {
    cout << "{ ";
    for (float f: v) {
      cout << f << " ";
    }
    cout << "}\n";
}

void question3() {
    cout << "Question 3:\n";

    vector<float> u{1, 1, 0};
    vector<float> v{0, 2, 3};
    vector<float> w{2, 4, 3};

    float angle_u_v = rad2dgr(innerAngle(u, v));
    float angle_u_w = rad2dgr(innerAngle(u, w));
    float angle_v_w = rad2dgr(innerAngle(v, w));

    cout << "Inner angles in degrees\n";
    cout << "u e v: " << angle_u_v << "\n";
    cout << "u e w: " << angle_u_w << "\n";
    cout << "v e w: " << angle_v_w << "\n";

    vector<float> prod_u_v = crossProduct(u, v);
    vector<float> prod_u_w = crossProduct(u, w);
    vector<float> prod_v_w = crossProduct(v, w);

    cout << "Cross product\n";
    cout << "u e v: ";
    printVector(prod_u_v);
    cout << "u e w: ";
    printVector(prod_u_w);
    cout << "v e w: ";
    printVector(prod_v_w);
}

void question4() {
    cout << "Question 4:\n";

    vector<float> u{-2, 3, 1};
    vector<float> v{0, 4, 1};
    
    vector<float> cross_prod = crossProduct(u, v);
    float paralelogram_area = length(cross_prod);
    cout << "Paralelogram area: " << paralelogram_area << "\n";
}

void question5() {
    cout << "Question 5:\n";

    vector<float> u{2, 6, 12};
    vector<float> v{1, 3, 6};

    vector<float> cross_prod = crossProduct(u, v);
    float cross_prod_len = length(cross_prod);
    cout << "Cross product length: " << cross_prod_len << "\n";
}

int main(int argc, char** argv) {
    // question3();
    // question4();
    // question5();

    return 0;
};
