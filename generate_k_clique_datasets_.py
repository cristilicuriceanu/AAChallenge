import random
import networkx as nx
import json
import os
from typing import List, Tuple, Set

class KCliqueDatasetGenerator:
    """Generator for k-clique problem datasets"""
    
    def __init__(self, seed: int = None):
        """
        Initialize the generator
        
        Args:
            seed: Random seed for reproducibility
        """
        if seed is not None:
            random.seed(seed)
            
    def generate_random_graph(self, n_nodes: int, edge_probability: float) -> nx.Graph:
        """
        Generate a random Erdős-Rényi graph
        
        Args:
            n_nodes: Number of nodes
            edge_probability: Probability of edge creation between any two nodes
            
        Returns: 
            NetworkX graph
        """
        return nx.erdos_renyi_graph(n_nodes, edge_probability)
    
    def add_clique(self, graph:  nx.Graph, k: int, node_subset: List[int] = None) -> Set[int]:
        """
        Add a k-clique to the graph
        
        Args:
            graph:  NetworkX graph
            k: Size of clique to add
            node_subset:  Specific nodes to form the clique (if None, random nodes are chosen)
            
        Returns: 
            Set of nodes forming the clique
        """
        if node_subset is None: 
            nodes = list(graph.nodes())
            if len(nodes) < k:
                raise ValueError(f"Graph has fewer than {k} nodes")
            clique_nodes = set(random.sample(nodes, k))
        else:
            if len(node_subset) != k:
                raise ValueError(f"node_subset must have exactly {k} nodes")
            clique_nodes = set(node_subset)
        
        # Add all edges to form a complete subgraph (clique)
        for node1 in clique_nodes:
            for node2 in clique_nodes: 
                if node1 != node2:
                    graph.add_edge(node1, node2)
        
        return clique_nodes
    
    def generate_graph_with_clique(self, n_nodes: int, k:  int, edge_probability: float) -> Tuple[nx.Graph, Set[int]]:
        """
        Generate a random graph with an embedded k-clique
        
        Args:
            n_nodes: Number of nodes
            k: Size of clique
            edge_probability:  Probability of random edges
            
        Returns:
            Tuple of (graph, clique_nodes)
        """
        graph = self.generate_random_graph(n_nodes, edge_probability)
        clique_nodes = self.add_clique(graph, k)
        return graph, clique_nodes
    
    def generate_graph_with_multiple_cliques(self, n_nodes: int, k: int, 
                                            n_cliques: int, edge_probability: float) -> Tuple[nx.Graph, List[Set[int]]]:
        """
        Generate a graph with multiple k-cliques
        
        Args:
            n_nodes: Number of nodes
            k: Size of each clique
            n_cliques: Number of cliques to add
            edge_probability: Probability of random edges
            
        Returns:
            Tuple of (graph, list of clique node sets)
        """
        graph = self.generate_random_graph(n_nodes, edge_probability)
        cliques = []
        
        available_nodes = list(graph.nodes())
        for _ in range(n_cliques):
            if len(available_nodes) < k:
                break
            clique_nodes = random.sample(available_nodes, k)
            clique = self.add_clique(graph, k, clique_nodes)
            cliques.append(clique)
            # Remove some nodes to reduce overlap (optional)
            # for node in list(clique)[:k//2]:
            #     if node in available_nodes:
            #         available_nodes.remove(node)
        
        return graph, cliques
    
    def graph_to_edge_list(self, graph: nx.Graph) -> List[Tuple[int, int]]: 
        """Convert graph to edge list format"""
        return list(graph.edges())
    
    def graph_to_adjacency_matrix(self, graph: nx.Graph) -> List[List[int]]:
        """Convert graph to adjacency matrix format"""
        nodes = sorted(graph.nodes())
        n = len(nodes)
        matrix = [[0] * n for _ in range(n)]
        
        node_to_idx = {node: idx for idx, node in enumerate(nodes)}
        
        for u, v in graph.edges():
            i, j = node_to_idx[u], node_to_idx[v]
            matrix[i][j] = 1
            matrix[j][i] = 1
        
        return matrix
    
    def save_dataset(self, graph: nx.Graph, k: int, cliques: List[Set[int]], 
                    filename: str, format: str = 'json'):
        """
        Save dataset to file
        
        Args:
            graph: NetworkX graph
            k:  Clique size
            cliques:  List of clique node sets
            filename: Output filename
            format:  Output format ('json', 'edge_list', 'dimacs')
        """
        os.makedirs(os.path. dirname(filename) if os.path.dirname(filename) else '.', exist_ok=True)
        
        if format == 'json':
            data = {
                'n_nodes': graph.number_of_nodes(),
                'n_edges': graph. number_of_edges(),
                'k': k,
                'edges': self.graph_to_edge_list(graph),
                'cliques': [list(clique) for clique in cliques],
                'adjacency_matrix': self.graph_to_adjacency_matrix(graph)
            }
            with open(filename, 'w') as f:
                json.dump(data, f, indent=2)
        
        elif format == 'edge_list':
            with open(filename, 'w') as f:
                f.write(f"# n_nodes: {graph.number_of_nodes()}\n")
                f.write(f"# n_edges: {graph.number_of_edges()}\n")
                f.write(f"# k: {k}\n")
                for u, v in graph.edges():
                    f.write(f"{u} {v}\n")
        
        elif format == 'dimacs':
            # DIMACS format for clique problems
            with open(filename, 'w') as f:
                f.write(f"p edge {graph.number_of_nodes()} {graph.number_of_edges()}\n")
                for u, v in graph.edges():
                    f.write(f"e {u+1} {v+1}\n")  # DIMACS uses 1-indexed nodes
    
    def generate_test_suite(self, output_dir: str = 'datasets'):
        """Generate a comprehensive test suite with various difficulty levels"""
        
        test_cases = [
            # Easy cases
            {'name': 'easy_small', 'n_nodes': 20, 'k': 3, 'edge_prob': 0.3, 'n_cliques': 1},
            {'name': 'easy_medium', 'n_nodes': 50, 'k': 4, 'edge_prob': 0.25, 'n_cliques':  1},
            
            # Medium cases
            {'name': 'medium_sparse', 'n_nodes': 100, 'k': 5, 'edge_prob': 0.1, 'n_cliques':  2},
            {'name':  'medium_dense', 'n_nodes': 100, 'k': 6, 'edge_prob': 0.4, 'n_cliques':  2},
            
            # Hard cases
            {'name': 'hard_large', 'n_nodes': 200, 'k': 7, 'edge_prob': 0.15, 'n_cliques':  3},
            {'name':  'hard_very_dense', 'n_nodes': 150, 'k': 8, 'edge_prob': 0.5, 'n_cliques':  2},
            
            # Very hard cases
            {'name': 'very_hard_sparse', 'n_nodes': 300, 'k': 10, 'edge_prob': 0.05, 'n_cliques': 1},
            {'name': 'very_hard_large', 'n_nodes': 500, 'k': 12, 'edge_prob': 0.1, 'n_cliques':  2},
        ]
        
        for test_case in test_cases:
            print(f"Generating {test_case['name']}...")
            
            graph, cliques = self.generate_graph_with_multiple_cliques(
                n_nodes=test_case['n_nodes'],
                k=test_case['k'],
                n_cliques=test_case['n_cliques'],
                edge_probability=test_case['edge_prob']
            )
            
            # Save in multiple formats
            base_path = os.path.join(output_dir, test_case['name'])
            self.save_dataset(graph, test_case['k'], cliques, f"{base_path}.json", format='json')
            self.save_dataset(graph, test_case['k'], cliques, f"{base_path}.txt", format='edge_list')
            self.save_dataset(graph, test_case['k'], cliques, f"{base_path}. dimacs", format='dimacs')
            
            print(f"  - Nodes: {graph.number_of_nodes()}, Edges: {graph.number_of_edges()}")
            print(f"  - k={test_case['k']}, Cliques: {len(cliques)}")


