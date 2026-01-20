import os
import subprocess
import random
import csv
import matplotlib.pyplot as plt

# Configuration
CPP_BINARY = "./kclique"  # Compile with: g++ -std=c++17 kclique.cpp -o kclique
TEST_DIR = "tests"
RESULTS_FILE = "results.csv"

def generate_planted_clique(filename, n, hidden_clique_size, noise_density):
    """
    Generates a graph with a guaranteed clique of size 'hidden_clique_size'.
    """
    print(f"Generating {filename} (N={n}, Planted K={hidden_clique_size})...")
    
    nodes = list(range(n))
    random.shuffle(nodes)
    clique_nodes = set(nodes[:hidden_clique_size])
    
    edges = set()
    
    # 1. Add Planted Clique
    sorted_clique = sorted(list(clique_nodes))
    for i in range(len(sorted_clique)):
        for j in range(i + 1, len(sorted_clique)):
            u, v = sorted_clique[i], sorted_clique[j]
            edges.add((u, v))
            
    # 2. Add Noise (High density = Harder for Backtracking)
    for i in range(n):
        for j in range(i + 1, n):
            u, v = nodes[i], nodes[j]
            if (u, v) in edges or (v, u) in edges:
                continue
            if random.random() < noise_density:
                edges.add((u, v))

    with open(filename, 'w') as f:
        f.write(f"{n} {len(edges)}\n")
        for u, v in edges:
            f.write(f"{u} {v}\n")

def create_tests():
    if not os.path.exists(TEST_DIR):
        os.makedirs(TEST_DIR)
    
    test_files = []
    
    # Range 20 to 100 nodes.
    # Noise density 0.7 makes the graph dense, forcing Backtracking to explore more.
    for n in range(20, 101, 20): # 20, 40, 60, 80, 100
        filename = os.path.join(TEST_DIR, f"hard_{n}.in")
        k = max(5, int(n * 0.15)) 
        generate_planted_clique(filename, n, hidden_clique_size=k, noise_density=0.7)
        test_files.append((n, filename))
        
    return test_files

def run_benchmark(test_files):
    results = [] 
    print("\n--- Starting Benchmark ---")
    
    for n, filepath in test_files:
        print(f"Running N={n}...", end=" ", flush=True)
        try:
            proc = subprocess.run([CPP_BINARY, filepath], capture_output=True, text=True)
            output = proc.stdout.splitlines()
            
            for line in output:
                if "RESULT_START" in line or "RESULT_END" in line: continue
                if "," in line:
                    algo, res, time_us = line.split(',')
                    if int(res) != -1:
                        results.append({
                            "N": n,
                            "Algorithm": algo,
                            "Size": int(res),
                            "TimeUS": int(time_us)
                        })
            print("Done.")
        except Exception as e:
            print(f"Error: {e}")
            
    return results

def save_and_plot(results):
    keys = ["N", "Algorithm", "Size", "TimeUS"]
    with open(RESULTS_FILE, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=keys)
        writer.writeheader()
        writer.writerows(results)
    print(f"\nResults saved to {RESULTS_FILE}")

    data = {}
    for row in results:
        algo = row["Algorithm"]
        if algo not in data: data[algo] = {'x': [], 'y': []}
        data[algo]['x'].append(row["N"])
        data[algo]['y'].append(row["TimeUS"])
        
    if data:
        plt.figure(figsize=(10, 6))
        for algo, coords in data.items():
            plt.plot(coords['x'], coords['y'], marker='o', label=algo)
        plt.title("Algorithm Performance on Hard Graphs")
        plt.xlabel("Number of Nodes (N)")
        plt.ylabel("Time (microseconds)")
        plt.legend()
        plt.grid(True)
        plt.savefig("clique_benchmark.png")
        print("Graph saved to clique_benchmark.png")

if __name__ == "__main__":
    if not os.path.exists(CPP_BINARY):
        print(f"Error: {CPP_BINARY} not found. Please compile C++ first.")
        exit(1)
    
    tests = create_tests()
    data = run_benchmark(tests)
    save_and_plot(data)
