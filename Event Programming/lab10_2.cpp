#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
//#include <sys/vfs.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ev.h>
#include <stdlib.h>
ev_child cw;

static void child_cb (EV_P_ ev_child *w, int revents)
{

    ev_child_stop (EV_A_ w);
    printf ("\nprocess STATUS: %x\n\n", w->rstatus);
    ev_break(EV_A_  EVBREAK_ALL);
}


int main(int argc, char *argv[], char *envp[]) {
    printf("the program (main) has started working\n");

    struct ev_loop *loop = EV_DEFAULT;


    pid_t pid =fork();
    if(pid==0){
        printf("the сhild part has started working\n");
        std::cout<<"child (parent part): "<<getpid()<<std::endl;
        char envar[]="HERE -> transmitted using envp";
        char*envp[] = {
                envar,
                NULL
        };
        int rv=execle("lab10_1","h","i","!",NULL,envp);
        if (rv==-1)
        {
            perror("exec");
        }
        printf("the сhild part has finished\n");
    }
    else if(pid==-1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else{
        printf("the parent part has started working\n");

        std::cout<<"current (parent part): "<<getpid()<<std::endl;
        std::cout<<"parent (parent part): "<<getppid()<<std::endl;
        int status;
        int w;
        struct timespec tw={0,500};
        struct timespec tr;
        
        ev_child_init (&cw, child_cb, pid, 0);
        ev_child_start (EV_DEFAULT_ &cw);
        ev_run (loop, 0);
        while ((w=waitpid(pid,&status,WNOHANG))==0){
            nanosleep(&tw,&tr);
        }
        printf("the parent part has finished\n");
    }
   
    return 0;
}



