// Build:
//   g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -Iassignment_01/include assignment_01/src/csr.cpp assignment_01/src/graph_algorithms.cpp common_wrapper/wrapper.cpp -pthread -o wrapper
// Run:
//   ./wrapper

#include "../assignment_01/include/csr.h"
#include "../assignment_01/include/graph_algorithms.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

static std::mutex g_print_mutex; // keeps concurrent output from interleaving mid-line


static void locked_print(const std::string &s)
{
    std::lock_guard<std::mutex> lock(g_print_mutex);
    std::cout << s;
    std::cout.flush();
}

static std::vector<std::string> list_txt_files(const std::string &dir)
{
    std::vector<std::string> files;
    if (!fs::exists(dir) || !fs::is_directory(dir))
        return files;
    for (const auto &entry : fs::directory_iterator(dir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".txt")
        {
            files.push_back(entry.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

static std::string prompt_line(const std::string &msg)
{
    std::cout << msg;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static int prompt_int(const std::string &msg, int default_val)
{
    std::string line = prompt_line(msg);
    if (line.empty())
        return default_val;
    try
    {
        return std::stoi(line);
    }
    catch (...)
    {
        return default_val;
    }
}



static void run_bfs_file(const std::string &path)
{
    std::ostringstream out;

    AdjacencyList g;
    if (!read_adjacency_list(path, g))
    {
        out << "[BFS]  " << path << " -> ERROR: could not read/parse file\n";
        locked_print(out.str());
        return;
    }
    CSR csr = build_csr(g); 

    auto t1 = std::chrono::high_resolution_clock::now();
    BFSResult res = bfs_csr(csr, g.source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    out << "[BFS]  " << path
        << " | V=" << g.V << " source=" << g.source
        << " | visited=" << res.traversal.size() << "/" << g.V
        << " | time=" << ms << " ms\n";
    locked_print(out.str());
}

static void run_dfs_file(const std::string &path)
{
    std::ostringstream out;

    AdjacencyList g;
    if (!read_adjacency_list(path, g))
    {
        out << "[DFS]  " << path << " -> ERROR: could not read/parse file\n";
        locked_print(out.str());
        return;
    }
    CSR csr = build_csr(g); 

    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<int> traversal = dfs_csr(csr, g.source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    out << "[DFS]  " << path
        << " | V=" << g.V << " source=" << g.source
        << " | visited=" << traversal.size() << "/" << g.V
        << " | time=" << ms << " ms\n";
    locked_print(out.str());
}

static void run_sssp_file(const std::string &path)
{
    std::ostringstream out;

    AdjacencyList g;
    if (!read_adjacency_list(path, g))
    {
        out << "[SSSP] " << path << " -> ERROR: could not read/parse file\n";
        locked_print(out.str());
        return;
    }
    CSR csr = build_csr(g);

    auto t1 = std::chrono::high_resolution_clock::now();
    SSSPResult res = sssp_dijkstra_csr(csr, g.source);
    auto t2 = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    long long reachable = 0;
    for (double d : res.distance)
        if (!std::isinf(d))
            ++reachable;

    out << "[SSSP] " << path
        << " | V=" << g.V << " source=" << g.source
        << (g.weighted ? "" : " (unweighted -> unit weights)")
        << " | reachable=" << reachable << "/" << g.V
        << " | time=" << ms << " ms\n";
    locked_print(out.str());
}

static void run_bfs_suite(const std::vector<std::string> &files)
{
    for (const auto &f : files)
        run_bfs_file(f);
}
static void run_dfs_suite(const std::vector<std::string> &files)
{
    for (const auto &f : files)
        run_dfs_file(f);
}
static void run_sssp_suite(const std::vector<std::string> &files)
{
    for (const auto &f : files)
        run_sssp_file(f);
}

static void menu_bfs()
{
    std::cout << "\n-- BFS --\n"
              << "1. Run a single test file\n"
              << "2. Run all *.txt files in a folder\n"
              << "3. Back\n";
    int choice = prompt_int("Choose: ", 3);

    if (choice == 1)
    {
        std::string path = prompt_line("Input file path: ");
        run_bfs_file(path);
    }
    else if (choice == 2)
    {
        std::string dir = prompt_line("Folder path [default tests/graphs]: ");
        if (dir.empty())
            dir = "assignment_01/tests/graphs";
        auto files = list_txt_files(dir);
        if (files.empty())
        {
            std::cout << "No .txt files found in " << dir << "\n";
            return;
        }
        std::cout << "Running " << files.size() << " BFS test file(s)...\n";
        run_bfs_suite(files);
    }
}

static void menu_dfs()
{
    std::cout << "\n-- DFS --\n"
              << "1. Run a single test file\n"
              << "2. Run all *.txt files in a folder\n"
              << "3. Back\n";
    int choice = prompt_int("Choose: ", 3);

    if (choice == 1)
    {
        std::string path = prompt_line("Input file path: ");
        run_dfs_file(path);
    }
    else if (choice == 2)
    {
        std::string dir = prompt_line("Folder path [default tests/graphs]: ");
        if (dir.empty())
            dir = "assignment_01/tests/graphs";
        auto files = list_txt_files(dir);
        if (files.empty())
        {
            std::cout << "No .txt files found in " << dir << "\n";
            return;
        }
        std::cout << "Running " << files.size() << " DFS test file(s)...\n";
        run_dfs_suite(files);
    }
}

static void menu_sssp()
{
    std::cout << "\n-- SSSP (Dijkstra, positive weights) --\n"
              << "1. Run a single test file\n"
              << "2. Run all *.txt files in a folder\n"
              << "3. Back\n";
    int choice = prompt_int("Choose: ", 3);

    if (choice == 1)
    {
        std::string path = prompt_line("Input file path: ");
        run_sssp_file(path);
    }
    else if (choice == 2)
    {
        std::string dir = prompt_line("Folder path [default tests/graphs]: ");
        if (dir.empty())
            dir = "assignment_01/tests/graphs";
        auto files = list_txt_files(dir);
        if (files.empty())
        {
            std::cout << "No .txt files found in " << dir << "\n";
            return;
        }
        std::cout << "Running " << files.size() << " SSSP test file(s)...\n";
        run_sssp_suite(files);
    }
}

static void menu_run_all()
{

    std::string graph_dir = prompt_line("Graph folder (used for BFS/DFS/SSSP) [default tests/graphs]: ");
    if (graph_dir.empty())
        graph_dir = "assignment_01/tests/graphs";

    auto graph_files = list_txt_files(graph_dir);

    if (graph_files.empty())
    {
        std::cout << "No test files found in either folder.\n";
        return;
    }

    std::cout << "Launching BFS, DFS,\n"
              << "and SSSP (" << graph_files.size() << " graph files each) concurrently...\n\n";

    auto wall_start = std::chrono::high_resolution_clock::now();

    std::thread bfs_thread(run_bfs_suite, graph_files);
    std::thread dfs_thread(run_dfs_suite, graph_files);
    std::thread sssp_thread(run_sssp_suite, graph_files);

    bfs_thread.join();
    dfs_thread.join();
    sssp_thread.join();

    auto wall_end = std::chrono::high_resolution_clock::now();
    double wall_ms = std::chrono::duration<double, std::milli>(wall_end - wall_start).count();

    std::cout << "\nAll five suites finished. Total wall-clock time: " << wall_ms << " ms\n";
}

// ---------------------------------------------------------------------
// Main menu loop
// ---------------------------------------------------------------------

int main()
{
    std::cout << "======================================\n"
              << " CS509 Assignment 1 - Task Runner\n"
              << "======================================\n";

    while (true)
    {
        std::cout << "\nMain menu:\n"
                  << "1. Run BFS\n"
                  << "2. Run DFS\n"
                  << "3. Run SSSP\n"
                  << "4. Run All (BFS + DFS + SSSP at the same time)\n"
                  << "5. Exit\n";
        int choice = prompt_int("Choose an option: ", 5);

        switch (choice)
        {

        case 1:
            menu_bfs();
            break;
        case 2:
            menu_dfs();
            break;
        case 3:
            menu_sssp();
            break;
        case 4:
            menu_run_all();
            break;
        case 5:
            std::cout << "Goodbye.\n";
            return 0;
        default:
            std::cout << "Invalid choice, try again.\n";
            break;
        }
    }
}
