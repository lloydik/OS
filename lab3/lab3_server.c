// Вариант 3

/*
Выбрать из очереди самое старое сообщение указанного типа.
Определить те текстовые файлы, количество строк в которых превышает 10, и послать об этом сообщение клиенту.
Определить время, когда в очередь было передано самое последнее сообщение.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/msg.h>
#include <ctype.h>

#define MSG_KEY 1234   // ключ для очереди
#define MSG_SIZE 2048   // максимальный размер текста сообщения (да-да, это таск на pwn для ctf)

struct queuedMessage {
    long mtype;          
    char text[MSG_SIZE];
};

int count_lines(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    int count = 0;
    char buf[2048];
    while (fgets(buf, sizeof(buf), fp)) count++;
    fclose(fp);
    return count;
}

int main() {
    int msqid;
    struct queuedMessage clmsg;
    struct queuedMessage srvmsg;
    time_t last_time = 0;

    if ((msqid = msgget(MSG_KEY, IPC_CREAT | 0666)) < 0) {
        perror("msgget error");
        exit(1);
    }

    while (1) {
        if (msgrcv(msqid, &clmsg, sizeof(clmsg.text), 1, 0) == -1) {
            perror("msgrcv error");
            exit(1);
        }

        if (strcmp(clmsg.text, "<") == 0) {
            srvmsg.mtype = 2;
            strcpy(srvmsg.text, "<");
            msgsnd(msqid, &srvmsg, sizeof(srvmsg.text), IPC_NOWAIT);
            printf("Server: last message was recieved in %s", ctime(&last_time));
            // break; // Если надо выключать сервер, то раскоментировать строку
        }
        last_time = time(NULL);
        
        clmsg.text[strlen(clmsg.text)-1] = 0;
        // printf("Text: %s", clmsg.text);
        int lines = count_lines(clmsg.text);
        // printf("File %s has %d rows\n", clmsg.text, lines);
        if (lines > 10) {
            srvmsg.mtype = 2;
            // printf("SEND: File %s has %d rows", clmsg.text, lines);
            snprintf(srvmsg.text, MSG_SIZE, "File %s has %d rows", clmsg.text, lines);
            msgsnd(msqid, &srvmsg, sizeof(srvmsg.text), IPC_NOWAIT);
        }
    }

    return 0;
}
