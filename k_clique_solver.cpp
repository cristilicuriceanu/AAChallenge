#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <queue>
#include <cmath>
#include <random>
#include <climits>

using namespace std;
using namespace std::chrono;

// ============= CLASA GRAPH =============
class Graph {
private:
    int n;
    vector<vector<bool>> adj_matrix;
    vector<set<int>> adj_list;
    vector<int> degrees;

public:
    Graph(int nodes) : n(nodes), adj_matrix(nodes, vector<bool>(nodes, false)), 
                       adj_list(nodes), degrees(nodes, 0) {}
    
    void addEdge(int u, int v) {
        if (u < 0 || v < 0 || u >= n || v >= n) {
            cerr << "Warning: Invalid edge (" << u << ", " << v << ")\n";
            return;
        }
        if (u == v || adj_matrix[u][v]) return;
        adj_matrix[u][v] = true;
        adj_matrix[v][u] = true;
        adj_list[u].insert(v);
        adj_list[v].insert(u);
        degrees[u]++;
        degrees[v]++;
    }
    
    bool hasEdge(int u, int v) const {
        return adj_matrix[u][v];
    }
    
    int getNodes() const { return n; }
    
    const set<int>& getNeighbors(int u) const {
        return adj_list[u];
    }
    
    int getDegree(int u) const {
        return degrees[u];
    }
    
    // Verifică dacă un set de noduri formează o clică
    bool isClique(const vector<int>& nodes) const {
        for (size_t i = 0; i < nodes.size(); i++) {
            for (size_t j = i + 1; j < nodes.size(); j++) {
                if (! adj_matrix[nodes[i]][nodes[j]]) {
                    return false;
                }
            }
        }
        return true;
    }
    
    void printGraph() const {
        cout << "Graf cu " << n << " noduri și " << getEdgeCount() << " muchii\n";
    }
    
    int getEdgeCount() const {
        int count = 0;
        for (int i = 0; i < n; i++) {
            count += degrees[i];
        }
        return count / 2;
    }
};

// ============= STRUCTURĂ PENTRU REZULTATE =============
struct SolutionResult {
    vector<int> clique;
    bool found;
    long long time_microseconds;
    long long nodes_explored;
    string algorithm_name;
    
    void print() const {
        cout << "\n=== " << algorithm_name << " ===\n";
        if (found && ! clique.empty()) {
            cout << "✓ k-Clique găsit:  { ";
            for (int node : clique) {
                cout << node << " ";
            }
            cout << "}\n";
            cout << "Mărime: " << clique.size() << "\n";
        } else {
            cout << "✗ Nu s-a găsit k-clique\n";
        }
        cout << "Timp execuție: " << time_microseconds << " μs (" 
             << time_microseconds / 1000.0 << " ms)\n";
        if (nodes_explored > 0) {
            cout << "Noduri explorate: " << nodes_explored << "\n";
        }
    }
};

// ============= ALGORITM 1: BACKTRACKING (EXACT) =============
class ExactBacktrackingSolver {
private:
    const Graph& graph;
    vector<int> current_clique;
    vector<int> best_clique;
    int k;
    long long nodes_explored;
    bool found;
    
    void backtrack(int start) {
        nodes_explored++;
        
        if ((int)current_clique.size() == k) {
            best_clique = current_clique;
            found = true;
            return;
        }
        
        int needed = k - current_clique. size();
        int available = graph.getNodes() - start;
        if (available < needed) {
            return;
        }
        
        for (int i = start; i < graph.getNodes(); i++) {
            if (found) return;
            
            bool is_connected = true;
            for (int node : current_clique) {
                if (! graph.hasEdge(i, node)) {
                    is_connected = false;
                    break;
                }
            }
            
            if (is_connected) {
                current_clique.push_back(i);
                backtrack(i + 1);
                current_clique.pop_back();
            }
        }
    }
    
public:
    ExactBacktrackingSolver(const Graph& g, int clique_size) 
        : graph(g), k(clique_size), nodes_explored(0), found(false) {}
    
    SolutionResult solve() {
        auto start_time = high_resolution_clock:: now();
        
        current_clique.clear();
        best_clique.clear();
        nodes_explored = 0;
        found = false;
        
        backtrack(0);
        
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time);
        
        SolutionResult result;
        result.clique = best_clique;
        result. found = found;
        result. time_microseconds = duration.count();
        result.nodes_explored = nodes_explored;
        result.algorithm_name = "BACKTRACKING EXACT";
        
        return result;
    }
};

// ============= ALGORITM 2: EURISTICĂ GREEDY CU COLORARE =============
class GreedyColoringSolver {
private:
    const Graph& graph;
    int k;
    
