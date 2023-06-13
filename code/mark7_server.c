#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define MAX_QUEUE 5
#define MAX_BUFFER 512

int sock;
int queue = 0;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t barber_mutex = PTHREAD_MUTEX_INITIALIZER;

void *worker(void *arg) {
    struct sockaddr_in *client_addr = (struct sockaddr_in *)arg;
    char buffer[MAX_BUFFER] = "Serving";

    pthread_mutex_lock(&barber_mutex);
    printf("Barber: Cutting hair\n");
    sleep(2 + (rand() % 3));
    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
    pthread_mutex_unlock(&barber_mutex);

    pthread_mutex_lock(&queue_mutex);
    queue--;
    pthread_mutex_unlock(&queue_mutex);

    free(arg);
    return NULL;
}

void handle_signal(int signal) {
    close(sock);
    printf("\nServer stopped\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char buffer[MAX_BUFFER];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    int nBytes;

    signal(SIGINT, handle_signal);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Cannot create socket");
        return 0;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 0;
    }

    addr_size = sizeof(struct sockaddr_in);

    while(1) {
        pthread_mutex_lock(&queue_mutex);
        printf("Current queue size: %d\n", queue);
        pthread_mutex_unlock(&queue_mutex);

        nBytes = recvfrom(sock, buffer, MAX_BUFFER, 0, (struct sockaddr *)&client_addr, &addr_size);

        if (nBytes > 0) {
            buffer[nBytes] = '\0';
            printf("Received from client: %s\n", buffer);

            if (strcmp(buffer, "Need a haircut") == 0) {
                pthread_mutex_lock(&queue_mutex);
                if (queue < MAX_QUEUE) {
                    queue++;
                    struct sockaddr_in *client_addr_cpy = malloc(sizeof(struct sockaddr_in));
                    memcpy(client_addr_cpy, &client_addr, sizeof(struct sockaddr_in));
                    pthread_t thread_id;
                    pthread_create(&thread_id, NULL, &worker, client_addr_cpy);
                    pthread_detach(thread_id);
                } else {
                    strcpy(buffer, "Rejected");
                    sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_size);
                }
                pthread_mutex_unlock(&queue_mutex);
            } else if (strcmp(buffer, "Queue status") == 0) {
                pthread_mutex_lock(&queue_mutex);
                sprintf(buffer, "Queue size: %d", queue);
                pthread_mutex_unlock(&queue_mutex);
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_size);
            }
        }
    }

    return 0;
}
