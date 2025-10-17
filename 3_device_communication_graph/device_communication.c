#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEVICES 20
#define DEVICE_ID_LEN 10

typedef struct {
    char device_ids[MAX_DEVICES][DEVICE_ID_LEN];
    int adj_matrix[MAX_DEVICES][MAX_DEVICES];
    int device_count;
} Graph;

Graph graph = {{{0}}, {{0}}, 0};

int find_device_index(const char* device_id);
int add_device(const char* device_id);
void add_connection(const char* from, const char* to);
void load_initial_connections();
void display_adjacency_matrix();
void query_device_connections(const char* device_id);
void display_menu();
void add_dynamic_connection();
void remove_connection();
void list_all_devices();

int main() {
    printf("=== IoT Device Communication Mapping Tool ===\n");
    printf("Loading device communication data...\n");
    
    load_initial_connections();
    
    char choice;
    char device_id[DEVICE_ID_LEN];
    
    while (1) {
        display_menu();
        printf("> ");
        scanf(" %c", &choice);
        
        switch (choice) {
            case '1':
                display_adjacency_matrix();
                break;
            case '2':
                printf("Enter device ID to query: ");
                scanf("%s", device_id);
                query_device_connections(device_id);
                break;
            case '3':
                list_all_devices();
                break;
            case '4':
                add_dynamic_connection();
                break;
            case '5':
                remove_connection();
                break;
            case '6':
                printf("System shutdown.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
        printf("\n");
    }
    
    return 0;
}

int find_device_index(const char* device_id) {
    for (int i = 0; i < graph.device_count; i++) {
        if (strcmp(graph.device_ids[i], device_id) == 0) {
            return i;
        }
    }
    return -1;
}

int add_device(const char* device_id) {
    if (graph.device_count >= MAX_DEVICES) {
        printf("Error: Maximum device limit reached\n");
        return -1;
    }
    
    int index = find_device_index(device_id);
    if (index != -1) {
        return index;
    }
    
    strcpy(graph.device_ids[graph.device_count], device_id);
    return graph.device_count++;
}

void add_connection(const char* from, const char* to) {
    int from_idx = add_device(from);
    int to_idx = add_device(to);
    
    if (from_idx != -1 && to_idx != -1) {
        graph.adj_matrix[from_idx][to_idx] = 1;
    }
}

void load_initial_connections() {
    add_connection("D001", "D002");
    add_connection("D001", "D003");
    add_connection("D002", "D004");
    add_connection("D003", "D005");
    add_connection("D004", "D005");
    add_connection("D004", "D006");
    add_connection("D005", "D007");
    add_connection("D006", "D008");
    
    printf("Loaded %d devices with communication links\n", graph.device_count);
}

void display_adjacency_matrix() {
    printf("\n=== Adjacency Matrix ===\n");
    printf("     ");
    for (int i = 0; i < graph.device_count; i++) {
        printf("%5s", graph.device_ids[i]);
    }
    printf("\n");
    
    for (int i = 0; i < graph.device_count; i++) {
        printf("%5s", graph.device_ids[i]);
        for (int j = 0; j < graph.device_count; j++) {
            printf("%5d", graph.adj_matrix[i][j]);
        }
        printf("\n");
    }
}

void query_device_connections(const char* device_id) {
    int device_idx = find_device_index(device_id);
    
    if (device_idx == -1) {
        printf("Error: Device '%s' not found in the network\n", device_id);
        return;
    }
    
    printf("\n=== Device %s Connections ===\n", device_id);
    
    printf("Outgoing connections (sends data to):\n");
    int outgoing_count = 0;
    for (int i = 0; i < graph.device_count; i++) {
        if (graph.adj_matrix[device_idx][i] == 1) {
            printf("  -> %s\n", graph.device_ids[i]);
            outgoing_count++;
        }
    }
    if (outgoing_count == 0) {
        printf("  None\n");
    }
    
    printf("Incoming connections (receives data from):\n");
    int incoming_count = 0;
    for (int i = 0; i < graph.device_count; i++) {
        if (graph.adj_matrix[i][device_idx] == 1) {
            printf("  <- %s\n", graph.device_ids[i]);
            incoming_count++;
        }
    }
    if (incoming_count == 0) {
        printf("  None\n");
    }
    
    printf("Total connections: %d outgoing, %d incoming\n", outgoing_count, incoming_count);
}

void display_menu() {
    printf("\n=== Device Communication Tool ===\n");
    printf("1. Display adjacency matrix\n");
    printf("2. Query device connections\n");
    printf("3. List all devices\n");
    printf("4. Add new connection\n");
    printf("5. Remove connection\n");
    printf("6. Exit\n");
}

void add_dynamic_connection() {
    char from[DEVICE_ID_LEN], to[DEVICE_ID_LEN];
    
    printf("Enter source device ID: ");
    scanf("%s", from);
    printf("Enter destination device ID: ");
    scanf("%s", to);
    
    add_connection(from, to);
    printf("Connection added: %s -> %s\n", from, to);
}

void remove_connection() {
    char from[DEVICE_ID_LEN], to[DEVICE_ID_LEN];
    
    printf("Enter source device ID: ");
    scanf("%s", from);
    printf("Enter destination device ID: ");
    scanf("%s", to);
    
    int from_idx = find_device_index(from);
    int to_idx = find_device_index(to);
    
    if (from_idx == -1 || to_idx == -1) {
        printf("Error: One or both devices not found\n");
        return;
    }
    
    if (graph.adj_matrix[from_idx][to_idx] == 1) {
        graph.adj_matrix[from_idx][to_idx] = 0;
        printf("Connection removed: %s -> %s\n", from, to);
    } else {
        printf("No connection exists between %s and %s\n", from, to);
    }
}

void list_all_devices() {
    printf("\n=== All Devices in Network ===\n");
    for (int i = 0; i < graph.device_count; i++) {
        printf("%d. %s\n", i + 1, graph.device_ids[i]);
    }
    printf("Total devices: %d\n", graph.device_count);
}