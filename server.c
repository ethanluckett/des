// The client accepts input from the user, encrypts it,
// and sends it to the server. The server will then decrypt
// and output the message.
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
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

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(54333);

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        printf("Failed to create socket\n");
        exit(1);
    }

    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("Failed to bind socket\n");
        exit(2);
    }

    if (listen(server_sock, 5) < 0) {
        printf("Failed to open socket for listening\n");
        exit(3);
    }

    struct sockaddr_in client_addr;
    int client_len = 0;

    while (1) {
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock == -1) {
            printf("Failed to create accept socket\n");
            exit(1);
        }

        int n = 0, max_len = 1024;
        char buf[max_len];

        while ((n = recv(client_sock, buf, max_len, 0)) > 0) {
            for (int offset = 0; offset < n; offset += 8) {
                uint64_t block;
                memcpy(&block, buf+offset, 8);
                uint64_t dec_block = des(block, key, 'd');
                // print out one byte at a time since 
                // little endian reverses the byte order
                for (int i = 0; i < 8; i++) {
                    char *byte = ((char*)&dec_block)+i;
                    // values <= 8 are padding bytes; don't print these
                    if (*byte > 0x8) {
                        printf("%c", *byte);
                    }
                }
            }
            memset(buf, 0, max_len);
        }

        close(client_sock);
        printf("Client disconnected\n");
    }

    close(server_sock);

}