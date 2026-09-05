# CS509 Assignment 4 — Buddy Task
K-Means Clustering + FastMap

## Layout

```
buddy/
├── include/            kmeans.h, fastmap.h
├── src/                kmeans.cpp, fastmap.cpp
├── tools/               wrapper.cpp (driver), generate_kmeans_data.cpp, generate_fastmap_data.cpp
├── tests/               km_*.txt, fm_*.txt
└── README.md
```

## Build

```bash
g++ -O2 -std=c++17 -Iinclude src/kmeans.cpp src/fastmap.cpp \
    tools/wrapper.cpp -o wrapper.exe
```

Generators (optional, only needed to regenerate/resize test data):

```bash
g++ -O2 -std=c++17 tools/generate_kmeans_data.cpp  -o gen_kmeans.exe
g++ -O2 -std=c++17 tools/generate_fastmap_data.cpp -o gen_fastmap.exe
# ./gen_kmeans.exe  <N> <D> <K> <output_path> [seed] [max_iter] [tolerance]
# ./gen_fastmap.exe <N> <K> <output_path> [seed] [underlying_dims]
```

## Run

```bash
./wrapper.exe
```

Menu-driven: run a single test file, run all `km_*.txt` / `fm_*.txt`
files in a folder (default `tests/`), or run both suites concurrently.

## Timing rule

For K-Means, both the assignment and update steps across all iterations
are timed. For FastMap, pivot selection, projection, and distance
deflation across all k dimensions are timed. File reading and input
validation happen before the timer starts.

## Input validation

- **K-Means**: rejects non-positive N, D, or K, K > N, a non-positive
  tolerance, or a non-positive `MAX_ITERATIONS`.
- **FastMap**: rejects a non-square/malformed matrix, a non-symmetric
  matrix, a non-zero diagonal entry, a negative distance, or a target
  dimensionality k that is non-positive or ≥ N.

## Algorithm notes

- **K-Means**: Lloyd's algorithm. Initial centroids are the first K
  input points, in input order, for reproducibility. If a cluster
  becomes empty during an update, its previous centroid is kept for
  that iteration. Stops when no point changes cluster or the maximum
  centroid shift ≤ `TOLERANCE`, or `MAX_ITERATIONS` is reached.
- **FastMap**: for each of the k target dimensions, pivots are chosen by
  a random start followed by two rounds of "move to the object farthest
  from the current pivot" (approximates the farthest pair without an
  O(N²) exhaustive search). Each object is projected onto the pivot line
  via the law of cosines, then the remaining pairwise distances are
  deflated to remove the contribution already captured before the next
  dimension is computed. Because pivot selection is randomized, exact
  coordinates can differ slightly between correct implementations.

## Results — K-Means Clustering

| File | N | D | K | Max Iter. | Actual Iter. | WCSS | Time | Status |
|---|---|---|---|---|---|---|---|---|
| km_01.txt | 100 | 2 | 3 | 300 | 3 | 2066.86 | 0.0049 ms | Pass |
| km_02.txt | 1,000 | 2 | 5 | 300 | 2 | 18343.4 | 0.022 ms | Pass |
| km_03.txt | 10,000 | 5 | 8 | 300 | 2 | 449,260 | 0.566 ms | Pass |
| km_04.txt | 100,000 | 5 | 10 | 300 | 2 | 4,504,670 | 8.56 ms | Pass |

(`km_example_spec.txt` holds the 6-point worked example from the
assignment sheet.)

## Results — FastMap

| File | N | Target k | Pivots (per dim) | Avg. Distance Error (sampled) | Time | Status |
|---|---|---|---|---|---|---|
| fm_01.txt | 10 | 2 | (3,4), (6,8) | ~0 (synthetic, low-noise) | 0.0064 ms | Pass |
| fm_02.txt | 100 | 2 | (54,33), (91,70) | see run output | 0.056 ms | Pass |
| fm_03.txt | 1,000 | 3 | (403,525), (333,601), (519,33) | see run output | 10.3 ms | Pass |
| fm_04.txt | 10,000 | 3 | — | — | — | Not generated |

`fm_04` (N = 10,000, full N×N matrix) was intentionally not generated:
the plain-text distance matrix format means the file size grows with
N², and at N = 10,000 that is already ~100,000,000 numbers (~800 MB as
text). This is documented rather than hidden, per the assignment's
"document observed behavior for the largest sizes" allowance. Run
`gen_fastmap.exe 10000 3 tests/fm_04.txt` on a machine with sufficient
disk space to produce it — the algorithm itself places no N limit
beyond available memory and O(N²) time per dimension.

## Test-data generation

- K-Means points: K well-separated Gaussian blobs in D dimensions, N
  points split evenly across the K blobs.
- FastMap distances: N random points placed in a small internal
  Euclidean space, with pairwise Euclidean distance written out. This
  guarantees a symmetric, zero-diagonal matrix that satisfies the
  triangle inequality.
