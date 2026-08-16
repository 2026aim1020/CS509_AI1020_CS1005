#ifndef GRADIENT_DESCENT_H
#define GRADIENT_DESCENT_H

#include <string>
#include <vector>

// f(x) = c0 + c1*x + c2*x^2 + ... + cd*x^d
struct GDInput
{
    int degree = 0;
    std::vector<double> coeffs; // size degree+1, ascending power order
    double x0 = 0.0;
    double learning_rate = 0.0;
    double tolerance = 0.0;
    long long max_iterations = 0;
};

struct GDResult
{
    double final_x = 0.0;
    double final_fx = 0.0;
    long long iterations = 0;
    bool converged = false;
};

double poly_eval(const std::vector<double> &coeffs, double x);
double poly_derivative_eval(const std::vector<double> &coeffs, double x);

// Parses the DEGREE / COEFFICIENTS / INITIAL_X / LEARNING_RATE / TOLERANCE /
// MAX_ITERATIONS text format and validates required constraints.
bool read_gd_input(const std::string &path, GDInput &in, std::string &error);

GDResult gradient_descent(const GDInput &in);

#endif
