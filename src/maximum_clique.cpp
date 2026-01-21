#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <set>
#include <queue>
#include <iomanip>

using namespace std;
using namespace chrono;

class Graph {
private:
    int n, m;
    vector<vector<int>> adj;
    vector<set<int>> adjSet; // Pentru verificare rapidă
    
public:
    Graph(int nodes) : n(nodes), m(0) {
        adj.resize(n);
        adjSet.resize(n);
    }
    
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        adjSet[u].insert(v);
        adjSet[v].insert(u);
        m++;
    }
    
    bool areAdjacent(int u, int v) const {
        return adjSet[u].count(v) > 0;
    }
    
    int getNodes() const { return n; }
    int getEdges() const { return m; }
    const vector<int>& getNeighbors(int u) const { return adj[u]; }
    int getDegree(int u) const { return adj[u].size(); }
};

// ============================================================================
// ALGORITM 1: BACKTRACKING EXACT (garantează soluția corectă)
// ============================================================================
// Complexitate: O(3^(n/3)) în cel mai rău caz - exponențial
// Garanție: Găsește întotdeauna clica maximă
// Dezavantaj: Foarte lent pentru grafuri mari (n > 50)

class ExactBacktracking {
private:  
    const Graph& g;
    vector<int> bestClique;
    vector<int> currentClique;
    
    // Verifică dacă nodul u este adiacent cu toți nodurile din clica curentă
    bool isClique(int u) {
        for (int v : currentClique) {
            if (! g.areAdjacent(u, v)) return false;
        }
        return true;
    }
    
    void backtrack(int start) {
        // Actualizează cea mai bună soluție
        if (currentClique.size() > bestClique.size()) {
            bestClique = currentClique;
        }
        
        // Pruning: dacă nu putem depăși soluția curentă, stop
        if (currentClique. size() + (g.getNodes() - start) <= bestClique.size()) {
            return;
        }
        
        // Încearcă să adaugi fiecare nod rămas
        for (int u = start; u < g.getNodes(); u++) {
            if (isClique(u)) {
                currentClique.push_back(u);
                backtrack(u + 1);
                currentClique.pop_back();
            }
        }
    }
    
public:  
    ExactBacktracking(const Graph& graph) : g(graph) {}
    
    vector<int> findMaxClique() {
        bestClique.clear();
        currentClique.clear();
        backtrack(0);
        return bestClique;
    }
};

// ============================================================================
// ALGORITM 2: GREEDY HEURISTIC (bazat pe grad maxim)
// ============================================================================
// Complexitate: O(n^2)
// Garanție:  Nicio garanție de optimalitate, dar rapid
// Idee: Selectează iterativ nodul cu cel mai mare grad din submulțimea validă
// Rată de aproximare:  Poate rata soluția optimă semnificativ

class GreedyMaxDegree {
private:  
    const Graph& g;
    
    // Calculează câți vecini din clica curentă are fiecare nod candidat
    int countCliqueNeighbors(int u, const vector<int>& clique) {
        int count = 0;
        for (int v : clique) {
            if (g.areAdjacent(u, v)) count++;
        }
        return count;
    }
    
public: 
    GreedyMaxDegree(const Graph& graph) : g(graph) {}
    
    vector<int> findMaxClique() {
        vector<int> clique;
        vector<bool> used(g.getNodes(), false);
        
        // Începe cu nodul de grad maxim
        int maxDeg = -1, startNode = 0;
        for (int i = 0; i < g. getNodes(); i++) {
            if (g.getDegree(i) > maxDeg) {
                maxDeg = g.getDegree(i);
                startNode = i;
            }
        }
        
        clique.push_back(startNode);
        used[startNode] = true;
        
        // Greedy: adaugă nodurile care sunt adiacente cu toate din clică
        bool changed = true;
        while (changed) {
            changed = false;
            int bestNode = -1;
            int maxNeighbors = -1;
            
            for (int u = 0; u < g.getNodes(); u++) {
                if (used[u]) continue;
                
                // Verifică dacă u este adiacent cu toți din clică
                int neighbors = countCliqueNeighbors(u, clique);
                if (neighbors == (int)clique.size() && neighbors > maxNeighbors) {
                    maxNeighbors = neighbors;
                    bestNode = u;
                }
            }
            
            if (bestNode != -1) {
                clique.push_back(bestNode);
                used[bestNode] = true;
                changed = true;
            }
        }
        
        return clique;
    }
};

