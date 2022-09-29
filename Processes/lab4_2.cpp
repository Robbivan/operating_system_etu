#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
//#include <sys/vfs.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char *argv[], char *envp[]) {
    printf("the program (main) has started working\n");
    pid_t pid =fork();
    if(pid==0){
        printf("the сhild part has started working\n");
        std::cout<<"child (parent part): "<<getpid()<<std::endl;
        char envar[]="HERE -> transmitted using envp";
        char*envp[] = {
        	envar,
                NULL
        };
        int rv=execle("lab4_1","h","i","!",NULL,envp);
        if (rv==-1)
        {
            perror("exec");
        }
        printf("the сhild part has finished\n");
    }
    else if(pid==-1){
        perror("fork");
    }
    else{
        printf("the parent part has started working\n");
        std::cout<<"current (parent part): "<<getpid()<<std::endl;
        std::cout<<"parent (parent part): "<<getppid()<<std::endl;
        int status;
        int w;
        struct timespec tw={0,500};
        struct timespec tr;
        while ((w=waitpid(pid,&status,WNOHANG))==0){
            nanosleep(&tw,&tr);
        }
        if (w==-1){
            perror("waitpid");
        }
        else{
            std::cout<<"Exit status: "<<status<<std::endl;
        }
        printf("the parent part has finished\n");
    }
    return 0;
}

