#include "../include/gradient_descent.h"

#include <cmath>
#include <fstream>
#include <sstream>

double poly_eval(const std::vector<double> &coeffs, double x)
{
    double result = 0.0, power = 1.0;
    for (double c : coeffs)
    {
        result += c * power;
        power *= x;
    }
    return result;
}

double poly_derivative_eval(const std::vector<double> &coeffs, double x)
{
    double result = 0.0, power = 1.0;
    for (size_t k = 1; k < coeffs.size(); ++k)
    {
        result += static_cast<double>(k) * coeffs[k] * power;
        power *= x;
    }
    return result;
}

bool read_gd_input(const std::string &path, GDInput &in, std::string &error)
{
    std::ifstream f(path);
    if (!f.is_open())
    {
        error = "could not open file";
        return false;
    }

    std::string tag;
    bool have_degree = false;
    while (f >> tag)
    {
        if (tag == "DEGREE")
        {
            f >> in.degree;
            have_degree = true;
        }
        else if (tag == "COEFFICIENTS")
        {
            if (!have_degree || in.degree < 0)
            {
                error = "COEFFICIENTS given before a valid DEGREE";
                return false;
            }
            in.coeffs.assign(in.degree + 1, 0.0);
            for (int i = 0; i <= in.degree; ++i)
            {
                if (!(f >> in.coeffs[i]))
                {
                    error = "coefficient count != degree+1";
                    return false;
                }
            }
        }
        else if (tag == "INITIAL_X")
        {
            f >> in.x0;
        }
        else if (tag == "LEARNING_RATE")
        {
            f >> in.learning_rate;
        }
        else if (tag == "TOLERANCE")
        {
            f >> in.tolerance;
        }
        else if (tag == "MAX_ITERATIONS")
        {
            f >> in.max_iterations;
        }
    }

    if (in.degree < 1)
    {
        error = "invalid degree";
        return false;
    }
    if (static_cast<int>(in.coeffs.size()) != in.degree + 1)
    {
        error = "coefficient count != degree+1";
        return false;
    }
    if (in.learning_rate <= 0)
    {
        error = "non-positive learning rate";
        return false;
    }
    if (in.tolerance <= 0)
    {
        error = "non-positive tolerance";
        return false;
    }
    if (in.max_iterations <= 0)
    {
        error = "non-positive max iterations";
        return false;
    }
    return true;
}

GDResult gradient_descent(const GDInput &in)
{
    GDResult res;
    double x = in.x0;
    long long iter = 0;

    double grad = poly_derivative_eval(in.coeffs, x);
    while (std::fabs(grad) > in.tolerance && iter < in.max_iterations)
    {
        x = x - in.learning_rate * grad; // x = x - learning_rate * grad
        grad = poly_derivative_eval(in.coeffs, x);
        ++iter;
    }

    res.final_x = x;
    res.final_fx = poly_eval(in.coeffs, x);
    res.iterations = iter;
    res.converged = std::fabs(grad) <= in.tolerance;
    return res;
}
