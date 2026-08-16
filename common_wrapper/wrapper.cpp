
// g++ -O2 -std=c++17 -static-libgcc -static-libstdc++ -pthread \
//   -Iassignment_01/include \
//   -Iassignment_02/include \
//   assignment_01/src/csr.cpp \
//   assignment_01/src/graph_algorithms.cpp \
//   assignment_02/src/graph_analytics.cpp \
//   common_wrapper/wrapper.cpp \
//   -o wrapper.exe

//
// Build (from the project root

#include "../assignment_01/include/csr.h"              // Assignment 1
#include "../assignment_01/include/graph_algorithms.h" // Assignment 1
#include "../assignment_02/include/graph_analytics.h"  // Assignment 2
#include "../assignment_03/include/maxflow.h"          // Assignment 3
#include "../assignment_03/include/gradient_descent.h" // Assignment 3

#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace common
{

    std::mutex print_mutex;

    void locked_print(const std::string &s)
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << s;
        std::cout.flush();
    }

    std::vector<std::string> list_txt_files(const std::string &dir)
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

    std::vector<std::string> filter_prefix(const std::vector<std::string> &files, const std::string &prefix)
    {
        std::vector<std::string> out;
        for (const auto &f : files)
        {
            std::string base = fs::path(f).filename().string();
            if (base.rfind(prefix, 0) == 0)
                out.push_back(f);
        }
        return out;
    }

    std::string prompt_line(const std::string &msg)
    {
        std::cout << msg;
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    int prompt_int(const std::string &msg, int default_val)
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

}

// ===========================================================================
// Assignment 1: BFS + DFS + SSSP
// ===========================================================================
namespace a1
{

    using common::list_txt_files;
    using common::locked_print;
    using common::prompt_int;
    using common::prompt_line;

    void run_bfs_file(const std::string &path)
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
        out << "[BFS]  " << path << " | V=" << g.V << " source=" << g.source
            << " | visited=" << res.traversal.size() << "/" << g.V << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_dfs_file(const std::string &path)
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
        out << "[DFS]  " << path << " | V=" << g.V << " source=" << g.source
            << " | visited=" << traversal.size() << "/" << g.V << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_sssp_file(const std::string &path)
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
        out << "[SSSP] " << path << " | V=" << g.V << " source=" << g.source
            << (g.weighted ? "" : " (unweighted -> unit weights)")
            << " | reachable=" << reachable << "/" << g.V << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_bfs_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_bfs_file(f);
    }
    void run_dfs_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_dfs_file(f);
    }
    void run_sssp_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_sssp_file(f);
    }

    void menu_bfs()
    {
        std::cout << "\n-- BFS --\n1. Run a single test file\n2. Run all *.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_bfs_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_01/tests/graphs]: ");
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
    void menu_dfs()
    {
        std::cout << "\n-- DFS --\n1. Run a single test file\n2. Run all *.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_dfs_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_01/tests/grpahs]: ");
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
    void menu_sssp()
    {
        std::cout << "\n-- SSSP (Dijkstra) --\n1. Run a single test file\n2. Run all *.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_sssp_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_01/tests/graphs]: ");
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

    void menu_run_all()
    {

        std::string graph_dir = prompt_line("Graph folder (BFS/DFS/SSSP) [default assignment_01/tests/graphs]: ");
        if (graph_dir.empty())
            graph_dir = "assignment_01/tests/graphs";

        auto graph_files = list_txt_files(graph_dir);
        if (graph_files.empty())
        {
            std::cout << "No test files found.\n";
            return;
        }

        std::cout << "Launching BFS/DFS/SSSP concurrently...\n\n";
        auto t0 = std::chrono::high_resolution_clock::now();

        std::thread t1(run_bfs_suite, graph_files);
        std::thread t2(run_dfs_suite, graph_files);
        std::thread t3(run_sssp_suite, graph_files);

        t1.join();
        t2.join();
        t3.join();

        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << "\nAssignment 1 finished. Wall-clock: "
                  << std::chrono::duration<double, std::milli>(t_end - t0).count() << " ms\n";
    }

    void open_submenu()
    {
        while (true)
        {
            std::cout << "\n== Assignment 1: BFS/DFS/SSSP ==\n"
                      << "1. Run BFS\n2. Run DFS\n3. Run SSSP\n"
                      << "4. Run All (this assignment, concurrently)\n5. Back to main menu\n";
            int c = prompt_int("Choose an option: ", 7);
            switch (c)
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
                return;
            default:
                std::cout << "Invalid choice, try again.\n";
                break;
            }
        }
    }

    // Non-interactive: appends this assignment's default-folder suites as
    // threads onto `threads`, for the cross-assignment "Run EVERYTHING" mode.
    void launch_concurrent(std::vector<std::thread> &threads)
    {
        auto graph_files = list_txt_files("assignment_01/tests/graphs");

        threads.emplace_back(run_bfs_suite, graph_files);
        threads.emplace_back(run_dfs_suite, graph_files);
        threads.emplace_back(run_sssp_suite, graph_files);
    }

}

