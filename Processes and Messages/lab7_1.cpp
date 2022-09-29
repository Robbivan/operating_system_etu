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
static void* proc(void*arg1) {
    printf("thread 1 has started working\n");

    char path[]="/home";
    struct statfs buf;
    int fileStat;
    long bufGoing;
    while(flag==0){
        fileStat=statfs(path,&buf);
        if (fileStat==-1){
            perror("statfs");
        }
        bufGoing=buf.f_namelen;
        printf ("%ld", bufGoing);
        fflush(stdout);
        const std::string str=std::to_string(bufGoing);
        int res_mq_send=mq_send(mq,str.c_str(),strlen(str.c_str())+1,0);
        if (res_mq_send==-1){
            perror("mq_send");
        }

        sleep(1);
    }
    printf("thread 1 has finished working\n");
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
    printf("the program 1 has started working\n");
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize = 6;
    attr.mq_curmsgs = 0;

    mq=mq_open("/myqueue",O_CREAT | O_WRONLY| O_NONBLOCK, 0644, &attr);
    //mq=mq_open("/myqueue",O_CREAT | O_WRONLY, 0644, &attr);//with block
    
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

