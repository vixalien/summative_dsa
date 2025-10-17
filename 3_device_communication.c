#include <stdio.h>
#include <string.h>

#define MAX_DEVICES 10

const char *devices[MAX_DEVICES] = {"D001", "D002", "D003", "D004",
                                    "D005", "D006", "D007", "D008"};

int adj_matrix[MAX_DEVICES][MAX_DEVICES] = {0};
int device_count = 8;

int find_device_index(const char *device_id) {
  for (int i = 0; i < device_count; i++) {
    if (strcmp(devices[i], device_id) == 0) {
      return i;
    }
  }
  return -1;
}

void add_connection(const char *from, const char *to) {
  int from_idx = find_device_index(from);
  int to_idx = find_device_index(to);

  if (from_idx != -1 && to_idx != -1) {
    adj_matrix[from_idx][to_idx] = 1;
  }
}

void display_adjacency_matrix() {
  printf("\nAdjacency Matrix:\n     ");
  for (int i = 0; i < device_count; i++) {
    printf("%5s", devices[i]);
  }
  printf("\n");

  for (int i = 0; i < device_count; i++) {
    printf("%5s", devices[i]);
    for (int j = 0; j < device_count; j++) {
      printf("%5d", adj_matrix[i][j]);
    }
    printf("\n");
  }
}

void query_device_connections(const char *device_id) {
  int device_idx = find_device_index(device_id);

  if (device_idx == -1) {
    printf("Device '%s' not found\n", device_id);
    fflush(stdout);
    return;
  }

  printf("\nDevice %s:\n", device_id);

  printf("Outgoing: ");
  int has_outgoing = 0;
  for (int i = 0; i < device_count; i++) {
    if (adj_matrix[device_idx][i] == 1) {
      printf("%s ", devices[i]);
      has_outgoing = 1;
    }
  }
  if (!has_outgoing)
    printf("None");
  printf("\n");

  printf("Incoming: ");
  int has_incoming = 0;
  for (int i = 0; i < device_count; i++) {
    if (adj_matrix[i][device_idx] == 1) {
      printf("%s ", devices[i]);
      has_incoming = 1;
    }
  }
  if (!has_incoming)
    printf("None");
  printf("\n");
  fflush(stdout);
}

int main() {
  printf("IoT Device Communication Tool\n");

  add_connection("D001", "D002");
  add_connection("D001", "D003");
  add_connection("D002", "D004");
  add_connection("D003", "D005");
  add_connection("D004", "D005");
  add_connection("D004", "D006");
  add_connection("D005", "D007");
  add_connection("D006", "D008");

  display_adjacency_matrix();

  char device_id[10];
  while (1) {
    printf("\nEnter device ID (or 'exit'): ");
    fflush(stdout);
    scanf("%s", device_id);

    if (strcmp(device_id, "exit") == 0) {
      break;
    }

    query_device_connections(device_id);
  }

  printf("System shutdown.\n");
  return 0;
}
