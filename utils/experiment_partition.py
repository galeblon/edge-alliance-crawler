import networkx as nx
import matplotlib.pyplot as plt
import subprocess
import numpy as np
import csv
import math
import itertools

num_of_cliques = 10

for i in range(10):
    with open("experiments_partition_{0}.csv".format(i), 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
        csvwriter.writerow(["Clique Size", "Rewire chance", "Seed size", "Mean community size"])
        for clique_size in range(10, 100, 10):
            for rewiring in np.arange(0.005, 0.05, 0.005):
                graph = nx.random_partition_graph([clique_size, 100], 1.0-rewiring, rewiring)
                nx.write_graph6(graph, "tmp.g6", header=False)
                for seed_size in range(1, (int)(math.sqrt(clique_size)), 1):
                    mean_result = 0.0
                    #combinations = list(itertools.combinations(range(1, clique_size), seed_size))
                    #for combination in combinations:
                    args = "def_alliance.exe tmp.g6 {0} {1}".format("4", ' '.join(str(i) for i in range(0, seed_size, 1)))
                    result = subprocess.check_output(args)
                    
                    #print(result)
                    #pos = nx.spring_layout(graph)
                    #nx.draw_networkx_nodes(graph, pos, cmap=plt.get_cmap('jet'), node_size=100)
                    #nx.draw_networkx_labels(graph, pos, font_size=8)
                    #nx.draw_networkx_edges(graph, pos, arrows=False)
                    #plt.show()
                    
                    mean_result = mean_result + len(result.splitlines())
                    #mean_result = mean_result / len(combinations)
                    print(' '.join(str(i) for i in [clique_size, rewiring, seed_size, mean_result]))
                    csvwriter.writerow([clique_size, rewiring, seed_size, mean_result])

 