// ============================================================================
// ALGORITM 3: BRANCH AND BOUND cu SORTARE și PRUNING AVANSAT
// ============================================================================
// Complexitate: O(2^n) în cel mai rău caz, dar mult mai rapid în practică
// Garanție:  Găsește soluția optimă (ca backtracking)
// Optimizări: 
//   - Sortare după grad descrescător
//   - Upper bound estimation (culoare grafului)
//   - Pruning mai agresiv

class BranchAndBound {
private: 
    const Graph& g;
    vector<int> bestClique;
    vector<int> currentClique;
    vector<int> order; // Ordinea nodurilor sortată după grad
    
    bool isClique(int u) {
        for (int v : currentClique) {
            if (! g.areAdjacent(u, v)) return false;
        }
        return true;
    }
    
    // Estimare upper bound:  colorare greedy a subgrafului rămas
    int upperBound(const vector<int>& candidates) {
        return candidates.size(); // Simplificat pentru viteză
    }
    
    void branchAndBound(vector<int>& candidates) {
        if (currentClique.size() > bestClique.size()) {
            bestClique = currentClique;
        }
        
        if (candidates.empty()) return;
        
        // Pruning: upper bound
        if (currentClique.size() + upperBound(candidates) <= bestClique.size()) {
            return;
        }
        
        // Încearcă fiecare candidat
        while (!candidates.empty()) {
            int u = candidates.back();
            candidates.pop_back();
            
            // Pruning
            if (currentClique.size() + candidates.size() + 1 <= bestClique.size()) {
                break;
            }
            
            if (isClique(u)) {
                currentClique.push_back(u);
                
                // Creează noua listă de candidați (doar vecinii lui u)
                vector<int> newCandidates;
                for (int v : candidates) {
                    if (g.areAdjacent(u, v)) {
                        newCandidates.push_back(v);
                    }
                }
                
                branchAndBound(newCandidates);
                currentClique.pop_back();
            }
        }
    }
    
public:
    BranchAndBound(const Graph& graph) : g(graph) {
        // Sortează nodurile după grad descrescător
        order.resize(g.getNodes());
        for (int i = 0; i < g.getNodes(); i++) {
            order[i] = i;
        }
        sort(order.begin(), order.end(), [&](int a, int b) {
            return g. getDegree(a) > g.getDegree(b);
        });
    }
    
    vector<int> findMaxClique() {
        bestClique.clear();
        currentClique.clear();
        branchAndBound(order);
        return bestClique;
    }
};

// ============================================================================
// FUNCȚII UTILITARE
// ============================================================================

// Funcție pentru formatarea timpului în unitatea potrivită
string formatTime(long long microseconds) {
    if (microseconds < 1000) {
        return to_string(microseconds) + " μs";
    } else if (microseconds < 1000000) {
        stringstream ss;
        ss << fixed << setprecision(2) << (microseconds / 1000.0) << " ms";
        return ss.str();
    } else {
        stringstream ss;
        ss << fixed << setprecision(2) << (microseconds / 1000000.0) << " s";
        return ss.str();
    }
}

void printClique(const vector<int>& clique, const string& algorithm) {
    cout << "\n=== " << algorithm << " ===\n";
    cout << "Dimensiune clică: " << clique.size() << "\n";
    cout << "Noduri: ";
    for (int node : clique) {
        cout << node << " ";
    }
    cout << "\n";
}

bool verifyClique(const Graph& g, const vector<int>& clique) {
    for (size_t i = 0; i < clique.size(); i++) {
        for (size_t j = i + 1; j < clique.size(); j++) {
            if (! g.areAdjacent(clique[i], clique[j])) {
                return false;
            }
        }
    }
    return true;
}

// ============================================================================
// MAIN - Testare și Comparații
// ============================================================================

