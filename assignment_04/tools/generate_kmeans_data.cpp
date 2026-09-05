// Generates a random K-Means points file: K well-separated Gaussian blobs
// in D dimensions, N points total.
//
// Build: g++ -O2 -std=c++17 tools/generate_kmeans_data.cpp -o gen_kmeans.exe
// Usage: ./gen_kmeans.exe <N> <D> <K> <output_path> [seed] [max_iter] [tolerance]

#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " <N> <D> <K> <output_path> [seed] [max_iter] [tolerance]\n";
        return 1;
    }
    int N = std::stoi(argv[1]);
    int D = std::stoi(argv[2]);
    int K = std::stoi(argv[3]);
    std::string out_path = argv[4];
    unsigned seed = argc > 5 ? static_cast<unsigned>(std::stoul(argv[5])) : 42u;
    int max_iter = argc > 6 ? std::stoi(argv[6]) : 300;
    double tolerance = argc > 7 ? std::stod(argv[7]) : 1e-4;

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> center_dist(0.0, 100.0);
    std::normal_distribution<double> spread(0.0, 3.0);

    std::vector<std::vector<double>> centers(K, std::vector<double>(D));
    for (int k = 0; k < K; ++k)
        for (int d = 0; d < D; ++d)
            centers[k][d] = center_dist(rng);

    std::ofstream out(out_path);
    out << N << " " << D << " " << K << "\n";
    out << std::fixed << std::setprecision(6);
    for (int i = 0; i < N; ++i)
    {
        int k = i % K; // spread points evenly across blobs
        for (int d = 0; d < D; ++d)
        {
            double v = centers[k][d] + spread(rng);
            out << v << (d + 1 < D ? " " : "\n");
        }
    }
    out << "MAX_ITERATIONS " << max_iter << "\n";
    out << "TOLERANCE " << tolerance << "\n";
    std::cout << "Wrote " << out_path << " with N=" << N << " D=" << D << " K=" << K << "\n";
    return 0;
}
