#ifndef GRAPH_ALGORITHMS_H
#define GRAPH_ALGORITHMS_H

#include "csr.h"
#include <vector>
#include <limits>


// --- SSSP (Dijkstra) ---------------------------------------------------

struct SSSPResult
{
    std::vector<double> distance;
};
SSSPResult sssp_dijkstra_csr(const CSR &csr, int source);

#endif 