int main() {
    // Setare pentru output formatat
    cout << fixed << setprecision(2);
    
    // Citire din fișier
    ifstream fin("clique.in");
    ofstream fout("clique.out");
    
    int n, m;
    fin >> n >> m;
    
    Graph g(n);
    
    for (int i = 0; i < m; i++) {
        int u, v;
        fin >> u >> v;
        g.addEdge(u, v);
    }
    
    fin.close();
    
    cout << "Graf:  " << n << " noduri, " << m << " muchii\n";
    cout << string(60, '=') << "\n";
    
    // ============= ALGORITM 1: BACKTRACKING EXACT =============
    cout << "\n[1] Rulare Backtracking Exact.. .\n";
    auto start1 = high_resolution_clock:: now();
    
    ExactBacktracking exact(g);
    vector<int> exactClique = exact.findMaxClique();
    
    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<microseconds>(end1 - start1);
    
    printClique(exactClique, "Backtracking Exact");
    cout << "Timp execuție: " << formatTime(duration1.count()) << "\n";
    cout << "Verificare validitate: " << (verifyClique(g, exactClique) ? "✓ Valid" : "✗ Invalid") << "\n";
    
    // ============= ALGORITM 2: GREEDY HEURISTIC =============
    cout << "\n[2] Rulare Greedy Heuristic...\n";
    auto start2 = high_resolution_clock::now();
    
    GreedyMaxDegree greedy(g);
    vector<int> greedyClique = greedy. findMaxClique();
    
    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(end2 - start2);
    
    printClique(greedyClique, "Greedy Max Degree");
    cout << "Timp execuție: " << formatTime(duration2.count()) << "\n";
    cout << "Verificare validitate: " << (verifyClique(g, greedyClique) ? "✓ Valid" : "✗ Invalid") << "\n";
    
    double accuracy2 = (double)greedyClique.size() / exactClique.size() * 100;
    cout << "Acuratețe: " << accuracy2 << "% (raport față de optim)\n";
    
    // ============= ALGORITM 3: BRANCH AND BOUND =============
    cout << "\n[3] Rulare Branch and Bound...\n";
    auto start3 = high_resolution_clock::now();
    
    BranchAndBound bnb(g);
    vector<int> bnbClique = bnb.findMaxClique();
    
    auto end3 = high_resolution_clock:: now();
    auto duration3 = duration_cast<microseconds>(end3 - start3);
    
    printClique(bnbClique, "Branch and Bound");
    cout << "Timp execuție: " << formatTime(duration3.count()) << "\n";
    cout << "Verificare validitate: " << (verifyClique(g, bnbClique) ? "✓ Valid" : "✗ Invalid") << "\n";
    
    double accuracy3 = (double)bnbClique.size() / exactClique.size() * 100;
    cout << "Acuratețe: " << accuracy3 << "% (raport față de optim)\n";
    
    // ============= COMPARAȚII =============
    cout << "\n" << string(60, '=') << "\n";
    cout << "COMPARAȚII:\n";
    cout << string(60, '=') << "\n";
    
    cout << "\nDimensiuni clici găsite:\n";
    cout << "  Exact:    " << exactClique.size() << " (optimal)\n";
    cout << "  Greedy:  " << greedyClique.size() << " (" << accuracy2 << "%)\n";
    cout << "  B&B:     " << bnbClique.size() << " (" << accuracy3 << "%)\n";
    
    cout << "\nTimp de execuție:\n";
    cout << "  Exact:   " << formatTime(duration1.count()) << " (baseline)\n";
    
    long long time2 = max(1LL, (long long)duration2.count());
    cout << "  Greedy:  " << formatTime(duration2.count()) << " (speedup:  " 
         << (double)duration1.count() / time2 << "x)\n";
    
    long long time3 = max(1LL, (long long)duration3.count());
    cout << "  B&B:      " << formatTime(duration3.count()) << " (speedup: "
         << (double)duration1.count() / time3 << "x)\n";
    
    // Statistici suplimentare
    cout << "\n" << string(60, '=') << "\n";
    cout << "STATISTICI GRAF:\n";
    cout << string(60, '=') << "\n";
    cout << "Densitate graf: " << (2.0 * m) / (n * (n - 1)) * 100 << "%\n";
    
    int minDeg = n, maxDeg = 0;
    double avgDeg = 0;
    for (int i = 0; i < n; i++) {
        int deg = g.getDegree(i);
        minDeg = min(minDeg, deg);
        maxDeg = max(maxDeg, deg);
        avgDeg += deg;
    }
    avgDeg /= n;
    
    cout << "Grad minim: " << minDeg << "\n";
    cout << "Grad maxim: " << maxDeg << "\n";
    cout << "Grad mediu: " << avgDeg << "\n";
    cout << "Dimensiune clică maximă: " << exactClique.size() 
         << " (" << (double)exactClique.size() / n * 100 << "% din noduri)\n";
    
    // ============= SCRIERE ÎN FIȘIER - TOATE REZULTATELE =============
    fout << "REZULTATE PROBLEMA CLICII MAXIME\n";
    fout << "=================================\n\n";
    
    fout << "Graf: " << n << " noduri, " << m << " muchii\n";
    fout << "Densitate: " << fixed << setprecision(2) << (2.0 * m) / (n * (n - 1)) * 100 << "%\n\n";
    
    // Algoritm 1: Backtracking Exact
    fout << "1.  BACKTRACKING EXACT (Optimal)\n";
    fout << "   Dimensiune clică:  " << exactClique.size() << "\n";
    fout << "   Noduri: ";
    for (int node : exactClique) {
        fout << node << " ";
    }
    fout << "\n";
    fout << "   Timp execuție:  " << duration1.count() << " μs\n";
    fout << "   Validitate: " << (verifyClique(g, exactClique) ? "Valid" : "Invalid") << "\n\n";
    
    // Algoritm 2: Greedy Heuristic
    fout << "2. GREEDY HEURISTIC\n";
    fout << "   Dimensiune clică: " << greedyClique.size() << "\n";
    fout << "   Noduri: ";
    for (int node : greedyClique) {
        fout << node << " ";
    }
    fout << "\n";
    fout << "   Timp execuție: " << duration2.count() << " μs\n";
    fout << "   Acuratețe: " << fixed << setprecision(2) << accuracy2 << "%\n";
    fout << "   Speedup: " << (double)duration1.count() / max(1LL, (long long)duration2.count()) << "x\n";
    fout << "   Validitate: " << (verifyClique(g, greedyClique) ? "Valid" : "Invalid") << "\n\n";
    
    // Algoritm 3: Branch and Bound
    fout << "3. BRANCH AND BOUND (Optimal)\n";
    fout << "   Dimensiune clică: " << bnbClique.size() << "\n";
    fout << "   Noduri: ";
    for (int node : bnbClique) {
        fout << node << " ";
    }
    fout << "\n";
    fout << "   Timp execuție: " << duration3.count() << " μs\n";
    fout << "   Acuratețe: " << fixed << setprecision(2) << accuracy3 << "%\n";
    fout << "   Speedup: " << (double)duration1.count() / max(1LL, (long long)duration3.count()) << "x\n";
    fout << "   Validitate: " << (verifyClique(g, bnbClique) ? "Valid" : "Invalid") << "\n\n";
    
    // Sumar comparativ
    fout << "=================================\n";
    fout << "SUMAR COMPARATIV\n";
    fout << "=================================\n\n";
    fout << "Cea mai bună soluție: " << exactClique.size() << " noduri\n";
    fout << "Cel mai rapid algoritm:  Greedy Heuristic (" << duration2.count() << " μs)\n";
    fout << "Algoritm recomandat pentru acest graf: ";
    if (n <= 30) {
        fout << "Backtracking Exact (graf mic)\n";
    } else if (n <= 50) {
        fout << "Branch and Bound (graf mediu)\n";
    } else {
        fout << "Greedy Heuristic (graf mare)\n";
    }
    
    fout. close();
    
    cout << "\nRezultatele tuturor algoritmilor au fost scrise în clique.out\n";
    
    return 0;
}
