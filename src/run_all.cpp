#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

// Doar numele fișierelor (fără prefixul folderului, îl adăugăm dinamic)
const vector<string> testFiles = {
    "test1_small.in",
    "test2_medium.in",
    "test3_clique.in",
    "test4_sparse.in",
    "test5_dense.in"
};

#ifdef _WIN32
    const string EXECUTABLE = "maximum_clique.exe";
#else
    const string EXECUTABLE = "./maximum_clique";
#endif

void printSummaryFromOutput() {
    ifstream fin("clique.out");
    string line;
    bool inSummary = false;
    cout << "--- Rezultat ---\n";
    while (getline(fin, line)) {
        if (line.find("SUMAR COMPARATIV") != string::npos) inSummary = true;
        if (inSummary) cout << line << "\n";
    }
    fin.close();
}

int main() {
    cout << "Start automatizare teste...\n";

    // 1. Verificări preliminare
    if (!fs::exists(EXECUTABLE) && !fs::exists("maximum_clique")) {
        cerr << "EROARE: Executabilul 'maximum_clique' lipseste! Ruleaza 'make'.\n";
        return 1;
    }

    if (!fs::exists("input")) {
        cerr << "EROARE: Folderul 'input' lipseste! Ruleaza generatorul mai intai.\n";
        return 1;
    }

    // 2. Creare director output
    if (!fs::exists("output")) {
        fs::create_directory("output");
        cout << "Director creat: output/\n";
    }

    int successCount = 0;

    for (const auto& fileName : testFiles) {
        // Construim căile complete
        fs::path inputPath = fs::path("input") / fileName;
        fs::path outputFileName = fileName; 
        outputFileName.replace_extension(".out"); // test1.in -> test1.out
        fs::path outputPath = fs::path("output") / outputFileName;

        if (!fs::exists(inputPath)) {
            cerr << "SKIP: " << inputPath << " nu exista.\n";
            continue;
        }

        cout << "\n=== Rulare: " << fileName << " ===\n";

        // A. Copiază input/test.in -> clique.in (în root, pentru ca programul C++ să îl vadă)
        fs::copy_file(inputPath, "clique.in", fs::copy_options::overwrite_existing);

        // B. Execută algoritmul
        int ret = system(EXECUTABLE.c_str());

        if (ret == 0) {
            // C. Afișează sumar în consolă
            printSummaryFromOutput();

            // D. Mută/Copiază clique.out -> output/test.out
            fs::copy_file("clique.out", outputPath, fs::copy_options::overwrite_existing);
            cout << "Salvata in: " << outputPath << "\n";
            successCount++;
        } else {
            cerr << "Eroare la executia testului.\n";
        }
    }

    // Curățenie fișiere temporare din root
    if (fs::exists("clique.in")) fs::remove("clique.in");
    if (fs::exists("clique.out")) fs::remove("clique.out");

    cout << "\n" << string(60, '=') << "\n";
    cout << "FINALIZAT: " << successCount << "/" << testFiles.size() << " teste. Verifica folderul 'output/'.\n";
    
    return 0;
}