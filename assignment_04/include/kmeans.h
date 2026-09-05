#ifndef KMEANS_H
#define KMEANS_H

#include <string>
#include <vector>

struct KMeansInput
{
    int N = 0;                               // number of points
    int D = 0;                               // dimensions
    int K = 0;                               // number of clusters
    std::vector<std::vector<double>> points; // N x D
    int max_iterations = 300;
    double tolerance = 1e-4;
};

struct KMeansResult
{
    std::vector<int> assignment;                // cluster id per point
    std::vector<std::vector<double>> centroids; // K x D
    double wcss = 0.0;
    int iterations = 0;
    bool converged = false;
};

// Reads the points file described in the assignment spec:
//   N D K
//   D coordinates per line, N lines
//   MAX_ITERATIONS n
//   TOLERANCE epsilon
bool read_kmeans_input(const std::string &path, KMeansInput &in, std::string &err);

// Lloyd's algorithm. Initial centroids are the first K input points (in
// input order) for reproducibility. If a cluster becomes empty during an
// update, its previous centroid is kept unchanged for that iteration.
// Stops when the maximum centroid shift is <= tolerance or max_iterations
// is reached.
KMeansResult kmeans(const KMeansInput &in);

#endif