    vector<int> greedyColoring() {
        int n = graph.getNodes();
        vector<int> colors(n, -1);
        vector<set<int>> neighbor_colors(n);
        
        vector<pair<int, int>> degree_nodes;
        for (int i = 0; i < n; i++) {
            degree_nodes.push_back({graph.getDegree(i), i});
        }
        sort(degree_nodes.rbegin(), degree_nodes.rend());
        
        for (auto [deg, v] : degree_nodes) {
            int color = 0;
            while (neighbor_colors[v].count(color)) {
                color++;
            }
            colors[v] = color;
            
            for (int u : graph.getNeighbors(v)) {
                neighbor_colors[u].insert(color);
            }
        }
        
        return colors;
    }
    
    vector<int> buildCliqueFromSubset(vector<int> candidates) {
        vector<int> clique;
        
        while (! candidates.empty() && (int)clique.size() < k) {
            int best_node = -1;
            int max_connections = -1;
            
            for (int v : candidates) {
                int connections = 0;
                for (int u : candidates) {
                    if (v != u && graph.hasEdge(v, u)) {
                        connections++;
                    }
                }
                
                if (connections > max_connections) {
                    max_connections = connections;
                    best_node = v;
                }
            }
            
            if (best_node == -1) break;
            
            clique. push_back(best_node);
            
            vector<int> new_candidates;
            for (int v : candidates) {
                if (v != best_node && graph.hasEdge(best_node, v)) {
                    new_candidates.push_back(v);
                }
            }
            candidates = new_candidates;
        }
        
        return clique;
    }
    
public:
    GreedyColoringSolver(const Graph& g, int clique_size) 
        : graph(g), k(clique_size) {}
    
    SolutionResult solve() {
        auto start_time = high_resolution_clock::now();
        
        vector<int> colors = greedyColoring();
        
        map<int, vector<int>> color_classes;
        for (int i = 0; i < graph.getNodes(); i++) {
            color_classes[colors[i]].push_back(i);
        }
        
        vector<int> best_clique;
        
        for (auto& [color, nodes] : color_classes) {
            if ((int)nodes.size() >= k) {
                vector<int> clique = buildCliqueFromSubset(nodes);
                
                if ((int)clique.size() >= k && graph.isClique(clique)) {
                    best_clique = clique;
                    best_clique.resize(k);
                    break;
                }
                
                if (clique.size() > best_clique.size()) {
                    best_clique = clique;
                }
            }
        }
        
        if ((int)best_clique.size() < k) {
            vector<int> all_nodes;
            for (int i = 0; i < graph.getNodes(); i++) {
                all_nodes.push_back(i);
            }
            
            sort(all_nodes.begin(), all_nodes.end(), [&](int a, int b) {
                return graph.getDegree(a) > graph.getDegree(b);
            });
            
            best_clique = buildCliqueFromSubset(all_nodes);
        }
        
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time);
        
        SolutionResult result;
        result. clique = best_clique;
        result.found = (int)best_clique.size() >= k && graph.isClique(best_clique);
        result.time_microseconds = duration.count();
        result.nodes_explored = 0;
        result.algorithm_name = "GREEDY + COLORING (Euristică 1)";
        
        return result;
    }
};

// ============= ALGORITM 3: LOCAL SEARCH CU TABU =============
class TabuSearchSolver {
private: 
    const Graph& graph;
    int k;
    int max_iterations;
    int tabu_tenure;
    mt19937 rng;
    
    vector<int> initialSolution() {
        vector<int> clique;
        vector<int> candidates;
        
        int max_degree = -1;
        int start_node = 0;
        for (int i = 0; i < graph.getNodes(); i++) {
            if (graph.getDegree(i) > max_degree) {
                max_degree = graph.getDegree(i);
                start_node = i;
            }
        }
        
        clique.push_back(start_node);
        
        for (int v :  graph.getNeighbors(start_node)) {
            candidates.push_back(v);
        }
        
        while (! candidates.empty() && (int)clique.size() < k) {
            int best = -1;
            int max_conn = -1;
            
            for (int v : candidates) {
                int conn = 0;
                for (int u : clique) {
                    if (graph.hasEdge(v, u)) conn++;
                }
                if (conn > max_conn) {
                    max_conn = conn;
                    best = v;
                }
            }
            
            if (best == -1 || max_conn < (int)clique.size()) break;
            
            clique. push_back(best);
            
            vector<int> new_candidates;
            for (int v : candidates) {
                if (v != best && graph.hasEdge(best, v)) {
                    bool valid = true;
                    for (int u : clique) {
                        if (! graph.hasEdge(v, u)) {
                            valid = false;
                            break;
                        }
                    }
                    if (valid) new_candidates.push_back(v);
                }
            }
            candidates = new_candidates;
        }
        
        return clique;
    }
    
