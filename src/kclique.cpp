#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Graph {
    int n = 0;
    int k = 0;
    vector<vector<int>> adj;
    vector<int> degree;
    // Adjacency Matrix for O(1) lookups
    vector<vector<bool>> adjMat; 
};

// --- Helpers ---

string clean_string(string s) {
    s.erase(remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(remove(s.begin(), s.end(), ':'), s.end());
    return s;
}

// --- Algorithm 1: Bare Bones Backtracking (The Control) ---
bool solve_backtracking(Graph& g, vector<int>& current_clique, int start_node) {
    if (current_clique.size() == g.k) return true;

    for (int i = start_node; i < g.n; ++i) {
        // Check connection to all existing clique members
        bool connected = true;
        for (int v : current_clique) {
            if (!g.adjMat[i][v]) {
                connected = false;
                break;
            }
        }
        
        if (connected) {
            current_clique.push_back(i);
            if (solve_backtracking(g, current_clique, i + 1)) return true;
            current_clique.pop_back();
        }
    }
    return false;
}

// --- Smart Forward Checking Logic (Used by Alg 2 and 3) ---
bool solve_smart(Graph& g, vector<int>& current_clique, vector<int>& candidates) {
    if (current_clique.size() == g.k) return true;

    // PRUNING 1: If we don't have enough candidates left to reach k, stop.
    if (current_clique.size() + candidates.size() < g.k) return false;

    for (int i = 0; i < candidates.size(); ++i) {
        int u = candidates[i];

        // PRUNING 2: Bound check inside the loop
        // If (current size) + (u) + (all candidates after u) < k, we can't win.
        if (current_clique.size() + (candidates.size() - i) < g.k) return false;

        // Valid candidates for next step must be:
        // 1. In the current candidate list (after u)
        // 2. Connected to u
        vector<int> next_candidates;
        next_candidates.reserve(candidates.size()); 

        for (int j = i + 1; j < candidates.size(); ++j) {
            int v = candidates[j];
            if (g.adjMat[u][v]) {
                next_candidates.push_back(v);
            }
        }

        // Check if this branch is viable before recurring
        if (current_clique.size() + 1 + next_candidates.size() >= g.k) {
            current_clique.push_back(u);
            if (solve_smart(g, current_clique, next_candidates)) return true;
            current_clique.pop_back();
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    // 1. Setup
    if (argc < 2) { cerr << "Usage: " << argv[0] << " <input_file>" << endl; return 1; }
    ifstream infile(argv[1]);
    if (!infile.is_open()) { cerr << "Error opening file." << endl; return 1; }

    Graph g;
    string line;
    
    // 2. Robust Parsing
    while (getline(infile, line)) {
        if (line.empty()) continue;
        if (line.back() == '\r') line.pop_back(); // Fix Windows Line Endings

        if (line[0] == '#') {
            stringstream ss(line);
            string dummy, keyStr; 
            ss >> dummy;   // Reads '#'
            ss >> keyStr;  // Reads 'n_nodes:' or 'k:'
            
            string key = clean_string(keyStr);
            if (key == "n_nodes") ss >> g.n;
            else if (key == "k") ss >> g.k;
        } else {
            // Data line found (0 1)
            stringstream ss(line);
            int u, v;
            if (ss >> u >> v) {
                // Initialize if this is the first data line
                if(g.adj.empty()) {
                    if (g.n == 0) { cerr << "Error: n_nodes not found in header." << endl; return 1; }
                    g.adj.resize(g.n);
                    g.degree.resize(g.n, 0);
                    // Using vector<bool> is space efficient (1 bit per entry)
                    g.adjMat.assign(g.n, vector<bool>(g.n, false));
                }
                
                if (u < g.n && v < g.n) {
                    g.adj[u].push_back(v); g.adj[v].push_back(u);
                    g.adjMat[u][v] = g.adjMat[v][u] = true;
                    g.degree[u]++; g.degree[v]++;
                }
                break; // Break header loop, move to fast bulk read
            }
        }
    }

    // 3. Fast Bulk Read
    int u, v;
    while (infile >> u >> v) {
        if (u < g.n && v < g.n) {
            g.adj[u].push_back(v); g.adj[v].push_back(u);
            g.adjMat[u][v] = g.adjMat[v][u] = true;
            g.degree[u]++; g.degree[v]++;
        }
    }
    infile.close();

    cout << "Graph Loaded: " << g.n << " nodes, Target k=" << g.k << endl;
    if (g.n == 0) { cout << "Error: Graph has 0 nodes." << endl; return 1; }

    cout << "-------------------------------------------" << endl;

    // --- Benchmark 1: Bare Backtracking ---
    {
        vector<int> clique;
        auto start = high_resolution_clock::now();
        bool found = solve_backtracking(g, clique, 0);
        auto stop = high_resolution_clock::now();
        auto d = duration_cast<microseconds>(stop - start).count();
        cout << left << setw(25) << "[1] Bare Backtracking" << "| Time: " << setw(8) << d << " us | " << (found?"Found":"Fail") << endl;
    }

    // --- Benchmark 2: Degree Heuristic (Sorted Smart Pruning) ---
    {
        vector<int> clique;
        auto start = high_resolution_clock::now();
        
        vector<int> candidates(g.n);
        for(int i=0; i<g.n; i++) candidates[i] = i;
        sort(candidates.begin(), candidates.end(), [&](int a, int b) {
            return g.degree[a] > g.degree[b];
        });

        bool found = solve_smart(g, clique, candidates);
        auto stop = high_resolution_clock::now();
        auto d = duration_cast<microseconds>(stop - start).count();
        cout << left << setw(25) << "[2] Degree Heuristic" << "| Time: " << setw(8) << d << " us | " << (found?"Found":"Fail") << endl;
    }

    // --- Benchmark 3: Smart Pruning (Unsorted) ---
    {
        vector<int> clique;
        auto start = high_resolution_clock::now();
        
        vector<int> candidates(g.n);
        for(int i=0; i<g.n; i++) candidates[i] = i;

        bool found = solve_smart(g, clique, candidates);
        auto stop = high_resolution_clock::now();
        auto d = duration_cast<microseconds>(stop - start).count();
        cout << left << setw(25) << "[3] Smart Pruning" << "| Time: " << setw(8) << d << " us | " << (found?"Found":"Fail") << endl;
    }

    return 0;
}