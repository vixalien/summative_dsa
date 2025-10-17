#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HEIGHT 256

typedef struct HuffmanNode {
  char data;
  unsigned freq;
  struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct MinHeap {
  unsigned size;
  unsigned capacity;
  HuffmanNode **array;
} MinHeap;

HuffmanNode *newNode(char data, unsigned freq) {
  HuffmanNode *temp = (HuffmanNode *)malloc(sizeof(HuffmanNode));
  temp->left = temp->right = NULL;
  temp->data = data;
  temp->freq = freq;
  return temp;
}

MinHeap *createMinHeap(unsigned capacity) {
  MinHeap *minHeap = (MinHeap *)malloc(sizeof(MinHeap));
  minHeap->size = 0;
  minHeap->capacity = capacity;
  minHeap->array =
      (HuffmanNode **)malloc(minHeap->capacity * sizeof(HuffmanNode *));

  return minHeap;
}

void swapNodes(HuffmanNode **a, HuffmanNode **b) {
  HuffmanNode *t = *a;
  *a = *b;
  *b = t;
}

void minHeapify(MinHeap *minHeap, int idx) {
  int smallest = idx;
  int left = 2 * idx + 1;
  int right = 2 * idx + 2;

  if (left < minHeap->size &&
      minHeap->array[left]->freq < minHeap->array[smallest]->freq)
    smallest = left;

  if (right < minHeap->size &&
      minHeap->array[right]->freq < minHeap->array[smallest]->freq)
    smallest = right;

  if (smallest != idx) {
    swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
    minHeapify(minHeap, smallest);
  }
}

int isSizeOne(MinHeap *minHeap) { return (minHeap->size == 1); }

HuffmanNode *extractMin(MinHeap *minHeap) {
  HuffmanNode *temp = minHeap->array[0];
  minHeap->array[0] = minHeap->array[--minHeap->size];
  minHeapify(minHeap, 0);
  return temp;
}

void insertMinHeap(MinHeap *minHeap, HuffmanNode *node) {
  int i = minHeap->size++;
  while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
    minHeap->array[i] = minHeap->array[(i - 1) / 2];
    i = (i - 1) / 2;
  }
  minHeap->array[i] = node;
}

void buildMinHeap(MinHeap *minHeap) {
  int n = minHeap->size - 1;
  for (int i = (n - 1) / 2; i >= 0; i--)
    minHeapify(minHeap, i);
}

int isLeaf(HuffmanNode *root) { return !(root->left) && !(root->right); }

MinHeap *buildAndCreateHeap(char data[], int freq[], int size) {
  MinHeap *minHeap = createMinHeap(size);
  for (int i = 0; i < size; ++i)
    minHeap->array[i] = newNode(data[i], freq[i]);
  minHeap->size = size;
  buildMinHeap(minHeap);
  return minHeap;
}

HuffmanNode *buildTree(char data[], int freq[], int size) {
  HuffmanNode *left, *right, *top;
  MinHeap *minHeap = buildAndCreateHeap(data, freq, size);
  while (!isSizeOne(minHeap)) {
    left = extractMin(minHeap);
    right = extractMin(minHeap);
    top = newNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;
    insertMinHeap(minHeap, top);
  }
  return extractMin(minHeap);
}

void storeCodes(HuffmanNode *root, int arr[], int top, char *codes[256]) {
  if (root->left) {
    arr[top] = 0;
    storeCodes(root->left, arr, top + 1, codes);
  }
  if (root->right) {
    arr[top] = 1;
    storeCodes(root->right, arr, top + 1, codes);
  }
  if (isLeaf(root)) {
    codes[(unsigned char)root->data] = (char *)malloc(top + 1);
    for (int i = 0; i < top; ++i)
      codes[(unsigned char)root->data][i] = arr[i] + '0';
    codes[(unsigned char)root->data][top] = '\0';
  }
}

void writeTree(FILE *file, HuffmanNode *root) {
  if (isLeaf(root)) {
    fputc('1', file);
    fputc(root->data, file);
  } else {
    fputc('0', file);
    writeTree(file, root->left);
    writeTree(file, root->right);
  }
}

HuffmanNode *readTree(FILE *file) {
  int flag = fgetc(file);
  if (flag == '1') {
    HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
    node->data = fgetc(file);
    node->left = node->right = NULL;
    return node;
  }
  HuffmanNode *node = (HuffmanNode *)malloc(sizeof(HuffmanNode));
  node->data = '\0';
  node->left = readTree(file);
  node->right = readTree(file);
  return node;
}

