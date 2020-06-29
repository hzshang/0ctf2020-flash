/*
 * wrapper.c
 * Copyright (C) 2020 hzshang <hzshang15@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

static int pid;
void stop_child(){
    kill(pid,9);
    waitpid(pid,0,0);
}
void intr_handler(int sig){
    stop_child();
    exit(0);
}
int main(int argc,char* argv[],char* envp[]){
    if(argc <= 1)
        return 1;
//    setvbuf(stdin,0,2,0);
//    setvbuf(stdout,0,2,0);
//    setvbuf(stderr,0,2,0);
    int fd[2];
    int status;
    char buffer[0x1000];
    if(pipe(fd) < 0){
        perror("pipe");
        return 1;
    }
    pid = fork();
    if(pid == 0){
        close(fd[1]);
        close(0);
        if(dup2(fd[0],0)<0){
            perror("dup2");
            exit(1);
        }
        execvp(argv[1],&argv[1]);
        perror("execvp");
        return 1;
    }else{
        signal(SIGINT,intr_handler);
        signal(SIGKILL,intr_handler);
        close(fd[0]);
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);

        while(1){
            int n = select(1,&fds,0,0,0);
            if(n <= 0){
                perror("select");
                break;
            }
            int n_read = read(0,buffer,sizeof(buffer));
            if(n_read <= 0){
                perror("read");
                break;
            }
            int n_write = write(fd[1],buffer,n_read);
            if(n_write <= 0){
                perror("write");
                break;
            }
        }
        stop_child();
    }
    return 0;
}


