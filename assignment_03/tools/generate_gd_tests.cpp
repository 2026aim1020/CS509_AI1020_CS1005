#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct GDSpec
{
    std::string name;
    int degree;
    std::vector<double> coeffs; // ascending power order: c0 .. cd
    double x0, rate, tol;
    long long maxIter;
};

int main(int argc, char **argv)
{
    std::string outDir = argc > 1 ? argv[1] : ".";
    fs::create_directories(outDir);

    std::vector<GDSpec> specs = {
        // gd_01: x^2 - 6x + 9
        {"gd_01", 2, {9, -6, 1}, 0, 0.10, 1e-6, 5000},
        // gd_02: x^4 + 2x^2
        {"gd_02", 4, {0, 0, 2, 0, 1}, 2, 0.02, 1e-6, 10000},
        // gd_03: 0.1x^6 + 0.5x^4 + x^2
        {"gd_03", 6, {0, 0, 1, 0, 0.5, 0, 0.1}, 2, 0.02, 1e-6, 20000},
        // gd_04: 0.01x^8 + 0.05x^6 + 0.2x^4 + x^2
        {"gd_04", 8, {0, 0, 1, 0, 0.2, 0, 0.05, 0, 0.01}, 2, 0.01, 1e-8, 50000},
        // gd_05: 0.002x^10 + 0.01x^8 + 0.05x^6 + 0.2x^4 + x^2
        {"gd_05", 10, {0, 0, 1, 0, 0.2, 0, 0.05, 0, 0.01, 0, 0.002}, 2, 0.005, 1e-10, 100000},
    };

    for (const auto &s : specs)
    {
        std::string path = outDir + "/" + s.name + ".txt";
        std::ofstream out(path);
        out << "DEGREE " << s.degree << "\n";
        out << "COEFFICIENTS";
        for (double c : s.coeffs)
            out << " " << c;
        out << "\n";
        out << "INITIAL_X " << s.x0 << "\n";
        out << "LEARNING_RATE " << s.rate << "\n";
        out << "TOLERANCE " << s.tol << "\n";
        out << "MAX_ITERATIONS " << s.maxIter << "\n";
        out.close();
        std::cout << "Wrote " << path << "\n";
    }
    return 0;
}