long getFileSize(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) return 0;
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fclose(file);
  return size;
}

void compressFile() {
  long original_size = getFileSize("patient_record.txt");
  printf("Original file size: %ld bytes\n", original_size);

  FILE *in = fopen("patient_record.txt", "r");
  if (!in) {
    printf("Error reading input file.\n");
    exit(1);
  }

  int freq[256] = {0};
  char ch;
  while ((ch = fgetc(in)) != EOF)
    freq[(unsigned char)ch]++;
  fseek(in, 0, SEEK_SET);

  int size = 0;
  char data[256];
  int frequencies[256];
  for (int i = 0; i < 256; i++) {
    if (freq[i]) {
      data[size] = i;
      frequencies[size] = freq[i];
      size++;
    }
  }

  HuffmanNode *root = buildTree(data, frequencies, size);
  char *codes[256] = {NULL};
  int arr[MAX_TREE_HEIGHT], top = 0;
  storeCodes(root, arr, top, codes);

  FILE *out = fopen("compressed.txt", "wb");
  FILE *tree = fopen("tree.bin", "wb");
  writeTree(tree, root);
  fclose(tree);

  unsigned char buffer = 0;
  int bitCount = 0;
  while ((ch = fgetc(in)) != EOF) {
    char *code = codes[(unsigned char)ch];
    for (int i = 0; code[i]; i++) {
      buffer <<= 1;
      if (code[i] == '1')
        buffer |= 1;
      bitCount++;
      if (bitCount == 8) {
        fwrite(&buffer, 1, 1, out);
        bitCount = 0;
        buffer = 0;
      }
    }
  }
  if (bitCount) {
    buffer <<= (8 - bitCount);
    fwrite(&buffer, 1, 1, out);
  }

  fclose(in);
  fclose(out);

  long compressed_size = getFileSize("compressed.txt");
  printf("Compressed file size: %ld bytes\n", compressed_size);
  printf("Compression complete: patient_record.txt → compressed.txt\n");
  
  if (compressed_size < original_size) {
    printf("Compression ratio: %.1f%%\n", 
           ((float)(original_size - compressed_size) / original_size) * 100);
  }
}

void decompressFile() {
  long compressed_size = getFileSize("compressed.txt");
  printf("Compressed file size: %ld bytes\n", compressed_size);

  FILE *in = fopen("compressed.txt", "rb");
  FILE *tree = fopen("tree.bin", "rb");
  FILE *out = fopen("decompressed.txt", "w");

  if (!in || !tree || !out) {
    printf("Error opening files.\n");
    exit(1);
  }

  HuffmanNode *root = readTree(tree);
  HuffmanNode *current = root;

  int byte;
  while ((byte = fgetc(in)) != EOF) {
    for (int i = 7; i >= 0; i--) {
      int bit = (byte >> i) & 1;
      current = bit ? current->right : current->left;

      if (!current->left && !current->right) {
        fputc(current->data, out);
        current = root;
      }
    }
  }

  fclose(in);
  fclose(tree);
  fclose(out);

  long decompressed_size = getFileSize("decompressed.txt");
  printf("Decompressed file size: %ld bytes\n", decompressed_size);
  printf("Decompression complete: compressed.txt → decompressed.txt\n");
}

int compareFiles() {
  FILE* original = fopen("patient_record.txt", "r");
  FILE* decompressed = fopen("decompressed.txt", "r");
  
  if (!original || !decompressed) {
    printf("Error opening files for comparison\n");
    if (original) fclose(original);
    if (decompressed) fclose(decompressed);
    return 0;
  }

  int ch1, ch2;
  do {
    ch1 = fgetc(original);
    ch2 = fgetc(decompressed);
  } while (ch1 == ch2 && ch1 != EOF);

  fclose(original);
  fclose(decompressed);

  if (ch1 == ch2) {
    printf("Files match exactly - lossless compression verified\n");
    return 1;
  } else {
    printf("Files do not match\n");
    return 0;
  }
}

int main() {
  int choice;
  printf("===== Huffman Compression Tool =====\n");
  printf("1. Compress 'patient_record.txt'\n");
  printf("2. Decompress 'compressed.txt'\n");
  printf("Enter your choice (1 or 2): ");
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    compressFile();
    break;
  case 2:
    decompressFile();
    break;
  default:
    printf("Invalid choice.\n");
  }

  return 0;
}