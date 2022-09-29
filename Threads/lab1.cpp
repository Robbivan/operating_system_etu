#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>

typedef struct {
    int flag;
    char sym;
}targs;
static void* proc1(void*arg1) {
    printf("thread 1 has started working\n");
    targs *args1 = (targs*) arg1;
    while (args1->flag == 0) {
        putchar(args1->sym);
        fflush(stdout);
        printf(" ");
        sleep(1);
    }
    printf("thread 1 has finished working\n");
    pthread_exit((void*)2);
};
static void *proc2(void*arg2) {
    printf("thread 2 has started working\n");
    targs *args2 = (targs*) arg2;
    while (args2->flag == 0) {
        putchar(args2->sym);
        fflush(stdout);
        printf(" ");
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
    pthread_attr_t tattr1;
    pthread_attr_t tattr2;
    int ret_attr1=pthread_attr_init(&tattr1);
    if (ret_attr1==0){
        printf("ret_attr1 func OK\n");
    }
    else{
        printf("Unit1 error: %s\n",strerror(ret_attr1));
    }
    int ret_attr2=pthread_attr_init(&tattr2);
    if (ret_attr2==0){
        printf("ret_attr2 func OK\n");
    }
    else{
        printf("Unit2 error: %s\n",strerror(ret_attr2));
    }

    //pthread_attr_setdetachstate(&tattr1,PTHREAD_CREATE_DETACHED);//break join
    size_t a=1000000000000000000;//too big stack (crash it)
    int ret_check_stack=pthread_attr_setstacksize(&tattr1, a);
    if (ret_check_stack==0){
        printf("retcheck func OK\n");
    }
    else{
        printf("Stack error: %s\n",strerror(ret_check_stack));
    }
    printf("the program is waiting for the key to be pressed\n");
    int ret_val1=pthread_create(&thread1, &tattr1, proc1, &arg1);
    int ret_val2=pthread_create(&thread2, &tattr2, proc2,&arg2);
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
    printf("the program has finished its work\n");
    return 0;
}


