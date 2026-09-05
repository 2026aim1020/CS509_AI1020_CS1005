#ifndef FASTMAP_H
#define FASTMAP_H

#include <string>
#include <utility>
#include <vector>

struct FastMapInput
{
    int N = 0; // number of objects
    int K = 0; // target embedding dimensionality
    std::vector<std::vector<double>> dist; // N x N symmetric, zero diagonal
};

struct FastMapResult
{
    std::vector<std::vector<double>> coords; // N x K
    std::vector<std::pair<int, int>> pivots; // one pair per dimension
};

// Reads the N x N pairwise distance matrix file 
bool read_fastmap_input(const std::string &path, FastMapInput &in, std::string &err);

FastMapResult fastmap(const FastMapInput &in, unsigned seed = 42);

#endif
