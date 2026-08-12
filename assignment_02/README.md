# CS509 - Assignment 2 -(Triangle Counting + Betweenness Centrality + Connected Components)

## Reuse of Assignment 1

- _All three algorithms_ read their input and build CSR using
  Assignment 1's read_adjacency_list() / build_csr() unmodified.
- _Connected Components_ genuinely calls Assignment 1's bfs_csr()
  directly (declared in graph_algorithms.h) once per undiscovered
  component
- _Betweenness Centrality_ (Brandes' algorithm) needs each BFS to also
  track shortest-path counts (sigma) and predecessor lists, which
  Assignment 1's bfs_csr() (traversal + distance only) doesn't record.
  Rather than force that extra bookkeeping through a function that wasn't
  built for it, the BFS phase is implemented directly inside
  betweenness_centrality() - but it walks the exact same CSR
  row_ptr/col_idx arrays with the same level-order queue logic as
  Assignment 1's bfs_csr(). This is documented in graph_analytics.h
- _Triangle Counting_ sorts a private copy of each vertex's CSR
  neighbour list (per the spec's suggested optimization, so the
  common-neighbour check can binary-search instead of linear-scan). This
  sorting is buddy-task-specific post-processing on top of the CSR
  Assignment 1 already built

## Project layout

include/graph*analytics.h TriangleResult/count_triangles, CentralityResult/
betweenness_centrality, ComponentsResult/connected_components
src/graph_analytics.cpp Implementations (built on Assignment 1's CSR + bfs_csr)
src/tc_driver.cpp Triangle Counting driver
src/bc_driver.cpp Betweenness Centrality driver
src/cc_driver.cpp Connected Components driver
src/wrapper.cpp Menu-driven runner (single test / all tests / run
all three concurrently)
tools/gen_undirected_tests.cpp Generates tc*/bc*/cc* \*.txt at required sizes
tests/ Spec examples + generated test files
build.sh Compiles everything (set A1=<path> if Assignment 1
isn't at ../../assignment1)

## Build

bash
./build.sh

## tests

bash
./tools/gen_undirected_tests tests/graphs

## Run

bash
./tc_driver tests/tc_example.txt
./bc_driver tests/bc_example.txt
./cc_driver tests/cc_example.txt
./wrapper # menu-driven: single test / all tests / run all three concurrently

## Test data notes

- tc*<V>.txt / cc*<V>.txt are the _same underlying connected graph_
  per size (spanning path + extra edges + deliberate i -> i+2 edges so
  small graphs are guaranteed real triangles to count), just written under
  both algorithms' naming conventions since both need V=10/100/10000/
  50000/100000 anyway.
- bc\_<V>.txt uses the same graph-building approach but at Betweenness
  Centrality's reduced size list (10/100/1000/5000/10000), per the spec's
  O(V·E) cost note.
- These generated graphs are all single-component by construction (the
  spanning path guarantees connectivity), so Connected Components always
  reports 1 on them - that's expected, not a bug. The **isolated-vertex /
  multi-component** behavior is verified separately with the spec's own
  example (tests/cc_example.txt, 8 vertices, 4 components including two
  isolated vertices), which matches Section 9.3's expected output exactly.
- Triangle Counting prints the individual triangle list automatically when
  V ≤ 100 (both required sizes get it) and only the total above

## 11.2 Graph Analytics Results Table

| Algorithm              | Test File                     | Vertices | Edges   | Expected Output                                | Actual Output   | Time             | Status |
| ---------------------- | ----------------------------- | -------- | ------- | ---------------------------------------------- | --------------- | ---------------- | ------ |
| Triangle Counting      | tc_example.txt (spec example) | 6        | 8       | Total triangles: 3, list (0,1,2)(1,2,3)(3,4,5) | matches exactly | 0.0035 ms        | Pass   |
| Betweenness Centrality | bc_example.txt (spec example) | 5        | 4       | 0.00, 3.00, 4.00, 3.00, 0.00                   | matches exactly | 0.0034 ms        | Pass   |
| Connected Components   | cc_example.txt (spec example) | 8        | 4       | 4 components, ids 0,0,0,0,1,1,2,3              | matches exactly | 0.0027 ms        | Pass   |
| Triangle Counting      | tc_10.txt                     | 10       | 33      | triangle count                                 | 50              | 0.0069 ms        | Pass   |
| Connected Components   | cc_10.txt                     | 10       | 33      | component count                                | 1               | 0.0021 ms        | Pass   |
| Betweenness Centrality | bc_10.txt                     | 10       | 33      | centrality per vertex                          | max=2.99        | 0.0094 ms        | Pass   |
| Triangle Counting      | tc_100.txt                    | 100      | 348     | triangle count                                 | 91              | 0.0498 ms        | Pass   |
| Connected Components   | cc_100.txt                    | 100      | 348     | component count                                | 1               | 0.0056 ms        | Pass   |
| Betweenness Centrality | bc_100.txt                    | 100      | 348     | centrality per vertex                          | max=366.89      | 0.5700 ms        | Pass   |
| Betweenness Centrality | bc_1000.txt                   | 1,000    | 3,498   | centrality per vertex                          | max=5999.67     | 71.0 ms          | Pass   |
| Triangle Counting      | tc_10000.txt                  | 10,000   | 34,998  | triangle count                                 | 5,045           | 4.48 ms          | Pass   |
| Connected Components   | cc_10000.txt                  | 10,000   | 34,998  | component count                                | 1               | 0.45 ms          | Pass   |
| Betweenness Centrality | bc_5000.txt                   | 5,000    | 17,498  | centrality per vertex                          | max=50,630.92   | 1,712 ms         | Pass   |
| Betweenness Centrality | bc_10000.txt                  | 10,000   | 34,998  | centrality per vertex                          | max=125,071.03  | 7,300-8,180 ms\* | Pass   |
| Triangle Counting      | tc_50000.txt                  | 50,000   | 174,998 | triangle count                                 | 25,034          | 27.48 ms         | Pass   |
| Connected Components   | cc_50000.txt                  | 50,000   | 174,998 | component count                                | 1               | 2.59 ms          | Pass   |
| Triangle Counting      | tc_100000.txt                 | 100,000  | 349,998 | triangle count                                 | 50,039          | 60.69 ms         | Pass   |
| Connected Components   | cc_100000.txt                 | 100,000  | 349,998 | component count                                | 1               | 5.58 ms          | Pass   |

\*\* Two runs shown for the largest Betweenness Centrality case (standalone
bc_driver run vs. through ./wrapper's "Run All", where it ran
concurrently alongside Triangle Counting and Connected Components on
separate threads) - both completed correctly

## Notes

- All times are single-run wall-clock measurements from
  std::chrono::high_resolution_clock; the smallest cases are
  noise-dominated (sub-microsecond), reported as-is per the spec's guidance
  on very fast inputs.
- Betweenness Centrality's O(V·E) cost is the visible bottleneck here -
  going from V=5,000 to V=10,000 (2x vertices, roughly 2x edges) took the
  time from ~1.7s to ~7.3-8.2s, consistent with the ~4x (V·E) growth
  expected from doubling both factors. This is exactly why the spec caps
  Betweenness Centrality at V=10,000 instead of the full 100,000 scale used
  by Triangle Counting and Connected Components.
- Test-data generation seeds are fixed (std::mt19937 rng(11) for the
  tc*/cc* pairs, rng(22) for bc\_), so re-running the generator tool
  reproduces the exact files these results were measured on.
