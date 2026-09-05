// Generates a random FastMap distance-matrix file. N random objects are
// placed in a small Euclidean space internally (never written out) and the
// N x N matrix of pairwise Euclidean distances is written, which
// guarantees a symmetric, zero-diagonal matrix that satisfies the
// triangle inequality.

// Build: g++ -O2 -std=c++17 tools/generate_fastmap_data.cpp -o gen_fastmap.exe
// Usage: ./gen_fastmap.exe <N> <K> <output_path> [seed] [underlying_dims]

#include <cmath>
#include <fstream>
#include <limits>
#include <string>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " <N> <K> <output_path> [seed] [underlying_dims]\n";
        return 1;
    }
    int N = std::stoi(argv[1]);
    int K = std::stoi(argv[2]);
    std::string out_path = argv[3];
    unsigned seed = argc > 4 ? static_cast<unsigned>(std::stoul(argv[4])) : 42u;
    int underlying_dims = argc > 5 ? std::stoi(argv[5]) : std::max(K + 1, 4);

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> coord_dist(0.0, 100.0);

    std::vector<std::vector<double>> points(N, std::vector<double>(underlying_dims));
    for (int i = 0; i < N; ++i)
        for (int d = 0; d < underlying_dims; ++d)
            points[i][d] = coord_dist(rng);

    std::ofstream out(out_path);
    out << N << " " << K << "\n";
    out << std::fixed << std::setprecision(4);
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            double dist = 0.0;
            if (i != j)
            {
                double s = 0.0;
                for (int d = 0; d < underlying_dims; ++d)
                {
                    double diff = points[i][d] - points[j][d];
                    s += diff * diff;
                }
                dist = std::sqrt(s);
            }
            out << dist << (j + 1 < N ? " " : "\n");
        }
    }
    std::cout << "Wrote " << out_path << " with N=" << N << " K=" << K << "\n";
    return 0;
}