// ===========================================================================
// Assignment 2 - Buddy: Triangle Counting + Betweenness Centrality + Connected Components
// ===========================================================================
namespace a2b
{

    using common::filter_prefix;
    using common::list_txt_files;
    using common::locked_print;
    using common::prompt_int;
    using common::prompt_line;

    void run_tc_file(const std::string &path)
    {
        std::ostringstream out;
        AdjacencyList g;
        if (!read_adjacency_list(path, g))
        {
            out << "[TC]   " << path << " -> ERROR: could not read/parse file\n";
            locked_print(out.str());
            return;
        }
        CSR csr = build_csr(g);
        bool collect_list = (g.V <= 100);
        auto t1 = std::chrono::high_resolution_clock::now();
        TriangleResult res = count_triangles(csr, collect_list);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        out << "[TC]   " << path << " | V=" << g.V << " | triangles=" << res.total_triangles
            << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_bc_file(const std::string &path)
    {
        std::ostringstream out;
        AdjacencyList g;
        if (!read_adjacency_list(path, g))
        {
            out << "[BC]   " << path << " -> ERROR: could not read/parse file\n";
            locked_print(out.str());
            return;
        }
        CSR csr = build_csr(g);
        auto t1 = std::chrono::high_resolution_clock::now();
        CentralityResult res = betweenness_centrality(csr);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        double max_c = 0.0;
        for (double c : res.centrality)
            max_c = std::max(max_c, c);
        std::ostringstream cstr;
        cstr << std::fixed << std::setprecision(2) << max_c;
        out << "[BC]   " << path << " | V=" << g.V << " | max centrality=" << cstr.str()
            << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_cc_file(const std::string &path)
    {
        std::ostringstream out;
        AdjacencyList g;
        if (!read_adjacency_list(path, g))
        {
            out << "[CC]   " << path << " -> ERROR: could not read/parse file\n";
            locked_print(out.str());
            return;
        }
        CSR csr = build_csr(g);
        auto t1 = std::chrono::high_resolution_clock::now();
        ComponentsResult res = connected_components(csr);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        out << "[CC]   " << path << " | V=" << g.V << " | components=" << res.num_components
            << " | time=" << ms << " ms\n";
        locked_print(out.str());
    }
    void run_tc_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_tc_file(f);
    }
    void run_bc_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_bc_file(f);
    }
    void run_cc_suite(const std::vector<std::string> &files)
    {
        for (auto &f : files)
            run_cc_file(f);
    }

