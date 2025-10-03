/*
Вариант - 3
Клиент. 
Создать гнездо без установления соединения домена UNIX.
Отфильтровать информацию из серверного гнезда с целью выявления тех процессов, 
которые принадлежат данному пользователю. 
Результаты обработки передать в серверное гнездо.
*/
#include <arpa/inet.h>
#include <stdio.h>
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
    

   // Creating socket
    if ((sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    // reparing client socket
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sun_family = AF_UNIX;
    strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(client_addr.sun_path)-1);
    unlink(CLIENT_SOCKET_PATH);

    if (bind(sock_fd, (struct sockaddr*)&client_addr, sizeof(client_addr)) < 0) {
        perror("bind failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Client socket ready

    // Preparing server socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path)-1);

    printf("Client: waiting for messages from server...\n");
  
    
    char usr[32];
    FILE *fp = popen ("whoami", "r");
    if (fp == NULL) {
        printf("Failed to get current user\n" );
        exit(1);
    }
    char cmd_out[BUF_SIZE];
    while((fgets(cmd_out, BUF_SIZE , fp)) != NULL){
        strncpy(usr, cmd_out, strlen(cmd_out)-1);
    }

    while (1) {
        struct sockaddr_un src;
        socklen_t src_len = sizeof(src);
        char buf[BUF_SIZE];
        ssize_t len = recvfrom(sock_fd, buf, sizeof(buf)-1, 0, (struct sockaddr*)&src, &src_len);
        if (len < 0) {
            perror("recvfrom");
            break;
        }
        buf[len] = '\0';
        if (strcmp(buf, "DONE") == 0) {
            break;
        }
        if (strstr(buf, usr) != NULL) {
            if (sendto(sock_fd, buf, strlen(buf)+1, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("sendto (to server)");
            }
            // printf("Sent: %s\n", buf);
        }
    }
    sendto(sock_fd, "DONE", 5, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    // closing the connected socket
    close(sock_fd);
    unlink(CLIENT_SOCKET_PATH);
    return 0;
}