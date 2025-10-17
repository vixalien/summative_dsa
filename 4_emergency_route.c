#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAX_NODES 10
#define INF INT_MAX

const char* locations[MAX_NODES] = {
    "Dispatch Center", "Sector A", "Sector B", "Sector D",
    "Emergency Site", "Junction C", "Sector E"
};

int adj_matrix[MAX_NODES][MAX_NODES];
int node_count = 7;

int find_location_index(const char* location) {
    for (int i = 0; i < node_count; i++) {
        if (strcmp(locations[i], location) == 0) {
            return i;
        }
    }
    return -1;
}

void add_road(const char* from, const char* to, int time) {
    int from_idx = find_location_index(from);
    int to_idx = find_location_index(to);
    
    if (from_idx != -1 && to_idx != -1) {
        adj_matrix[from_idx][to_idx] = time;
        adj_matrix[to_idx][from_idx] = time;
    }
}

void dijkstra(int start, int dist[], int prev[]) {
    int visited[MAX_NODES] = {0};
    
    for (int i = 0; i < node_count; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    
    dist[start] = 0;
    
    for (int i = 0; i < node_count; i++) {
        int min_dist = INF;
        int u = -1;
        
        for (int v = 0; v < node_count; v++) {
            if (!visited[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }
        
        if (u == -1) break;
        visited[u] = 1;
        
        for (int v = 0; v < node_count; v++) {
            if (adj_matrix[u][v] > 0 && !visited[v]) {
                int new_dist = dist[u] + adj_matrix[u][v];
                if (new_dist < dist[v]) {
                    dist[v] = new_dist;
                    prev[v] = u;
                }
            }
        }
    }
}

void print_path(int prev[], int start, int end) {
    if (end == start) {
        printf("%s", locations[start]);
        return;
    }
    
    if (prev[end] == -1) {
        printf("No path found");
        return;
    }
    
    print_path(prev, start, prev[end]);
    printf(" -> %s", locations[end]);
}

void initialize_graph() {
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            adj_matrix[i][j] = 0;
        }
    }
    
    add_road("Dispatch Center", "Sector A", 10);
    add_road("Dispatch Center", "Sector D", 30);
    add_road("Sector A", "Sector B", 10);
    add_road("Sector B", "Emergency Site", 15);
    add_road("Sector D", "Emergency Site", 5);
    add_road("Sector B", "Junction C", 3);
    add_road("Junction C", "Sector E", 6);
    add_road("Sector E", "Emergency Site", 4);
}

int main() {
    printf("Emergency Route Optimization\n");
    
    initialize_graph();
    
    char start_location[50];
    printf("\nEnter starting location: ");
    fflush(stdout);
    fgets(start_location, sizeof(start_location), stdin);
    start_location[strcspn(start_location, "\n")] = 0;
    
    int start_idx = find_location_index(start_location);
    int end_idx = find_location_index("Emergency Site");
    
    if (start_idx == -1) {
        printf("Location not found\n");
        return 1;
    }
    
    int dist[MAX_NODES], prev[MAX_NODES];
    dijkstra(start_idx, dist, prev);
    
    printf("\nOptimal route: ");
    print_path(prev, start_idx, end_idx);
    printf("\nTotal travel time: %d minutes\n", dist[end_idx]);
    
    return 0;
}