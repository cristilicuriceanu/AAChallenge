#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// --- Graph Structure ---
struct Graph {
    int n = 0; // Number of nodes
    int m = 0; // Number of edges
    vector<vector<bool>> adjMat; // Adjacency Matrix
    vector<int> degrees;
};

// --- Helper: Read Graph ---
Graph readGraph(const string& filename) {
    ifstream f(filename);
    Graph g;
    if (!f.is_open()) return g;

    f >> g.n >> g.m;
    g.adjMat.assign(g.n, vector<bool>(g.n, false));
    g.degrees.assign(g.n, 0);

    for (int i = 0; i < g.m; ++i) {
        int u, v;
        f >> u >> v;
        if (u < g.n && v < g.n) {
            g.adjMat[u][v] = g.adjMat[v][u] = true;
            g.degrees[u]++;
            g.degrees[v]++;
        }
    }
    return g;
}

// --- Algorithm 1: Exact Backtracking (Branch & Bound) ---
// Returns the SIZE of the max clique found
void expand(const Graph& g, vector<int>& candidates, vector<int>& current_clique, int& max_size) {
    if (candidates.empty()) {
        if (current_clique.size() > max_size) {
            max_size = current_clique.size();
        }
        return;
    }

    // PRUNING: If current + candidates <= max_found, we can't beat the record. Stop.
    if (current_clique.size() + candidates.size() <= max_size) return;

    while (!candidates.empty()) {
        int v = candidates.back();
        candidates.pop_back();

        // New candidates must be connected to v AND be in the old candidates list
        vector<int> new_candidates;
        for (int u : candidates) {
            if (g.adjMat[v][u]) {
                new_candidates.push_back(u);
            }
        }

        current_clique.push_back(v);
        expand(g, new_candidates, current_clique, max_size);
        current_clique.pop_back();
    }
}

int solve_exact(const Graph& g) {
    int max_size = 0;
    vector<int> candidates(g.n);
    for (int i = 0; i < g.n; ++i) candidates[i] = i;
    
    vector<int> current_clique;
    expand(g, candidates, current_clique, max_size);
    return max_size;
}

// --- Algorithm 2: Greedy Heuristic (Degree Based) ---
// Sorts nodes by degree and greedily adds them. Very fast.
int solve_greedy_degree(const Graph& g) {
    vector<int> nodes(g.n);
    for(int i=0; i<g.n; ++i) nodes[i] = i;

    // Sort by degree descending
    sort(nodes.begin(), nodes.end(), [&](int a, int b) {
        return g.degrees[a] > g.degrees[b];
    });

    vector<int> clique;
    for (int u : nodes) {
        bool can_add = true;
        for (int v : clique) {
            if (!g.adjMat[u][v]) {
                can_add = false;
                break;
            }
        }
        if (can_add) clique.push_back(u);
    }
    return clique.size();
}

// --- Algorithm 3: Randomized Heuristic ---
// Shuffles nodes and builds a clique. Repeats 'iters' times.
int solve_randomized(const Graph& g, int iters = 100) {
    vector<int> nodes(g.n);
    for(int i=0; i<g.n; ++i) nodes[i] = i;

    // Use a random device
    random_device rd;
    mt19937 g_rng(rd());

    int best_size = 0;

    for(int k=0; k<iters; ++k) {
        shuffle(nodes.begin(), nodes.end(), g_rng);
        vector<int> clique;
        for (int u : nodes) {
            bool can_add = true;
            for (int v : clique) {
                if (!g.adjMat[u][v]) {
                    can_add = false;
                    break;
                }
            }
            if (can_add) clique.push_back(u);
        }
        if (clique.size() > best_size) best_size = clique.size();
    }
    return best_size;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./kclique <input_file>" << endl;
        return 1;
    }

    Graph g = readGraph(argv[1]);
    if (g.n == 0) return 0; // Empty or error

    // Print Header for Parsing
    cout << "RESULT_START" << endl;

    // 1. Run Exact
    auto start = high_resolution_clock::now();
    int res_exact = solve_exact(g);
    auto stop = high_resolution_clock::now();
    auto dur_exact = duration_cast<microseconds>(stop - start).count();
    cout << "Exact," << res_exact << "," << dur_exact << endl;

    // 2. Run Greedy Degree
    start = high_resolution_clock::now();
    int res_greedy = solve_greedy_degree(g);
    stop = high_resolution_clock::now();
    auto dur_greedy = duration_cast<microseconds>(stop - start).count();
    cout << "GreedyDegree," << res_greedy << "," << dur_greedy << endl;

    // 3. Run Randomized
    start = high_resolution_clock::now();
    int res_rand = solve_randomized(g);
    stop = high_resolution_clock::now();
    auto dur_rand = duration_cast<microseconds>(stop - start).count();
    cout << "Randomized," << res_rand << "," << dur_rand << endl;

    cout << "RESULT_END" << endl;

    return 0;
}
