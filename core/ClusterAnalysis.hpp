#include "Lattices.hpp"

#include <vector>

// Function to compute the sizes of clusters in a 2D lattice
template <typename Lattice>
std::vector<int> ClusterSizes(Lattice& lattice) {
    // Get the size of the lattice
    int L = lattice.getSize();
    
    // Initialize visited vector to keep track of visited sites
    std::vector<bool> visited(L * L, false);
    std::vector<int> cluster_sizes;

    // Loop over entire lattice
    for(int x=0;x<L;x++) {
        for(int y=0;y<L;y++) {
        
        // index of the current site (1D)
            int idx = x * L + y;
        
        // Skip site if it has already been visited
        if (visited[idx]) continue;
        
        // Get the spin of the current site
        int spin = lattice.getSpin({x, y});
        
        int cluster_size = 0;
        
        // Depth-First Search (DFS) stack
        std::vector<Point2D> stack;
        stack.push_back({x, y});
        visited[idx] = true;
        
        // DFS loop
        while (!stack.empty()) {
            auto p = stack.back();
            stack.pop_back();
            cluster_size++;

            // Check all neighbors of the current point
            for (auto interaction : lattice.getInteractions(p))
                {
                    Point2D n = interaction.neighbor;
                    // Calculate the index of the neighbor (1D)
                    int nidx = n.x * L + n.y;
                    
                    // If the neighbor has the same spin and has not been visited, add it to the stack
                    if (!visited[nidx] && lattice.getSpin(n) == spin)
                    {
                        // Mark the neighbor as visited and add it to the stack
                        visited[nidx] = true;
                        stack.push_back(n);
                    }
                }
            }
            // After the DFS is complete, we have the size of the cluster
            cluster_sizes.push_back(cluster_size);
        }
    }

    return cluster_sizes;
}


int largestCluster(const std::vector<int>& sizes)
{
    int maxSize = 0;
    for (int s : sizes)
        if (s > maxSize) maxSize = s;

    return maxSize;
}

double meanClusterSize(const std::vector<int>& sizes)
{
    double num = 0.0;
    double den = 0.0;

    for (int s : sizes)
    {
        num += s * s;
        den += s;
    }

    return num / den;
}

double clusterDensity(const std::vector<int>& sizes, int L)
{
    return double(sizes.size()) / (L * L);
}
