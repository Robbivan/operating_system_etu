#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <sys/vfs.h>
#include <string.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <signal.h>

#include <mqueue.h>

int flag=0;
mqd_t mq;
struct mq_attr attr;


static void* proc(void*arg1) {
    printf("thread 2 has started working\n");

    char buf [7];

    while(flag==0){
        memset (buf, 0, sizeof buf);
        ssize_t resmq_receive= mq_receive(mq, buf, 7, 0);
       for (size_t i=0;i<attr.mq_maxmsg-1;i++){
            printf ("%c", buf[i]);
            fflush(stdout);
        } 
    }
    printf("thread 2 has finished working\n");
    pthread_exit((void*)2);
}

void sig_handler(int signo){//for Ctrl+c (block mode)
    int res_mq_close=mq_close(mq);
    if (res_mq_close==-1){
        perror("mq_close");
    }

    exit(0);
}

int main() {
    signal(SIGINT,sig_handler);
    pthread_t thread;
    
    
    attr.mq_flags = 0;
    attr.mq_maxmsg = 6;
    attr.mq_msgsize = 6;
    attr.mq_curmsgs = 0;
    
    printf("the program 2 has started working\n");
    mq=mq_open("/myqueue",O_CREAT | O_RDONLY| O_NONBLOCK, 0644, &attr);
    //mq=mq_open("/myqueue",O_CREAT | O_RDONLY, 0644, &attr);//with block
    if (mq==-1){
        perror("mq_open");
    }
    
    
    printf("the program is waiting for the key to be pressed\n");

    int ret_val=pthread_create(&thread, NULL, proc, NULL);
    if (ret_val == 0){
        printf("pthread_create func OK\n");
    }
    else{
        printf("pthread_create error: %s\n",strerror(ret_val));
        return 0;
    }

    getchar();
    printf("the key is pressed\n");
    flag=1;
    int *exitcode;
    int ret_join=pthread_join(thread, (void**)&exitcode);
    if (ret_join == 0){
        printf("join func OK\n");
    }
    else{
        printf("join error: %s\n",strerror(ret_join));
        return 0;
    }
    printf("exitcode = %p\n",exitcode);

    int res_mq_close=mq_close(mq);
    if (res_mq_close==-1){
        perror("mq_close");
    }

    int res_mq_unlink=mq_unlink("/myqueue");
    if (res_mq_unlink==-1){
        perror("mq_unlink");
    }

    return 0;
}


