#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int main() {
    char str[512];
    char *command[30];
    int amper = -1;
    int child, pid, status;
    char *tmp;
    int i;

    while(1) {
        printf("$ ");
        fgets(str, sizeof(str), stdin);
        str[strlen(str)-1] = NULL;
        fflush(stdin);
        for(int i = 0; i < strlen(command); i++) {
            command[i] = NULL;
        }

        if(strcmp(str, "exit") == 0) { // exit 입력 시 종료
            exit(1);
        }
        // background 처리
        if(strchr(str, '&') != NULL) { // background process일 때
            amper = 1;
        } else {
            amper = 0;
        }

        i = 0;
        tmp = strtok(str, " ");
        while (tmp != NULL) {
            // printf("tmp : %s\n", tmp);
            command[i] = tmp;
            tmp = strtok(NULL, " ");
            i++;
        }
        command[i] = NULL;

        if((pid = fork()) == 0) { // 자식 프로세스
            execvp(command[0], command);
            printf("execp error!\n");
            exit(1);
        } else if(pid > 0) {
            child = wait(&status); // 자식 프로세스를 기다림
        } else {
            printf("error occured");
        }
    }

    return 0;
}