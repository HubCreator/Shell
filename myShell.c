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
    int child, pid1, pid2, status;
    char *splitted;
    int i, fd, fd_pipe[2];

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
        amper = -1;
        // dup2(STD_IN, STD_IN);
        // dup2(STD_OUT, STD_OUT);
        
        // ################################################################
        
        // background 처리
        if(strchr(str, '&') != NULL) { // background process일 때
            amper = 1;
        } else {
            amper = 0;
        }

        if((pid1 = fork()) == 0) { // 자식 - 실제 명령어 처리 부분
            i = 0;
            splitted = strtok(str, " ");
            while (splitted != NULL) {  // 명령어 처리 (띄어쓰기 단위로 split)
                command[i] = splitted; // 각 인덱스에 명령어를 분리해서 저장
                splitted = strtok(NULL, " ");
                i++;
            }
            command[i] = NULL;

            // TODO : redirection 처리
            for(int i = 0; i < strlen(command); i++) {
                if(command[i] == NULL) break;

                // output redirection
                if(strchr(command[i], '>') != NULL) {
                    fd = open(command[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    close(STD_OUT);
                    dup(fd);
                    close(fd);
                    redirection_command[0] = command[0];
                    redirection_command[1] = NULL;
                    execvp(redirection_command[0], redirection_command);
                    printf("execvp error1!\n");
                    exit(1);
                } 
                // input redirection 
                else if (strchr(command[i], '<') != NULL) {
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
                // pipe
                else if (strchr(command[i], '|') != NULL) {
                    if(pipe(fd_pipe) == -1) { // pipe 생성
                        printf("fail to call pipe()\n");
                        exit(4);
                    }
                    if((pid2 = fork()) == 0) { // 자식 - output redirection 처리
                        close(STD_OUT);
                        dup(fd_pipe[1]);
                        close(fd_pipe[1]);
                        close(fd_pipe[0]);
                        redirection_command[0] = command[i-1];
                        redirection_command[1] = NULL;
                        execvp(redirection_command[0], redirection_command);
                        printf("execvp error!\n");
                        exit(1);
                    } else if(pid2 > 0) { // 부모 - input redirection 처리
                        close(STD_IN);
                        dup(fd_pipe[0]);
                        close(fd_pipe[0]);
                        close(fd_pipe[1]);
                        redirection_command[0] = command[i+1];
                        redirection_command[1] = NULL;
                        execvp(redirection_command[0], redirection_command);
                        printf("execvp error!\n");
                        exit(1);
                    } else {
                        printf("fork error!\n");
                    }
                }
            }
            // 일반 명령어 처리
            execvp(command[0], command);
            printf("execvp error!\n");
            exit(1);
        } else if(pid1 > 0) { // 부모
            child = wait(&status); // 자식 프로세스를 기다림
        } else {
            printf("error occured");
        }
    }

    return 0;
}