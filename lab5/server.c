/*
Вариант 3
Сервер. 
Создать гнездо без установления соединения домена UNIX. 
Присвоить ему адрес. Послать в клиентское гнездо данные (идентификаторы) 
обо всех активных процессах системы управляемых терминалами. 
Результаты обработки клиентом этих данных распечатать.
*/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

#define SERVER_SOCKET_PATH "/tmp/server.sock"
#define CLIENT_SOCKET_PATH "/tmp/client.sock"
#define BUF_SIZE 2048

int main(int argc, char const* argv[])
{
    int sock_fd;
    struct sockaddr_un client_addr, server_addr;
    char buf[BUF_SIZE];
    socklen_t serv_addrlen = sizeof(server_addr);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path)-1);
    unlink(SERVER_SOCKET_PATH); // удалить старый путь, если есть (Очень много ошибок при дебаге было)

    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Server Socket created

    // Preparing client address.

    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(client_addr.sun_path)-1);
    
    // Client address prepared. Starting main logic

    FILE *fp = popen ("ps -ef", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }

    char info[BUF_SIZE];
    while (fgets(info, sizeof(info), fp) != NULL) {
        if (sendto(sock_fd, info, strlen(info)-1, 0, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
            perror("sendto (to client)");
        }
    }
    sendto(sock_fd, "DONE", 5, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    // puts("Sent: DONE");

    // Get filtered processes
    while (1) {
        struct sockaddr_un src;
        socklen_t src_len = sizeof(src);
        ssize_t len = recvfrom(sock_fd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&src, &src_len);
        if (len < 0) {
            perror("recvfrom");
            break;
        }
        buf[len] = '\0';
        if (strcmp(buf, "DONE") == 0) break;
        printf("From client: %s\n", buf);
    }

    printf("Server: received DONE from client. Exiting.\n");
    
    close(sock_fd);
    unlink(SERVER_SOCKET_PATH);
    return 0;
}