    void menu_tc()
    {
        std::cout << "\n-- Triangle Counting --\n1. Run a single test file\n2. Run all tc_*.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_tc_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_02/tests/graphs]: ");
            if (dir.empty())
                dir = "assignment_02/tests/graphs";
            auto files = filter_prefix(list_txt_files(dir), "tc_");
            if (files.empty())
            {
                std::cout << "No tc_*.txt files found in " << dir << "\n";
                return;
            }
            std::cout << "Running " << files.size() << " Triangle Counting test file(s)...\n";
            run_tc_suite(files);
        }
    }
    void menu_bc()
    {
        std::cout << "\n-- Betweenness Centrality --\n1. Run a single test file\n2. Run all bc_*.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_bc_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_02/tests/graphs]: ");
            if (dir.empty())
                dir = "assignment_02/tests/graphs";
            auto files = filter_prefix(list_txt_files(dir), "bc_");
            if (files.empty())
            {
                std::cout << "No bc_*.txt files found in " << dir << "\n";
                return;
            }
            std::cout << "Running " << files.size() << " Betweenness Centrality test file(s)...\n";
            run_bc_suite(files);
        }
    }
    void menu_cc()
    {
        std::cout << "\n-- Connected Components --\n1. Run a single test file\n2. Run all cc_*.txt files in a folder\n3. Back\n";
        int c = prompt_int("Choose: ", 3);
        if (c == 1)
            run_cc_file(prompt_line("Input file path: "));
        else if (c == 2)
        {
            std::string dir = prompt_line("Folder path [default assignment_02/tests/graphs]: ");
            if (dir.empty())
                dir = "assignment_02/tests/graphs";
            auto files = filter_prefix(list_txt_files(dir), "cc_");
            if (files.empty())
            {
                std::cout << "No cc_*.txt files found in " << dir << "\n";
                return;
            }
            std::cout << "Running " << files.size() << " Connected Components test file(s)...\n";
            run_cc_suite(files);
        }
    }
    void menu_run_all()
    {
        std::string dir = prompt_line("Graph folder [default assignment_02/tests/graphs]: ");
        if (dir.empty())
            dir = "assignment_02/tests/graphs";
        auto all_files = list_txt_files(dir);
        auto tc_files = filter_prefix(all_files, "tc_");
        auto bc_files = filter_prefix(all_files, "bc_");
        auto cc_files = filter_prefix(all_files, "cc_");
        if (tc_files.empty() && bc_files.empty() && cc_files.empty())
        {
            std::cout << "No test files found.\n";
            return;
        }

        std::cout << "Launching Triangle Counting, Betweenness Centrality, and Connected Components concurrently...\n\n";
        auto t0 = std::chrono::high_resolution_clock::now();
        std::thread t1(run_tc_suite, tc_files);
        std::thread t2(run_bc_suite, bc_files);
        std::thread t3(run_cc_suite, cc_files);
        t1.join();
        t2.join();
        t3.join();
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << "\nAssignment 2 (Buddy) finished. Wall-clock: "
                  << std::chrono::duration<double, std::milli>(t_end - t0).count() << " ms\n";
    }

    void open_submenu()
    {
        while (true)
        {
            std::cout << "\n== Assignment 2 -Triangle Counting + Betweenness Centrality + Connected Components ==\n"
                      << "1. Run Triangle Counting\n2. Run Betweenness Centrality\n3. Run Connected Components\n"
                      << "4. Run All (this assignment, concurrently)\n5. Back to main menu\n";
            int c = prompt_int("Choose an option: ", 5);
            switch (c)
            {
            case 1:
                menu_tc();
                break;
            case 2:
                menu_bc();
                break;
            case 3:
                menu_cc();
                break;
            case 4:
                menu_run_all();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice, try again.\n";
                break;
            }
        }
    }

    void launch_concurrent(std::vector<std::thread> &threads)
    {
        auto all_files = list_txt_files("assignment_02/tests/graphs");
        threads.emplace_back(run_tc_suite, filter_prefix(all_files, "tc_"));
        threads.emplace_back(run_bc_suite, filter_prefix(all_files, "bc_"));
        threads.emplace_back(run_cc_suite, filter_prefix(all_files, "cc_"));
    }

}

