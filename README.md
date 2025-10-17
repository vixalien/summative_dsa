# Summative DSA Project

Data structures and algorithms implementation covering doubly linked lists, binary search trees, graphs, shortest path algorithms, and compression techniques.

## Projects

1. **IoT Gateway Logs** - Doubly linked list for sensor data with live streaming and session persistence
2. **Access Control BST** - Binary search tree with string similarity for access control
3. **Device Communication Graph** - Graph adjacency matrix for IoT device communication mapping
4. **Emergency Route Dijkstra** - Shortest path algorithm for emergency response routing
5. **Huffman Compression** - Lossless file compression using Huffman coding

## Build & Run

```bash
# Build all
make

# Build individual programs
make iot_gateway
make access_control
make device_communication
make emergency_route
make huffman_compression

# Clean
make clean
```

Run programs from their respective directories:
```bash
cd 1_iot_gateway_logs && ./iot_gateway
cd 2_access_control_bst && ./access_control
cd 3_device_communication_graph && ./device_communication
cd 4_emergency_route_dijkstra && ./emergency_route
cd 5_huffman_compression && ./huffman_compression
```

## Features

### 1. IoT Gateway (Doubly Linked Lists)
- Circular buffer with max 20 sensor logs
- Live data streaming with threading
- Navigation: `n` next, `p` previous, `y` live mode, `z` pause
- Session persistence with auto save/load
- Sensors: temperature, humidity, pressure, vibration

### 2. Access Control (Binary Search Tree)
- Case-sensitive name matching from `authorized_names.txt`
- Typo detection using Levenshtein distance
- Security logging to `access_log.txt`
- BST-based O(log n) lookup performance

### 3. Device Communication (Graphs)
- Directed graph with adjacency matrix
- Device connection mapping and visualization
- Query incoming/outgoing connections
- Visual matrix display

### 4. Emergency Route (Dijkstra's Algorithm)
- Shortest path calculation for emergency response
- Dynamic travel times between locations
- Priority queue implementation
- Route optimization with total time calculation

### 5. Huffman Compression (Trees & Compression)
- Lossless compression/decompression
- Frequency analysis and optimal encoding
- File operations: compress → `compressed.huff`, decompress → `decompressed.txt`
- Compression statistics and ratio reporting

## Data Structures

- **Doubly Linked List**: Bidirectional navigation with O(1) insertion/deletion
- **Binary Search Tree**: O(log n) average search with string operations
- **Adjacency Matrix**: O(1) edge lookup for device connections
- **Priority Queue**: Dijkstra's algorithm implementation
- **Huffman Tree**: Optimal compression tree construction

## Files Generated

- `session_state.txt` - IoT gateway session persistence
- `access_log.txt` - Access control security log
- `compressed.huff` - Huffman compressed output
- `tree.bin` - Huffman encoding tree
- `decompressed.txt` - Decompressed file verification

## Requirements

- C99 compiler (gcc)
- pthread library for IoT gateway threading
- Unix/Linux environment