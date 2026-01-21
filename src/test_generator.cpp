#include <iostream>
#include <fstream>
#include <random>
#include <set>

using namespace std;

// Generează un graf aleator
void generateRandomGraph(int n, int m, const string& filename) {
    ofstream fout(filename);
    fout << n << " " << m << "\n";
    
    set<pair<int, int>> edges;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);
    
    while (edges.size() < m) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v && u < v) {
            edges.insert({u, v});
        }
    }
    
    for (auto [u, v] : edges) {
        fout << u << " " << v << "\n";
    }
    
    fout.close();
    cout << "Generat:  " << filename << " (" << n << " noduri, " << m << " muchii)\n";
}

// Generează un graf cu o clică mare garantată
void generateGraphWithClique(int n, int cliqueSize, int extraEdges, const string& filename) {
    ofstream fout(filename);
    
    set<pair<int, int>> edges;
    
    // Adaugă clica
    for (int i = 0; i < cliqueSize; i++) {
        for (int j = i + 1; j < cliqueSize; j++) {
            edges.insert({i, j});
        }
    }
    
    // Adaugă muchii random
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);
    
    while (edges.size() < cliqueSize * (cliqueSize - 1) / 2 + extraEdges) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v && u < v && edges.find({u, v}) == edges.end()) {
            edges.insert({u, v});
        }
    }
    
    fout << n << " " << edges.size() << "\n";
    for (auto [u, v] : edges) {
        fout << u << " " << v << "\n";
    }
    
    fout.close();
    cout << "Generat: " << filename << " (clică de " << cliqueSize << " noduri)\n";
}

int main() {
    cout << "Generare teste pentru problema clicii maxime...\n\n";
    
    // Test mic
    generateRandomGraph(20, 50, "test1_small.in");
    
    // Test mediu
    generateRandomGraph(40, 200, "test2_medium.in");
    
    // Test cu clică garantată
    generateGraphWithClique(30, 8, 100, "test3_clique.in");
    
    // Test sparse
    generateRandomGraph(50, 100, "test4_sparse.in");
    
    // Test dense
    generateRandomGraph(30, 300, "test5_dense.in");
    
    cout << "\nToate testele au fost generate!\n";
    cout << "Redenumește test-ul dorit în 'clique.in' pentru a-l rula.\n";
    
    return 0;
}