// ===========================================================================
// Helper functions
// ===========================================================================
namespace a3
{
    // ===========================================================================
    // Gradient Descent
    // ===========================================================================
    void run_gd_file(const std::string &path)
    {
        GDInput in;
        std::string err;
        if (!read_gd_input(path, in, err))
        {
            std::cout << path << " -> ERROR: " << err << "\n";
            return;
        }

        auto t1 = std::chrono::high_resolution_clock::now();
        GDResult res = gradient_descent(in);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

        std::cout << "\nAlgorithm: Gradient Descent\n";
        std::cout << "File: " << path << "\n";
        std::cout << "Degree: " << in.degree << "\n";
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Final x: " << res.final_x << "\n";
        std::cout << "Final f(x): " << res.final_fx << "\n";
        std::cout << "Iterations: " << res.iterations << "\n";
        std::cout << "Converged: " << (res.converged ? "true" : "false") << "\n";
        std::cout << "Execution time: " << ms << " ms\n";
    }

    void run_gd_suite(const std::vector<std::string> &files)
    {
        for (const auto &f : files)
            run_gd_file(f);
    }

    // ===========================================================================
    // Maxflow-Mincut
    // ===========================================================================
    void run_maxflow_file(const std::string &path)
    {
        AdjacencyList g;
        int sink;
        std::string err;
        if (!read_maxflow_input(path, g, sink, err))
        {
            std::cout << path << " -> ERROR: " << err << "\n";
            return;
        }

        // Preprocessing: adjacency-list -> CSR. NOT part of the timed algorithm.
        CSR csr = build_csr(g);

        auto t1 = std::chrono::high_resolution_clock::now();
        MaxflowResult res = dinic_maxflow(csr, g.source, sink);
        auto t2 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

        std::cout << "\nAlgorithm: Maxflow-Mincut\n";
        std::cout << "File: " << path << "\n";
        std::cout << "Source: " << g.source << "\n";
        std::cout << "Sink: " << sink << "\n";
        std::cout << "Maximum flow: " << res.max_flow << "\n";
        std::cout << "Minimum cut capacity: " << res.max_flow << "\n";
        if (g.V < 1000)
        {
            std::cout << "Source side:";
            for (int v : res.source_side)
                std::cout << " " << v;
            std::cout << "\nSink side:";
            for (int v : res.sink_side)
                std::cout << " " << v;
            std::cout << "\nCut edges:\n";
            for (const auto &[u, v, c] : res.cut_edges)
                std::cout << u << " " << v << " " << c << "\n";
        }
        std::cout << "Execution time: " << ms << " ms\n";
    }

    void run_maxflow_suite(const std::vector<std::string> &files)
    {
        for (const auto &f : files)
            run_maxflow_file(f);
    }

    // ===========================================================================
    // Menus
    // ===========================================================================
    void menu_gd()
    {
        while (true)
        {
            std::cout << "\n-- Gradient Descent --\n"
                      << "1. Run a single test file\n"
                      << "2. Run all gd_*.txt files in a folder\n"
                      << "3. Back\n";
            int c = common::prompt_int("Choose: ", 3);
            if (c == 1)
            {
                run_gd_file(common::prompt_line("Input file path: "));
            }
            else if (c == 2)
            {
                std::string dir = common::prompt_line("Folder path [default tests]: ");
                if (dir.empty())
                    dir = "assignment_03/tests";
                auto files = common::filter_prefix(common::list_txt_files(dir), "gd_");
                if (files.empty())
                {
                    std::cout << "No gd_*.txt files found in " << dir << "\n";
                    continue;
                }
                std::cout << "Running " << files.size() << " Gradient Descent test file(s)...\n";
                run_gd_suite(files);
            }
            else
            {
                return;
            }
        }
    }

