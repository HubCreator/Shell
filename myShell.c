#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

#define STD_IN 0
#define STD_OUT 1

int main() {
    char str[512];
    char *command[30];
    char *redirection_command[30];
    int amper = -1;
    int child, pid, status;
    char *splitted;
    int i, fd;

    while(1) {
        printf("$ "); // prompt 띄우기
        fgets(str, sizeof(str), stdin);
        str[strlen(str)-1] = NULL;
        fflush(stdin);

        // exit 입력 시 종료
        if(strcmp(str, "exit") == 0) { 
            exit(1);
        }

        // ###################### 초기화 ###################################
        for(int i = 0; i < strlen(command); i++) { 
            command[i] = NULL;              // command 초기화
            redirection_command[i] = NULL;    // input & output redirection 초기화
        }
        fd = -1;
        dup2(STD_IN, STD_IN);
        dup2(STD_OUT, STD_OUT);
        
        // ################################################################
        // background 처리
        if(strchr(str, '&') != NULL) { // background process일 때
            amper = 1;
        } else {
            amper = 0;
        }

        if((pid = fork()) == 0) { // 자식 프로세스
            // 명령어 처리
            i = 0;
            splitted = strtok(str, " ");
            while (splitted != NULL) {
                command[i] = splitted;
                splitted = strtok(NULL, " ");
                i++;
            }
            command[i] = NULL;

            // for(int i = 0; i < strlen(command); i++) {
            //     printf("command[%d] = |%s|\n", i, command[i]);
            // }

            // TODO : redirection 처리
            for(int i = 0; i < strlen(command); i++) {
                if(command[i] == NULL) break;
                if(strchr(command[i], '>') != NULL) {              // output redirection
                    fd = open(command[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    close(STD_OUT);
                    dup(fd);
                    close(fd);
                    redirection_command[0] = command[0];
                    redirection_command[1] = NULL;
                    execvp(redirection_command[0], redirection_command);
                    printf("execvp error1!\n");
                    exit(1);
                } else if (strchr(command[i], '<') != NULL) {      // input redirection 
                    fd = open(command[i+1], O_RDONLY);
                    close(STD_IN);
                    dup(fd);
                    close(fd);
                    redirection_command[0] = command[0];
                    redirection_command[1] = NULL;
                    execvp(redirection_command[0], redirection_command);
                    printf("execvp error2!\n");
                    exit(1);
                }
            }
            execvp(command[0], command);
            printf("execvp error3!\n");
            exit(1);
        } else if(pid > 0) {
            child = wait(&status); // 자식 프로세스를 기다림
        } else {
            printf("error occured");
        }
    }

    return 0;
}