def main():
    """Main function to demonstrate usage"""
    
    # Initialize generator with seed for reproducibility
    generator = KCliqueDatasetGenerator(seed=42)
    
    # Example 1: Generate a single graph with one k-clique
    print("Example 1: Single graph with k-clique")
    graph, clique = generator.generate_graph_with_clique(n_nodes=30, k=5, edge_probability=0.2)
    print(f"Generated graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges")
    print(f"Clique nodes: {clique}")
    
    # Save the example
    generator.save_dataset(graph, 5, [clique], 'datasets/example_single. json', format='json')
    print("Saved to datasets/example_single.json\n")
    
    # Example 2: Generate a graph with multiple k-cliques
    print("Example 2: Graph with multiple k-cliques")
    graph, cliques = generator. generate_graph_with_multiple_cliques(
        n_nodes=50, k=4, n_cliques=3, edge_probability=0.15
    )
    print(f"Generated graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges")
    print(f"Number of cliques: {len(cliques)}")
    for i, clique in enumerate(cliques):
        print(f"  Clique {i+1}:  {clique}")
    
    generator.save_dataset(graph, 4, cliques, 'datasets/example_multiple.json', format='json')
    print("Saved to datasets/example_multiple.json\n")
    
    # Example 3: Generate full test suite
    print("Example 3: Generating full test suite...")
    generator.generate_test_suite(output_dir='datasets')
    print("\nAll datasets generated successfully!")


if __name__ == "__main__":
    main()