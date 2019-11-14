// The client accepts input from the user, encrypts it,
// and sends it to the server. The server will then decrypt
// and output the message.
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "des.h"

void usage(char *argv0) {
    printf("Usage: %s -K <hex key\n", argv0);
    exit(0);
}

int main(int argc, char *argv[]) {
    uint64_t key = 0;
    int opt;

    while ((opt = getopt(argc, argv, "K:")) != -1) {
        switch (opt) {
            case 'K':
                key = strtoull(optarg, NULL, 16);
                break;
            default:
                usage(argv[0]);
        }
    }

    if (optind < 3) {
        usage(argv[0]);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Failed to create socket\n");
        exit(1);
    }
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(54333);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        printf("Failed to connect to server\n");
        exit(2);
    }

    size_t max_len = 1024;
    char *buf = calloc(max_len, sizeof(char));
    int len;

    while (1) {
        getline(&buf, &max_len, stdin);
        len = strlen(buf);
        if (len > 0) {
            int num_blocks = len / 8 + 1;
            int msg_len = num_blocks * 8;
            char *msg_padded = calloc(msg_len, sizeof(char));
            pad_input(msg_padded, buf, len, msg_len);

            char *msg_enc = calloc(msg_len, sizeof(char));
            for (int i = 0; i < num_blocks; i++) {
                uint64_t *input = ((uint64_t*)msg_padded)+i;
                uint64_t enc_block = des(*input, key, 'e');
                memcpy(msg_enc+(i*8), &enc_block, 8);
            }

            send(sock, msg_enc, msg_len, 0);

            free(msg_padded);
            free(msg_enc);
        }

    }

    close(sock);
}