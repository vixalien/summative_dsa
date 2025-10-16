#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_NAMES 40
#define SIMILARITY_THRESHOLD 3

typedef struct BSTNode {
    char name[MAX_NAME_LENGTH];
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct {
    char name[MAX_NAME_LENGTH];
    int distance;
} NameMatch;

BSTNode* root = NULL;
FILE* log_file = NULL;

BSTNode* create_node(const char* name);
BSTNode* insert_bst(BSTNode* node, const char* name);
BSTNode* search_bst(BSTNode* node, const char* name);
void load_authorized_names();
int levenshtein_distance(const char* s1, const char* s2);
void find_closest_match(BSTNode* node, const char* input, NameMatch* best_match);
void verify_access(const char* input_name);
void log_unauthorized_access(const char* name);
void cleanup_bst(BSTNode* node);
int min(int a, int b, int c);

int main() {
    printf("=== Smart Access Control System ===\n");
    printf("Loading authorized personnel database...\n");

    log_file = fopen("access_log.txt", "a");
    if (!log_file) {
        printf("Warning: Could not open log file\n");
    }

    load_authorized_names();

    char input_name[MAX_NAME_LENGTH];

    printf("\nAccess Control Terminal Ready\n");
    printf("Enter 'exit' to quit\n\n");

    while (1) {
        printf("Enter name: ");
        if (!fgets(input_name, sizeof(input_name), stdin)) {
            break;
        }

        input_name[strcspn(input_name, "\n")] = 0;

        if (strcmp(input_name, "exit") == 0) {
            break;
        }

        if (strlen(input_name) == 0) {
            continue;
        }

        verify_access(input_name);
        printf("\n");
    }

    cleanup_bst(root);
    if (log_file) {
        fclose(log_file);
    }

    printf("System shutdown complete.\n");
    return 0;
}

BSTNode* create_node(const char* name) {
    BSTNode* node = (BSTNode*)malloc(sizeof(BSTNode));
    if (!node) return NULL;

    strcpy(node->name, name);
    node->left = NULL;
    node->right = NULL;
    return node;
}

BSTNode* insert_bst(BSTNode* node, const char* name) {
    if (node == NULL) {
        return create_node(name);
    }

    int cmp = strcmp(name, node->name);
    if (cmp < 0) {
        node->left = insert_bst(node->left, name);
    } else if (cmp > 0) {
        node->right = insert_bst(node->right, name);
    }

    return node;
}

BSTNode* search_bst(BSTNode* node, const char* name) {
    if (node == NULL || strcmp(name, node->name) == 0) {
        return node;
    }

    if (strcmp(name, node->name) < 0) {
        return search_bst(node->left, name);
    }

    return search_bst(node->right, name);
}

void load_authorized_names() {
    FILE* file = fopen("authorized_names.txt", "r");
    if (!file) {
        printf("Error: Could not open authorized_names.txt\n");
        return;
    }

    char name[MAX_NAME_LENGTH];
    int count = 0;

    while (fgets(name, sizeof(name), file) && count < MAX_NAMES) {
        name[strcspn(name, "\n")] = 0;

        if (strlen(name) > 0) {
            root = insert_bst(root, name);
            count++;
        }
    }

    fclose(file);
    printf("Loaded %d authorized personnel names\n", count);
}

int min(int a, int b, int c) {
    int min_val = a;
    if (b < min_val) min_val = b;
    if (c < min_val) min_val = c;
    return min_val;
}

int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        matrix[0][j] = j;
    }

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (s1[i-1] == s2[j-1]) {
                matrix[i][j] = matrix[i-1][j-1];
            } else {
                matrix[i][j] = min(
                    matrix[i-1][j] + 1,    // deletion
                    matrix[i][j-1] + 1,    // insertion
                    matrix[i-1][j-1] + 1   // substitution
                );
            }
        }
    }

    return matrix[len1][len2];
}

void find_closest_match(BSTNode* node, const char* input, NameMatch* best_match) {
    if (node == NULL) return;

    int distance = levenshtein_distance(input, node->name);
    if (distance < best_match->distance) {
        best_match->distance = distance;
        strcpy(best_match->name, node->name);
    }

    find_closest_match(node->left, input, best_match);
    find_closest_match(node->right, input, best_match);
}

void verify_access(const char* input_name) {
    BSTNode* exact_match = search_bst(root, input_name);

    if (exact_match) {
        printf("ACCESS GRANTED\n");
        printf("Welcome, %s!\n", input_name);
        return;
    }

    NameMatch best_match;
    best_match.distance = 999;
    strcpy(best_match.name, "");

    find_closest_match(root, input_name, &best_match);

    if (best_match.distance <= SIMILARITY_THRESHOLD && strlen(best_match.name) > 0) {
        printf("ACCESS DENIED\n");
        printf("Did you mean: %s?\n", best_match.name);
    } else {
        printf("ACCESS DENIED\n");
        printf("Name not recognized. Access logged for review.\n");
        log_unauthorized_access(input_name);
    }
}

void log_unauthorized_access(const char* name) {
    if (log_file) {
        time_t now = time(NULL);
        char* timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = 0;

        fprintf(log_file, "[%s] UNAUTHORIZED ACCESS ATTEMPT: %s\n", timestamp, name);
        fflush(log_file);
    }
}

void cleanup_bst(BSTNode* node) {
    if (node == NULL) return;

    cleanup_bst(node->left);
    cleanup_bst(node->right);
    free(node);
}
