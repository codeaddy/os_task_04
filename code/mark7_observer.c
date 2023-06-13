#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_BUFFER 512

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char buffer[MAX_BUFFER];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int sock, nBytes;

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Cannot create socket");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    while(1) {
        printf("Entered while\n");
        strcpy(buffer, "Queue status");

        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        addr_size = sizeof(struct sockaddr_in);
        nBytes = recvfrom(sock, buffer, MAX_BUFFER, 0, (struct sockaddr *)&client_addr, &addr_size);
        buffer[nBytes] = '\0';

        printf("Server Response: %s\n", buffer);
        sleep(5);
    }

    return 0;
}
