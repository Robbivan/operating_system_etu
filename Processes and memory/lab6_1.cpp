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

int flag=0;
sem_t*sem_read;
sem_t *sem_write;
int mem_id; 
struct shmid_ds *ptr;
static void* proc(void*arg1) {
    printf("thread 1 has started working\n");
    char path[]="/home";
    struct statfs buf;
    int fileStat;
    long bufGoing;
    while (flag == 0) {
        fileStat=statfs(path,&buf);
        if (fileStat==-1){
            perror("statfs");
        }
        bufGoing=buf.f_namelen;
        printf ("%ld ", bufGoing);
        fflush(stdout);

        memcpy(ptr,&bufGoing,sizeof(long));
        sem_post(sem_read);
        sem_wait(sem_write);
        sleep(1);
    }
    printf("thread 1 has finished working\n");
    pthread_exit((void*)2);
};
void sig_handler(int signo)//for Ctrt+C
{
    int ret_close_sem_read=sem_close(sem_read);
    if (ret_close_sem_read == 0){
        printf("sem_close_read func OK\n");
    }
    else{
        perror("sem_close");
    }
    int ret_close_sem_write=sem_close(sem_write);
    if (ret_close_sem_write == 0){
        printf("sem_close_write func OK\n");
    }
    else{
        perror("sem_close");
    }

    int res_unlink_sem_read=sem_unlink("/sem_for_read");
    if (res_unlink_sem_read == 0){
        printf("sem_unlink_read func OK\n");
    }
    else{
        perror("sem_unlink");
    }
    int res_unlink_sem_write=sem_unlink("/sem_for_write");
    if (res_unlink_sem_write == 0){
        printf("sem_unlink_write func OK\n");
    }
    else{
        perror("sem_unlink");
    }


    int res_shmdt=shmdt(ptr);
    if (res_shmdt==-1){
        perror("shmdt");
    }
    int res_shmctl=shmctl(mem_id,IPC_RMID,ptr);
    if (res_shmctl==-1){
        perror("shmctl");
    }

    exit(0);
}

int main() {
    printf("the program 1 has started working\n");

    signal(SIGINT,sig_handler);
    pthread_t thread;
    key_t my_key = ftok(".", 27);
    //use "." because we don't know the full path to the file for each computer
    // So, we are looking in this directory

    mem_id = shmget (my_key, sizeof (long), 0644|IPC_CREAT);
    if (mem_id==-1){
        perror("shmget");
    }

    ptr=(shmid_ds*)shmat(mem_id,NULL,0);

    sem_read= sem_open("/sem_for_read", O_CREAT, 0644, 0);
    sem_write= sem_open("/sem_for_write", O_CREAT, 0644, 0);

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

    int ret_close_sem_read=sem_close(sem_read);
    if (ret_close_sem_read == 0){
        printf("sem_close_read func OK\n");
    }
    else{
        perror("sem_close");
    }
    int ret_close_sem_write=sem_close(sem_write);
    if (ret_close_sem_write == 0){
        printf("sem_close_write func OK\n");
    }
    else{
        perror("sem_close");
    }

    int res_unlink_sem_read=sem_unlink("/sem_for_read");
    if (res_unlink_sem_read == 0){
        printf("sem_unlink_read func OK\n");
    }
    else{
        perror("sem_unlink");
    }
    int res_unlink_sem_write=sem_unlink("/sem_for_write");
    if (res_unlink_sem_write == 0){
        printf("sem_unlink_write func OK\n");
    }
    else{
        perror("sem_unlink");
    }


    int res_shmdt=shmdt(ptr);
    if (res_shmdt==-1){
        perror("shmdt");
    }
    int res_shmctl=shmctl(mem_id,IPC_RMID,ptr);
    if (res_shmctl==-1){
        perror("shmctl");
    }


    printf("the program 1 has finished its work\n");
    return 0;
}