    void menu_maxflow()
    {
        while (true)
        {
            std::cout << "\n-- Maxflow-Mincut --\n"
                      << "1. Run a single test file\n"
                      << "2. Run all maxflow_*.txt files in a folder\n"
                      << "3. Back\n";
            int c = common::prompt_int("Choose: ", 3);
            if (c == 1)
            {
                run_maxflow_file(common::prompt_line("Input file path: "));
            }
            else if (c == 2)
            {
                std::string dir = common::prompt_line("Folder path [default tests]: ");
                if (dir.empty())
                    dir = "assignment_03/tests";
                auto files = common::filter_prefix(common::list_txt_files(dir), "maxflow_");
                if (files.empty())
                {
                    std::cout << "No maxflow_*.txt files found in " << dir << "\n";
                    continue;
                }
                std::cout << "Running " << files.size() << " Maxflow-Mincut test file(s)...\n";
                run_maxflow_suite(files);
            }
            else
            {
                return;
            }
        }
    }

    void main_menu()
    {
        while (true)
        {
            std::cout << "\n== Assignment 3 (Buddy): Gradient Descent + Maxflow-Mincut ==\n"
                      << "1. Gradient Descent\n"
                      << "2. Maxflow-Mincut\n"
                      << "3. Exit\n";
            int c = common::prompt_int("Choose: ", 3);
            if (c == 1)
                menu_gd();
            else if (c == 2)
                menu_maxflow();
            else
                return;
        }
    }

    void launch_concurrent(std::vector<std::thread> &threads)
    {
        auto files = common::list_txt_files("tests");
        threads.emplace_back(run_gd_suite, common::filter_prefix(files, "gd_"));
        threads.emplace_back(run_maxflow_suite, common::filter_prefix(files, "maxflow_"));
    }
}

// ===========================================================================
// Registry + main menu
// ===========================================================================

struct AssignmentModule
{
    std::string label;
    std::function<void()> open_submenu;
    std::function<void(std::vector<std::thread> &)> launch_concurrent;
};

int main()
{
    std::cout << "==================================================\n"
              << " CS509 - Master Task Runner\n"
              << "==================================================\n";

    std::vector<AssignmentModule> modules = {
        {"Assignment 1 (BFS + DFS + SSSP)", a1::open_submenu, a1::launch_concurrent},
        {"Assignment 2 -(Triangle Counting + Betweenness Centrality + Connected Components)", a2b::open_submenu, a2b::launch_concurrent},
        {"Assignment 3 (Gradient Descent + Maxflow-Mincut)", a3::main_menu, a3::launch_concurrent},
    };

    while (true)
    {
        std::cout << "\nWhich assignment would you like to run?\n";
        for (size_t i = 0; i < modules.size(); ++i)
        {
            std::cout << (i + 1) << ". " << modules[i].label << "\n";
        }
        int run_all_choice = static_cast<int>(modules.size()) + 1;
        int exit_choice = static_cast<int>(modules.size()) + 2;
        std::cout << run_all_choice << ". Run EVERYTHING (every assignment)\n";
        std::cout << exit_choice << ". Exit\n";

        int choice = common::prompt_int("Choose an option: ", exit_choice);

        if (choice >= 1 && choice <= static_cast<int>(modules.size()))
        {
            modules[choice - 1].open_submenu();
        }
        else if (choice == run_all_choice)
        {
            std::cout << "Launching every assignment's default test suite concurrently...\n\n";
            std::vector<std::thread> threads;
            auto t0 = std::chrono::high_resolution_clock::now();
            for (auto &m : modules)
                m.launch_concurrent(threads);
            for (auto &t : threads)
                t.join();
            auto t_end = std::chrono::high_resolution_clock::now();
            std::cout << "\nEVERYTHING finished (" << threads.size() << " suites across "
                      << modules.size() << " assignments). Total wall-clock: "
                      << std::chrono::duration<double, std::milli>(t_end - t0).count() << " ms\n"
                      << "\n";
        }
        else if (choice == exit_choice)
        {
            std::cout << "Goodbye.\n";
            return 0;
        }
        else
        {
            std::cout << "Invalid choice, try again.\n";
        }
    }
}