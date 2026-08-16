# CS509 - Assignment 3 (Buddy) - Report

**Task:** Gradient Descent and Maxflow-Mincut
**Work mode:** Pair
**Maxflow algorithm used:** Dinic's algorithm 
## 9.2 Gradient Descent Results Table

| File | Degree | x0 | Rate | Tol. | Max Iter. | Exp. x* | Exp. f(x*) | Actual x | Actual f(x) | Iter. / Time | Status |
|---|---|---|---|---|---|---|---|---|---|---|---|
| gd_01.txt | 2 | 0 | 0.10 | 1e-6 | 5,000 | 3 | 0 | 3.000000 | 0.000000 | 70 / 0.000739 ms | Pass |
| gd_02.txt | 4 | 2 | 0.02 | 1e-6 | 10,000 | 0 | 0 | 0.000000 | 0.000000 | 180 / 0.001689 ms | Pass |
| gd_03.txt | 6 | 2 | 0.02 | 1e-6 | 20,000 | 0 | 0 | 0.000000 | 0.000000 | 349 / 0.003980 ms | Pass |
| gd_04.txt | 8 | 2 | 0.01 | 1e-8 | 50,000 | 0 | 0 | 0.000000 | 0.000000 | 948 / 0.013069 ms | Pass |
| gd_05.txt | 10 | 2 | 0.005 | 1e-10 | 100,000 | 0 | 0 | 0.000000 | 0.000000 | 2,364 / 0.038255 ms | Pass |

All five tests converged (`Converged: true`) and matched the expected minimum x* to six decimal
places.
## 9.3 Maxflow-Mincut Results Table

| File | V | E | Source | Sink | Exp. Flow | Actual Flow | Cut Capacity | Time | Status |
|---|---|---|---|---|---|---|---|---|---|
| maxflow_10.txt | 10 | 49 | 0 | 9 | n/a | 84 | 84 | 0.010436 ms | Pass |
| maxflow_100.txt | 100 | 499 | 0 | 99 | n/a | 127 | 127 | 0.127046 ms | Pass |
| maxflow_1000.txt | 1,000 | 4,999 | 0 | 999 | n/a | 58 | 58 | 5.45451 ms | Pass |
| maxflow_10000.txt | 10,000 | 49,999 | 0 | 9,999 | n/a | 70 | 70 | 12.4857 ms | Pass |
| maxflow_50000.txt | 50,000 | 249,999 | 0 | 49,999 | n/a | 42 | 42 | 47.9254 ms | Pass |
| maxflow_100000.txt (optional) | 100,000 | 499,999 | 0 | 99,999 | n/a | 104 | 104 | 197.262 ms | Pass |

"Exp. Flow" is n/a because these are randomly generated capacity graphs with no independently
known optimum; correctness is instead verified by "Actual Flow" equaling "Cut Capacity" on every
input, which is exactly the max-flow/min-cut equality required by Section 2.4. This was also
confirmed against the assignment's own worked example (Section 7.2/7.3): running our driver on
that graph produces maximum flow = 23, source side {0,1,2,4}, sink side {3,5} — matching the
spec exactly.

## Test Graphs

Maxflow graphs were generated with `tools/generate_maxflow_graph.cpp`: a source(0)→sink(V-1) path
is guaranteed by routing through a shuffled ordering of intermediate vertices, then extra random
directed edges are added to reach an average out-degree of ~4 (sparse, per Section 4.2). Positive
integer capacities, seed = 65. The generator takes only an output directory and writes all six
sizes (10, 100, 1,000, 10,000, 50,000, and the optional 100,000) in one run. Gradient Descent test
files were generated with `tools/generate_gd_tests.cpp` and match Section 4.3's required functions
and parameters exactly.

## Notes

- **Timing:** For Maxflow-Mincut, `build_csr()` runs before the timer starts (preprocessing).
  `dinic_maxflow()` builds the residual network from the CSR and extracts the min-cut inside the
  same timed call, per Section 8.
- **Correctness:** Maximum flow equals minimum cut capacity on every test file; Gradient Descent
  converges to the expected minimum on every required test.
- **Failures:** No test failed to complete; no core-dumps, out-of-memory, or excessive runtime were
  observed at any required size, nor at the optional V = 100,000 Maxflow size (completed in
  ~197 ms).
