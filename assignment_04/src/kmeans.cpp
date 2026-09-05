#include "../include/kmeans.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

bool read_kmeans_input(const std::string &path, KMeansInput &in, std::string &err)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        err = "could not open file: " + path;
        return false;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        err = "missing N D K header line";
        return false;
    }
    {
        std::istringstream ss(line);
        if (!(ss >> in.N >> in.D >> in.K))
        {
            err = "malformed N D K header line";
            return false;
        }
    }
    if (in.N <= 0 || in.D <= 0 || in.K <= 0)
    {
        err = "N, D, and K must all be positive";
        return false;
    }
    if (in.K > in.N)
    {
        err = "K must not exceed N";
        return false;
    }

    in.points.assign(in.N, std::vector<double>(in.D));
    for (int i = 0; i < in.N; ++i)
    {
        if (!std::getline(file, line))
        {
            err = "unexpected end of file while reading point " + std::to_string(i);
            return false;
        }
        std::istringstream ss(line);
        for (int d = 0; d < in.D; ++d)
        {
            if (!(ss >> in.points[i][d]))
            {
                err = "point " + std::to_string(i) + " has fewer than D coordinates";
                return false;
            }
        }
    }

    bool has_max_iter = false, has_tol = false;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string tag;
        if (!(ss >> tag))
            continue;
        if (tag == "MAX_ITERATIONS")
        {
            ss >> in.max_iterations;
            has_max_iter = true;
        }
        else if (tag == "TOLERANCE")
        {
            ss >> in.tolerance;
            has_tol = true;
        }
    }

    if (!has_max_iter || in.max_iterations <= 0)
    {
        err = "MAX_ITERATIONS must be present and positive";
        return false;
    }
    if (!has_tol || in.tolerance <= 0.0)
    {
        err = "TOLERANCE must be present and positive";
        return false;
    }

    return true;
}

static double squared_dist(const std::vector<double> &a, const std::vector<double> &b)
{
    double s = 0.0;
    for (size_t d = 0; d < a.size(); ++d)
    {
        double diff = a[d] - b[d];
        s += diff * diff;
    }
    return s;
}

KMeansResult kmeans(const KMeansInput &in)
{
    KMeansResult result;
    int N = in.N, D = in.D, K = in.K;

    // Initial centroids: first K input points, in input order.
    result.centroids.assign(K, std::vector<double>(D, 0.0));
    for (int k = 0; k < K; ++k)
        result.centroids[k] = in.points[k];

    result.assignment.assign(N, -1);

    int iter = 0;
    bool converged = false;
    for (; iter < in.max_iterations; ++iter)
    {
        // Assignment step.
        bool any_change = false;
        for (int i = 0; i < N; ++i)
        {
            int best = 0;
            double best_dist = squared_dist(in.points[i], result.centroids[0]);
            for (int k = 1; k < K; ++k)
            {
                double d = squared_dist(in.points[i], result.centroids[k]);
                if (d < best_dist)
                {
                    best_dist = d;
                    best = k;
                }
            }
            if (result.assignment[i] != best)
                any_change = true;
            result.assignment[i] = best;
        }

        // Update step.
        std::vector<std::vector<double>> sums(K, std::vector<double>(D, 0.0));
        std::vector<int> counts(K, 0);
        for (int i = 0; i < N; ++i)
        {
            int c = result.assignment[i];
            ++counts[c];
            for (int d = 0; d < D; ++d)
                sums[c][d] += in.points[i][d];
        }

        double max_shift = 0.0;
        for (int k = 0; k < K; ++k)
        {
            if (counts[k] == 0)
                continue; // keep previous centroid unchanged for this iteration
            std::vector<double> new_centroid(D);
            for (int d = 0; d < D; ++d)
                new_centroid[d] = sums[k][d] / counts[k];
            max_shift = std::max(max_shift, std::sqrt(squared_dist(new_centroid, result.centroids[k])));
            result.centroids[k] = new_centroid;
        }

        ++result.iterations;
        if (!any_change || max_shift <= in.tolerance)
        {
            converged = true;
            break;
        }
    }
    result.converged = converged;

    // WCSS with final assignment/centroids.
    double wcss = 0.0;
    for (int i = 0; i < N; ++i)
        wcss += squared_dist(in.points[i], result.centroids[result.assignment[i]]);
    result.wcss = wcss;

    return result;
}