    vector<int> findCandidates(const vector<int>& clique) {
        set<int> in_clique(clique.begin(), clique.end());
        vector<int> candidates;
        
        for (int v = 0; v < graph.getNodes(); v++) {
            if (in_clique. count(v)) continue;
            
            bool is_candidate = true;
            for (int u : clique) {
                if (! graph.hasEdge(v, u)) {
                    is_candidate = false;
                    break;
                }
            }
            
            if (is_candidate) {
                candidates.push_back(v);
            }
        }
        
        return candidates;
    }
    
public: 
    TabuSearchSolver(const Graph& g, int clique_size, int max_iter = 1000, int tenure = 10) 
        : graph(g), k(clique_size), max_iterations(max_iter), tabu_tenure(tenure) {
        random_device rd;
        rng = mt19937(rd());
    }
    
    SolutionResult solve() {
        auto start_time = high_resolution_clock::now();
        
        vector<int> current_solution = initialSolution();
        vector<int> best_solution = current_solution;
        
        map<int, int> tabu_list;
        
        long long iterations = 0;
        
        while (iterations < max_iterations && (int)best_solution.size() < k) {
            iterations++;
            
            vector<int> candidates = findCandidates(current_solution);
            
            if (! candidates.empty()) {
                int best_candidate = -1;
                int best_score = -1;
                
                for (int v : candidates) {
                    if (tabu_list.count(v) && tabu_list[v] > (int)iterations) {
                        if ((int)current_solution.size() + 1 <= (int)best_solution.size()) {
                            continue;
                        }
                    }
                    
                    int score = graph.getDegree(v);
                    if (score > best_score) {
                        best_score = score;
                        best_candidate = v;
                    }
                }
                
                if (best_candidate != -1) {
                    current_solution.push_back(best_candidate);
                    
                    if (current_solution.size() > best_solution.size()) {
                        best_solution = current_solution;
                    }
                }
            } else if (! current_solution.empty()) {
                uniform_int_distribution<int> dist(0, current_solution.size() - 1);
                int remove_idx = dist(rng);
                int removed = current_solution[remove_idx];
                
                current_solution.erase(current_solution.begin() + remove_idx);
                
                tabu_list[removed] = iterations + tabu_tenure;
                
                if (current_solution.size() < best_solution.size() / 2) {
                    current_solution = best_solution;
                }
            } else {
                current_solution = initialSolution();
            }
            
            if (iterations % 100 == 0) {
                for (auto it = tabu_list.begin(); it != tabu_list.end(); ) {
                    if (it->second <= (int)iterations) {
                        it = tabu_list.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
        
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time);
        
        SolutionResult result;
        result. clique = best_solution;
        result.found = (int)best_solution.size() >= k && graph.isClique(best_solution);
        result.time_microseconds = duration.count();
        result.nodes_explored = iterations;
        result.algorithm_name = "TABU SEARCH (Euristică 2)";
        
        return result;
    }
};

// ============= UTILITĂȚI - ÎNCĂRCARE DATE =============

// Funcție îmbunătățită pentru citirea fișierelor generate de Python
Graph loadFromEdgeList(const string& filename, int& k) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Eroare:  Nu pot deschide fișierul " << filename << "\n";
        exit(1);
    }
    
    int n = 0, m = 0;
    k = 0;
    string line;
    
    cout << "Citire fișier: " << filename << "\n";
    
    // Citește header-urile (linii care încep cu #)
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        if (line[0] == '#') {
            // Încearcă să extragă n_nodes
            size_t pos = line.find("n_nodes:");
            if (pos != string::npos) {
                string num_str = line.substr(pos + 8);
                n = stoi(num_str);
                cout << "  n_nodes = " << n << "\n";
            }
            
            // Încearcă să extragă n_edges
            pos = line.find("n_edges:");
            if (pos != string::npos) {
                string num_str = line. substr(pos + 8);
                m = stoi(num_str);
                cout << "  n_edges = " << m << "\n";
            }
            
            // Încearcă să extragă k
            pos = line.find("k:");
            if (pos != string::npos) {
                string num_str = line.substr(pos + 2);
                k = stoi(num_str);
                cout << "  k = " << k << "\n";
            }
        } else {
            // Prima linie care nu e comentariu - pune-o înapoi pentru procesare
            break;
        }
    }
    
    if (n == 0) {
        cerr << "Eroare: Nu am putut citi numărul de noduri din fișier\n";
        exit(1);
    }
    
    Graph g(n);
    int edges_read = 0;
    
    // Procesează prima linie de date (dacă există)
    if (!line.empty() && line[0] != '#') {
        istringstream iss(line);
        int u, v;
        if (iss >> u >> v) {
            g.addEdge(u, v);
            edges_read++;
        }
    }
    
    // Citește restul muchiilor
    int u, v;
    while (file >> u >> v) {
        g.addEdge(u, v);
        edges_read++;
    }
    
    file.close();
    
