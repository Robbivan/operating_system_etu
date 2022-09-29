#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define stack_size (1024*1024)

static int child_func (void* arg){
    printf("Id child: %d\n",getpid());
    printf("Id par: %d\n",getppid());
    return 0;
}


int main (){
    int status;
    char *stack=(char*)malloc(stack_size);
    char *stack_top = stack + stack_size;
    printf("\nId current: %d\n",getpid());//current id
    
    printf ("\nUse different namespaces\n");
    int child_pid = clone(child_func, stack_top, CLONE_NEWPID | SIGCHLD, NULL);
    if (child_pid==-1) {
        perror ("clone");
    }

    waitpid(child_pid, &status, 0);
 


    printf ("\nUse general namespace\n");
    int child_pid2 = clone(child_func, stack_top, SIGCHLD, NULL);
    if (child_pid2 ==-1) {
        perror ("clone");
    }
    waitpid(child_pid2, &status, 0);

   
    return 0;
}


