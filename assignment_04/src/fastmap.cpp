#include "../include/fastmap.h"
#include <cmath>
#include <fstream>
#include <random>
#include <sstream>

bool read_fastmap_input(const std::string &path, FastMapInput &in, std::string &err)
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
        err = "missing N K header line";
        return false;
    }
    {
        std::istringstream ss(line);
        if (!(ss >> in.N >> in.K))
        {
            err = "malformed N K header line";
            return false;
        }
    }
    if (in.N <= 0)
    {
        err = "N must be positive";
        return false;
    }
    if (in.K <= 0 || in.K >= in.N)
    {
        err = "K must be positive and less than N";
        return false;
    }

    in.dist.assign(in.N, std::vector<double>(in.N, 0.0));
    for (int i = 0; i < in.N; ++i)
    {
        if (!std::getline(file, line))
        {
            err = "unexpected end of file while reading distance row " + std::to_string(i);
            return false;
        }
        std::istringstream ss(line);
        for (int j = 0; j < in.N; ++j)
        {
            if (!(ss >> in.dist[i][j]))
            {
                err = "distance row " + std::to_string(i) + " has fewer than N entries";
                return false;
            }
            if (in.dist[i][j] < 0.0)
            {
                err = "negative distance at (" + std::to_string(i) + "," + std::to_string(j) + ")";
                return false;
            }
        }
    }

    for (int i = 0; i < in.N; ++i)
    {
        if (in.dist[i][i] != 0.0)
        {
            err = "non-zero diagonal entry at (" + std::to_string(i) + "," + std::to_string(i) + ")";
            return false;
        }
        for (int j = i + 1; j < in.N; ++j)
        {
            if (in.dist[i][j] != in.dist[j][i])
            {
                err = "matrix is not symmetric at (" + std::to_string(i) + "," + std::to_string(j) + ")";
                return false;
            }
        }
    }

    return true;
}

static int farthest_from(int from, const std::vector<std::vector<double>> &d)
{
    int N = static_cast<int>(d.size());
    int best = (from == 0) ? 1 : 0;
    double best_d = d[from][best];
    for (int j = 0; j < N; ++j)
    {
        if (j == from)
            continue;
        if (d[from][j] > best_d)
        {
            best_d = d[from][j];
            best = j;
        }
    }
    return best;
}

FastMapResult fastmap(const FastMapInput &in, unsigned seed)
{
    FastMapResult result;
    int N = in.N, K = in.K;
    result.coords.assign(N, std::vector<double>(K, 0.0));

    std::vector<std::vector<double>> d = in.dist; // working (deflated) distances
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> pick(0, N - 1);

    for (int dim = 0; dim < K; ++dim)
    {
        // Heuristic pivot selection
        int a = pick(rng);
        int b = farthest_from(a, d);
        for (int r = 0; r < 2; ++r)
        {
            a = farthest_from(b, d);
            b = farthest_from(a, d);
        }
        result.pivots.push_back({a, b});

        double dab = d[a][b];
        if (dab <= 0.0)
        {
            // All remaining objects coincide on this axis; leave this
            // dimension's coordinates at 0 and skip deflation.
            continue;
        }

        for (int i = 0; i < N; ++i)
        {
            double dai = d[a][i];
            double dbi = d[b][i];
            result.coords[i][dim] = (dai * dai + dab * dab - dbi * dbi) / (2.0 * dab);
        }

        for (int i = 0; i < N; ++i)
        {
            for (int j = i + 1; j < N; ++j)
            {
                double delta = result.coords[i][dim] - result.coords[j][dim];
                double sq = d[i][j] * d[i][j] - delta * delta;
                double newd = sq > 0.0 ? std::sqrt(sq) : 0.0;
                d[i][j] = newd;
                d[j][i] = newd;
            }
        }
    }

    return result;
}
