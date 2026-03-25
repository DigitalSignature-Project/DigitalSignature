#include <pybind11/pybind11.h>


double calculate_bmi(double height, double weight) {
    double height_m = height / 100;
    double bmi_value = weight / (height_m * height_m);
    return bmi_value;
}

int suma_liczb(int a, int b) {
    return a + b;
}

PYBIND11_MODULE(bmi_computing, m) {
    m.def("calculate_bmi", &calculate_bmi, "Calculate bmi");
    m.def("add_two_numbers", &suma_liczb, "Add two numbers");
}
