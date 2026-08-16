// Driver for the Triangle Counting buddy task.
//
// Usage:
//   ./tc_driver <graph_input_file> [--list]
//
// --list forces printing the individual triangles even for large graphs

#include "../../assignment_01/include/csr.h"
#include "../include/graph_analytics.h"
#include <chrono>
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Error: missing input file.\n"
                  << "Usage: " << argv[0] << " <graph_input_file> [--list|--no-list]\n";
        return 1;
    }

    std::string path = argv[1];
    std::string flag = (argc >= 3) ? argv[2] : "";

    AdjacencyList g;
    if (!read_adjacency_list(path, g))
    {
        std::cerr << "Error: could not read or parse input file '" << path << "'.\n";
        return 1;
    }

    CSR csr = build_csr(g); // preprocessing, not timed

    bool collect_list = (g.V <= 100);
    if (flag == "--list")
        collect_list = true;
    if (flag == "--no-list")
        collect_list = false;

    auto t1 = std::chrono::high_resolution_clock::now();
    TriangleResult result = count_triangles(csr, collect_list);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    std::cout << "Algorithm: Triangle Counting\n";
    std::cout << "Total triangles: " << result.total_triangles << "\n";
    if (collect_list)
    {
        std::cout << "Triangles found:\n";
        for (const auto &t : result.triangles)
        {
            std::cout << "(" << t[0] << ", " << t[1] << ", " << t[2] << ")\n";
        }
    }
    std::cout << "Execution time: " << ms << " ms\n";
    return 0;
}
