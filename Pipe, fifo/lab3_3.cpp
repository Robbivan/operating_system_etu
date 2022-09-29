#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring>
#include <sys/vfs.h>
#include <fcntl.h>
typedef struct {
    int flag;
    char sym;
}targs;
int pipefd[2];
void print_long(long value) {
    if(value != 0) {
        print_long(value/10);
        putchar((value%10) + '0');
        fflush(stdout);
        printf(" ");
    }
}
static void* proc1(void*arg1) {
    
    long bufGoing;
    size_t count=sizeof(bufGoing);
    char path[]="/home";
    struct statfs buf;
    targs *args1 = (targs*) arg1;
    while (args1->flag == 0) {
        int fileStat=statfs(path,&buf);
        bufGoing=buf.f_namelen;
        ssize_t resWrite = write(pipefd[1], &bufGoing, count);

        if (resWrite==-1){
            perror("write");
        }
        else if (resWrite==0){
            printf("not a single byte was written");
        }
        else{
            printf("write func OK\n");
        }
        sleep(1);
    }
    
    pthread_exit((void*)2);
};
static void *proc2(void*arg2) {
    
    targs *args2 = (targs*) arg2;
    long bufGoing;
    size_t count=sizeof(bufGoing);
    while (args2->flag == 0) {

       bufGoing=0;
       ssize_t resRead= read(pipefd[0], &bufGoing, count);

       if (resRead==-1){
           perror("read");
           sleep(1);
       } else if (resRead==0){
            printf("end of file, the recording channel is closed");
            sleep(1);
       }else
       {
           
           print_long(bufGoing);
           printf(" ");
       }
    }
   
    pthread_exit((void*)2);
};

int main() {
    
    targs arg1;
    targs arg2;
    arg1.flag=0;
    arg1.sym='1';
    arg2.flag=0;
    arg2.sym='2';
    pthread_t thread1;
    pthread_t thread2;
    int rv=pipe(pipefd);
    if (rv==0){
        printf("pipe withFCNTL func OK\n");
    }
    else{
        perror("pipe");
    }
    int fc1=fcntl(pipefd[0],F_SETFL,O_NONBLOCK);
    if (fc1==-1){
        perror("fcntl");
    }
    int fc2=fcntl(pipefd[1],F_SETFL,O_NONBLOCK);
    if (fc2==-1){
        perror("fcntl");
    }
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
    close(pipefd[0]);
    close(pipefd[1]);
    printf("exitcode1 = %p\n",exitcode1);
    printf("exitcode2 = %p\n",exitcode2);
    
    return 0;
}



