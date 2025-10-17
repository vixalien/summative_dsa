CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

all: 1_iot_gateway 2_access_control 3_device_communication 4_emergency_route 5_huffman_compression

1_iot_gateway:
	$(CC) $(CFLAGS) -o 1_iot_gateway 1_iot_gateway.c -lpthread

2_access_control:
	$(CC) $(CFLAGS) -o 2_access_control 2_access_control.c

3_device_communication:
	$(CC) $(CFLAGS) -o 3_device_communication 3_device_communication.c

4_emergency_route:
	$(CC) $(CFLAGS) -o 4_emergency_route 4_emergency_route.c

5_huffman_compression:
	$(CC) $(CFLAGS) -o 5_huffman_compression 5_huffman_compression.c

clean:
	rm -f 1_iot_gateway \
	      2_access_control \
	      3_device_communication \
	      4_emergency_route \
	      5_huffman_compression \
	      5_compressed.txt \
	      5_decompressed.txt

.PHONY: all clean 1_iot_gateway 2_access_control 3_device_communication 4_emergency_route 5_huffman_compression
