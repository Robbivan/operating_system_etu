#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <sys/vfs.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int flag;
FILE *file;
sem_t*sem;
static void* proc(void*arg1) {
    printf("thread has started working\n");
    while (flag == 0) {
        int res_sem_wait= sem_wait(sem);
        if (res_sem_wait == 0){
            printf("sem_wait func OK\n");
        }
        else{
            perror("sem_wait");
        }
        for (int i=0;i<10;i++){
            putchar('2');
            fflush(stdout);
            fprintf (file, "%d", 2);
            fflush (file);
            sleep(1);
        }
        int res_sem_post=sem_post(sem);
        if (res_sem_post==0){
            printf("check_post func OK\n");
        }
        else{
            perror("sem_post");
        }
        sleep(1);
    }
    printf("thread has finished working\n");
    pthread_exit((void*)2);
};
int main() {
    printf("the program 2 has started working\n");
    flag=0;
    pthread_t thread;
    sem= sem_open("/sema",O_CREAT,0644,1);
    file = fopen("unit.txt", "a+");
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
    int ret_close_file=fclose(file);
    if (ret_close_file == 0){
        printf("fclose func OK\n");
    }
    else{
        printf("fclose error: %s\n",strerror(ret_close_file));
    }
    int ret_close_sem=sem_close(sem);
    if (ret_close_sem==0){
        printf("sem_close func OK\n");
    }
    else{
        perror("sem_close");
    }
    int res_unlink_sem=sem_unlink("/sema");
    if (res_unlink_sem==0){
        printf("sem_unlink func OK\n");
    }
    else{
        perror("sem_unlink");
    }
    printf("the program 2 has finished its work\n");
    return 0;
}

