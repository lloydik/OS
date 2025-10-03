// Вариант 3

/*
Создать очередь сообщений. 
Передать в эту очередь информацию (имена) о текстовых файлах текущего каталога. 
Вывести на экран ответы сервера
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/msg.h>

#define MSG_KEY 1234   // ключ для очереди
#define MSG_SIZE 2048   // максимальный размер текста сообщения (да-да, это таск на pwn для ctf)

struct queuedMessage {
    long mtype;          
    char text[MSG_SIZE];
};

int main() {
    int msqid;
    struct dirent *entry;
    DIR *dp;

    if ((msqid = msgget(MSG_KEY, IPC_CREAT | 0666)) < 0) {
        perror("msgget error");
        exit(1);
    }

    FILE *fp = popen ("file * | grep text | cut -d ':' -f 1", "r");
    if (fp == NULL) {
        printf("Failed to run command\n" );
        exit(1);
    }
    char info[MSG_SIZE];
    while (fgets(info, sizeof(info), fp) != NULL) {
        struct queuedMessage  msg;
        msg.mtype = 1;
        strcpy(msg.text, info);
        if (msgsnd(msqid, &msg, sizeof(msg.text), 0) == -1) {
            perror("msgsnd error");
        }
    }
    pclose(fp);

    struct queuedMessage endmsg;
    endmsg.mtype = 1;
    strcpy(endmsg.text, "<"); // Типа конец. Файл не может называться с таким символом
    msgsnd(msqid, &endmsg, sizeof(endmsg.text), 0);

    // читаем ответы от сервера
    struct queuedMessage srvmsg;
    while (1) {
        if (msgrcv(msqid, &srvmsg, sizeof(srvmsg.text), 2, 0) < 0) {
            perror("msgrcv error");
            break;
        }
        if (strcmp(srvmsg.text, "<") == 0) {// Типа конец. Файл не может называться с таким символом
            puts("End of work");
            break;
        }
        printf("Server says: %s\n", srvmsg.text);
    }

    return 0;
}
