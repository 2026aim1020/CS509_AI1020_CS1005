# CS509 Assignment 01


This covers the BFS, DFS, and SSSP buddy tasks, all built on top of the
same CSR representation.

- **CSR Graph**: adjacency-list -> Compressed Sparse Row conversion helper
- **BFS**: level-order traversal + minimum edge-count distances, over CSR
- **DFS**: pre-order traversal (iterative, explicit stack), over CSR
- **SSSP**: Dijkstra's algorithm (positive weights only), over CSR

The CSR module (`include/csr.h`, `src/csr.cpp`) is shared by all three graph
algorithms - each driver reads the adjacency list, builds CSR once
(untimed), then times only the algorithm call itself.

## Project layout

```
include/csr.h          AdjacencyList / CSR structs, function declarations
src/csr.cpp            read_adjacency_list, build_csr, print_csr
src/csr_driver.cpp     CSR driver (reads graph file, times conversion, prints)
include/graph_algorithms.h  bfs_csr, dfs_csr, sssp_dijkstra_csr declarations
src/graph_algorithms.cpp    BFS, DFS, and Dijkstra SSSP implementations (operate on CSR)
src/bfs_driver.cpp     BFS driver (reads file, builds CSR, times BFS, prints per spec)
src/dfs_driver.cpp     DFS driver (reads file, builds CSR, times DFS, prints per spec)
src/sssp_driver.cpp    SSSP driver (reads file, builds CSR, times Dijkstra, prints per spec)
                        every task concurrently (see below)
tools/gen_graph_tests.cpp  generates graph files at V = 10/100/10000/50000/100000
tests/                 generated input files
build.sh               compiles both drivers
common_wrapper/wrapper.cpp        Menu-driven runner: single test / all tests / run
```

## Build

```bash
./build.sh
```

## Run

```bash

# BFS / DFS / SSSP: <graph file>
./bfs_driver tests/graphs/bfs_dfs_10.txt
./dfs_driver tests/graphs/bfs_dfs_10.txt
./sssp_driver tests/graphs/sssp_10.txt
```

## Menu-driven wrapper (run everything from one program)

`./wrapper` gives an interactive menu instead of calling each driver
separately:

```
Main menu:
1. Run BFS
2. Run DFS
3. Run SSSP
4. Run All ( BFS + DFS + SSSP at the same time)
5. Exit
```

Options 1-3 each open a submenu to either run **a single test file** (you
type the path) or **every `.txt` file in a folder** `tests/graphs` for the graph tasks - BFS/DFS/SSSP all
read from the same folder since BFS and DFS ignore edge weights when
present, and SSSP falls back to unit weights with a warning if a file has
none). Option 4 launches all five suites on five separate `std::thread`s so
they execute **at the same time**; a mutex only serializes the console
`print` calls so lines don't interleave mid-print, it does not serialize the
actual work - the interleaved-by-completion-order output when running the
full test suite is proof they're genuinely concurrent. Total wall-clock time
for the concurrent run is printed at the end for reference, but per-file
algorithm timings (used for the README tables) are still the individual
numbers printed for each file - the wrapper does not change how those are
measured, it only changes how the test files are dispatched.

The wrapper links directly against `csr.cpp`/`graph_algorithms.cpp`
it calls the exact same timed
functions as  `csr_driver`, `bfs_driver`, `dfs_driver`, and
`sssp_driver`.

## Generate test files

```bash
g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ tools/gen_graph_tests.cpp -o tools/gen_graph_tests

./tools/gen_graph_tests tests/graphs
```

## Timing methodology

- The timer (`std::chrono::high_resolution_clock`) starts immediately before
  calling `build_csr`, and stops immediately
  after. File reading, parsing, and result printing are excluded, per the
  assignment's timing rule.
- CSR conversion time is reported **separately** from any graph algorithm's
  time, since it is preprocessing (relevant once BFS/DFS/SSSP are added).


## 9.3 BFS / DFS / SSSP Results Table

Source = vertex 0 for every run (matches the "SOURCE 0" line each generated
file ends with). Expected/Actual Output = "full traversal" for BFS/DFS
(all V vertices, each exactly once) and "all vertices reachable" for SSSP,
which is what the generated graphs guarantee since they're built from a
connected spanning path.

