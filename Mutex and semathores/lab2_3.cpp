#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <semaphore.h>
typedef struct {
    int flag;
    char sym;
}targs;
sem_t sem;
static void* proc1(void*arg1) {
    printf("thread 1 has started working\n");
    targs *args1 = (targs*) arg1;
    timespec tp;
    while (args1->flag == 0) {
        clock_gettime(CLOCK_REALTIME,&tp);
        tp.tv_sec+=1;
        int res_sem_wait=sem_timedwait(&sem,&tp);
        if (res_sem_wait == 0){
            printf("the resource 1 is free\n");
            for (int i=0;i<3;i++){
                putchar(args1->sym);
                fflush(stdout);
                printf(" ");
                sleep(1);
            }
            int res_sem_post=sem_post(&sem);
            if (res_sem_post==0){
                printf("check_post2 func OK\n");
            }
            else{
                perror("sem_post");
            }
        }
        else{
            printf("NO FREE resource 1\n");
            sleep(1);
        }

        sleep(1);
    }
    printf("thread 1 has finished working\n");
    pthread_exit((void*)2);
};
static void *proc2(void*arg2) {
    printf("thread 2 has started working\n");
    targs *args2 = (targs*) arg2;
    timespec tp;
    while (args2->flag == 0) {
        clock_gettime(CLOCK_REALTIME,&tp);
        tp.tv_sec+=1;
        int res_sem_wait=sem_timedwait(&sem,&tp);
        if (res_sem_wait == 0){
            printf("the resource 2 is free\n");
            for (int i=0;i<4;i++){
                putchar(args2->sym);
                fflush(stdout);
                printf(" ");
                sleep(1);
            }
            int res_sem_post=sem_post(&sem);
            if (res_sem_post==0){
                printf("check_post1 func OK\n");
            }
            else{
                perror("sem_post");
            }
        }
        else{
            printf("NO FREE resource 2\n");
            sleep(1);
        }
        sleep(1);
    }
    printf("thread 2 has finished working\n");
    pthread_exit((void*)2);
};

int main() {
    printf("the program has started working\n");
    targs arg1;
    targs arg2;
    arg1.flag=0;
    arg1.sym='1';
    arg2.flag=0;
    arg2.sym='2';
    pthread_t thread1;
    pthread_t thread2;


    sem_init(&sem,0,1);



    printf("the program is waiting for the key to be pressed\n");
    int ret_val1=pthread_create(&thread1, NULL, proc1, &arg1);
    int ret_val2=pthread_create(&thread2, NULL, proc2,&arg2);
    if (ret_val1 == 0){
        printf("function1 func OK\n");
    }
    else{
        printf("func1 error: %s\n",strerror(ret_val1));
        return 0;
    }
    if (ret_val2==0){
        printf("function2 func OK\n");
    }
    else{
        printf("func2 error: %s\n",strerror(ret_val2));
        return 0;
    }
    getchar();
    printf("the key is pressed\n");
    arg1.flag=1;
    arg2.flag=1;
    int *exitcode1;
    int *exitcode2;
    int ret_join1=pthread_join(thread1, (void**)&exitcode1);
    int ret_join2=pthread_join(thread2, (void**)&exitcode2);
    if (ret_join1 == 0){
        printf("join1 OK\n");
    }
    else{
        printf("join1 error: %s\n",strerror(ret_join1));
        return 0;
    }
    if (ret_join2 == 0){
        printf("join2 OK\n");
    }
    else{
        printf("join2 error: %s\n",strerror(ret_join2));
        return 0;
    }
    printf("exitcode1 = %p\n",exitcode1);
    printf("exitcode2 = %p\n",exitcode2);
    int res_sem_destroy=sem_destroy(&sem);
    if (res_sem_destroy==0)
    {
        printf("destroy_sem func OK\n");
    }
    else{
        perror("sem_destroy");
    }
    printf("the program has finished its work\n");
    return 0;
}


