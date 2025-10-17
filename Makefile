CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

all: iot_gateway access_control device_communication emergency_route huffman_compression

iot_gateway:
	$(CC) $(CFLAGS) -o 1_iot_gateway_logs/iot_gateway 1_iot_gateway_logs/iot_gateway.c -lpthread

access_control:
	$(CC) $(CFLAGS) -o 2_access_control_bst/access_control 2_access_control_bst/access_control.c

device_communication:
	$(CC) $(CFLAGS) -o 3_device_communication_graph/device_communication 3_device_communication_graph/device_communication.c

emergency_route:
	$(CC) $(CFLAGS) -o 4_emergency_route_dijkstra/emergency_route 4_emergency_route_dijkstra/emergency_route.c

huffman_compression:
	$(CC) $(CFLAGS) -o 5_huffman_compression/huffman_compression 5_huffman_compression/huffman_compression.c

clean:
	rm -f 1_iot_gateway_logs/iot_gateway
	rm -f 2_access_control_bst/access_control
	rm -f 3_device_communication_graph/device_communication
	rm -f 4_emergency_route_dijkstra/emergency_route
	rm -f 5_huffman_compression/huffman_compression
	rm -f 5_huffman_compression/compressed.txt
	rm -f 5_huffman_compression/decompressed.txt

.PHONY: all clean iot_gateway access_control device_communication emergency_route huffman_compression