| Algorithm | Test File | Vertices | Edges | Input Type | Source | Expected Output | Actual Output | Time | Status |
|---|---|---|---|---|---|---|---|---|---|
| BFS | bfs_dfs_10.txt (spec example) | 5 | 5 | Unweighted adjacency list | 0 | Traversal: 0 1 2 3 4 | Traversal: 0 1 2 3 4 | 0.00127 ms | Pass |
| DFS | bfs_dfs_10.txt (spec example) | 5 | 5 | Unweighted adjacency list | 0 | Traversal: 0 1 3 2 4 | Traversal: 0 1 3 2 4 | 0.00185 ms | Pass |
| SSSP | sssp_10.txt (spec example) | 5 | 6 | Positive-weighted adjacency list | 0 | 0,3,1,4,7 | 0,3,1,4,7 | 0.00177 ms | Pass |
| BFS | graph_unweighted_10.txt | 10 | 29 | Unweighted adjacency list | 0 | full traversal (10/10) | 10/10 visited | 0.00222 ms | Pass |
| DFS | graph_unweighted_10.txt | 10 | 29 | Unweighted adjacency list | 0 | full traversal (10/10) | 10/10 visited | 0.00200 ms | Pass |
| BFS | graph_unweighted_100.txt | 100 | 299 | Unweighted adjacency list | 0 | full traversal (100/100) | 100/100 visited | 0.00883 ms | Pass |
| DFS | graph_unweighted_100.txt | 100 | 299 | Unweighted adjacency list | 0 | full traversal (100/100) | 100/100 visited | 0.00982 ms | Pass |
| BFS | graph_unweighted_10000.txt | 10,000 | 29,999 | Unweighted adjacency list | 0 | full traversal (10000/10000) | 10000/10000 visited | 0.539 ms | Pass |
| DFS | graph_unweighted_10000.txt | 10,000 | 29,999 | Unweighted adjacency list | 0 | full traversal (10000/10000) | 10000/10000 visited | 0.869 ms | Pass |
| BFS | graph_unweighted_50000.txt | 50,000 | 149,999 | Unweighted adjacency list | 0 | full traversal (50000/50000) | 50000/50000 visited | 3.966 ms | Pass |
| DFS | graph_unweighted_50000.txt | 50,000 | 149,999 | Unweighted adjacency list | 0 | full traversal (50000/50000) | 50000/50000 visited | 5.474 ms | Pass |
| BFS | graph_unweighted_100000.txt | 100,000 | 299,999 | Unweighted adjacency list | 0 | full traversal (100000/100000) | 100000/100000 visited | 10.26 ms | Pass |
| DFS | graph_unweighted_100000.txt | 100,000 | 299,999 | Unweighted adjacency list | 0 | full traversal (100000/100000) | 100000/100000 visited | 17.71 ms | Pass |
| SSSP | graph_weighted_10.txt | 10 | 29 | Positive-weighted adjacency list | 0 | all reachable (10/10) | 10/10 reachable | 0.00273 ms | Pass |
| SSSP | graph_weighted_100.txt | 100 | 299 | Positive-weighted adjacency list | 0 | all reachable (100/100) | 100/100 reachable | 0.0440 ms | Pass |
| SSSP | graph_weighted_10000.txt | 10,000 | 29,999 | Positive-weighted adjacency list | 0 | all reachable (10000/10000) | 10000/10000 reachable | 2.804 ms | Pass |
| SSSP | graph_weighted_50000.txt | 50,000 | 149,999 | Positive-weighted adjacency list | 0 | all reachable (50000/50000) | 50000/50000 reachable | 26.63 ms | Pass |
| SSSP | graph_weighted_100000.txt | 100,000 | 299,999 | Positive-weighted adjacency list | 0 | all reachable (100000/100000) | 100000/100000 reachable | 60.20 ms | Pass |

Algorithm notes:
- BFS/DFS traversal order is driven entirely by the order neighbours appear
  in the CSR (which mirrors the input file's adjacency-list order) - both
  are deterministic and reproducible for a given input file.
- DFS is implemented iteratively with an explicit stack (neighbours pushed
  in reverse CSR order) so it produces the same pre-order traversal as a
  straightforward recursive DFS, without recursion-depth limits on the
  100,000-vertex graphs.
- SSSP uses Dijkstra's algorithm with a binary min-heap
  (`std::priority_queue`), correct only for positive edge weights, per the
  assignment's stated constraint. Running it on a graph file with no weights
  falls back to unit weights and prints a warning rather than failing.

## Notes

- Randomized test-data generation seeds are fixed (`std::mt19937 rng(65)` for graphs), so re-running the generator
  programs reproduces the same files and results shown above.
- All times above are single-run wall-clock measurements from
  `std::chrono::high_resolution_clock`; per the spec's guidance on very fast
  inputs