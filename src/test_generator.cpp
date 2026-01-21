#include <iostream>
#include <fstream>
#include <random>
#include <set>
#include <filesystem> // C++17 pentru gestionarea directoarelor

using namespace std;
namespace fs = std::filesystem;

// --- FUNCȚIILE DE GENERARE (Rămân neschimbate) ---
void generateRandomGraph(int n, int m, const string& filename) {
    ofstream fout(filename);
    fout << n << " " << m << "\n";
    set<pair<int, int>> edges;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);
    
    while (edges.size() < (unsigned long int)m) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v && u < v) edges.insert({u, v});
    }
    
    for (auto [u, v] : edges) fout << u << " " << v << "\n";
    fout.close();
    cout << "Generat:  " << filename << "\n";
}

void generateGraphWithClique(int n, int cliqueSize, int extraEdges, const string& filename) {
    ofstream fout(filename);
    set<pair<int, int>> edges;
    for (int i = 0; i < cliqueSize; i++) {
        for (int j = i + 1; j < cliqueSize; j++) edges.insert({i, j});
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, n - 1);
    while (edges.size() < (unsigned long int)(cliqueSize * (cliqueSize - 1) / 2 + extraEdges)) {
        int u = dis(gen);
        int v = dis(gen);
        if (u != v && u < v && edges.find({u, v}) == edges.end()) edges.insert({u, v});
    }
    fout << n << " " << edges.size() << "\n";
    for (auto [u, v] : edges) fout << u << " " << v << "\n";
    fout.close();
    cout << "Generat: " << filename << "\n";
}
// ------------------------------------------------

int main() {
    cout << "Generare teste...\n";

    // 1. Creăm directorul input dacă nu există
    if (!fs::exists("input")) {
        fs::create_directory("input");
        cout << "Director creat: input/\n";
    }
    
    // 2. Generăm fișierele direct în folderul input/
    generateRandomGraph(20, 50, "input/test1_small.in");
    generateRandomGraph(40, 200, "input/test2_medium.in");
    generateGraphWithClique(30, 8, 100, "input/test3_clique.in");
    generateRandomGraph(50, 100, "input/test4_sparse.in");
    generateRandomGraph(30, 300, "input/test5_dense.in");
    
    cout << "\nToate testele au fost generate în folderul 'input/'!\n";
    return 0;
}