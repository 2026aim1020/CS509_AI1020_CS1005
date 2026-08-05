#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -Iinclude src/csr.cpp src/graph_algorithms.cpp src/bfs_driver.cpp -o bfs_driver
g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -Iinclude src/csr.cpp src/graph_algorithms.cpp src/dfs_driver.cpp -o dfs_driver
g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -Iinclude src/csr.cpp src/graph_algorithms.cpp src/sssp_driver.cpp -o sssp_driver

echo "Build complete:./bfs_driver, ./dfs_driver, ./sssp_driver, "