    cout << "  Muchii citite: " << edges_read << "\n";
    
    if (k == 0) {
        // Dacă k nu a fost specificat în fișier, folosim valoare implicită
        k = 5;
        cout << "  k nu a fost specificat, folosim k = " << k << "\n";
    }
    
    return g;
}

Graph createTestGraph() {
    cout << "Creare graf de test.. .\n";
    int n = 15;
    Graph g(n);
    
    // Creează un 5-clique:  {0, 1, 2, 3, 4}
    cout << "  Adăugare 5-clique: {0, 1, 2, 3, 4}\n";
    for (int i = 0; i < 5; i++) {
        for (int j = i + 1; j < 5; j++) {
            g.addEdge(i, j);
        }
    }
    
    // Adaugă muchii random
    g.addEdge(4, 5);
    g.addEdge(5, 6);
    g.addEdge(6, 7);
    g.addEdge(7, 8);
    g.addEdge(1, 8);
    g.addEdge(2, 9);
    g.addEdge(9, 10);
    g.addEdge(10, 11);
    g.addEdge(11, 12);
    g.addEdge(12, 13);
    g.addEdge(13, 14);
    g.addEdge(3, 10);
    
    return g;
}

void compareResults(const vector<SolutionResult>& results) {
    cout << "\n" << string(70, '=') << "\n";
    cout << "COMPARAȚIE ALGORITMI\n";
    cout << string(70, '=') << "\n\n";
    
    cout << "Algoritm                          Găsit?    Mărime  Timp(ms)   Noduri\n";
    cout << string(70, '-') << "\n";
    
    for (const auto& result :  results) {
        cout.width(30);
        cout << left << result.algorithm_name;
        cout << (result.found ? " ✓  " : " ✗  ");
        cout. width(7);
        cout << right << result.clique.size();
        cout.width(11);
        cout << right << fixed;
        cout.precision(3);
        cout << (result.time_microseconds / 1000.0);
        if (result.nodes_explored > 0) {
            cout. width(10);
            cout << right << result.nodes_explored;
        }
        cout << "\n";
    }
    
    cout << string(70, '=') << "\n";
}

// ============= MAIN =============
int main(int argc, char* argv[]) {
    cout << "\n";
    cout << "╔════════════════════════════════════════════════════════════╗\n";
    cout << "║         K-CLIQUE PROBLEM SOLVER - 3 Algoritmi              ║\n";
    cout << "╚════════════════════════════════════════════════════════════╝\n";
    cout << "\n";
    
    Graph graph(0);
    int k = 5;
    
    if (argc > 1) {
        // Încarcă din fișier
        string filename = argv[1];
        if (argc > 2) {
            k = atoi(argv[2]);
        }
        
        cout << "Mod:  Încărcare din fișier\n";
        graph = loadFromEdgeList(filename, k);
        
        // Dacă k a fost specificat ca argument, suprascrie valoarea din fișier
        if (argc > 2) {
            k = atoi(argv[2]);
            cout << "  k suprascris din argument:  " << k << "\n";
        }
    } else {
        // Graf de test
        cout << "Mod: Graf de test implicit\n";
        graph = createTestGraph();
    }
    
    cout << "\n";
    graph.printGraph();
    cout << "Căutăm un " << k << "-clique\n";
    cout << string(70, '=') << "\n";
    
    vector<SolutionResult> results;
    
    // ALGORITM 1: Exact Backtracking
    cout << "\n[1/3] Rulare algoritm exact (Backtracking)...\n";
    ExactBacktrackingSolver exact(graph, k);
    SolutionResult result1 = exact.solve();
    result1.print();
    results.push_back(result1);
    
    // ALGORITM 2: Greedy + Coloring
    cout << "\n[2/3] Rulare euristică 1 (Greedy + Coloring)...\n";
    GreedyColoringSolver greedy(graph, k);
    SolutionResult result2 = greedy.solve();
    result2.print();
    results.push_back(result2);
    
    // ALGORITM 3: Tabu Search
    cout << "\n[3/3] Rulare euristică 2 (Tabu Search)...\n";
    TabuSearchSolver tabu(graph, k, 1000, 10);
    SolutionResult result3 = tabu.solve();
    result3.print();
    results.push_back(result3);
    
    // Comparație
    compareResults(results);
    
    // Verificare finală
    cout << "\n=== VERIFICARE CORECTITUDINE ===\n";
    int correct = 0;
    for (const auto& result : results) {
        if (result.found) {
            bool valid = graph.isClique(result.clique);
            cout << result.algorithm_name << ": " 
                 << (valid ? "✓ Clică validă" : "✗ Clică invalidă") << "\n";
            if (valid) correct++;
        }
    }
    cout << "\nAlgoritmi care au găsit soluție validă: " << correct << "/" << results.size() << "\n";
    
    return 0;
}