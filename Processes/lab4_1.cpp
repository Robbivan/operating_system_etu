#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
//#include <sys/vfs.h>
#include <fcntl.h>
int main(int argc, char *argv[], char *envp[]) {
    std::cout<<"the program 2 has started working\n";
    std::cout<<"Output to the screen from iteration of list to NULL\n";
    std::cout<<"current: "<<getpid()<<std::endl;
    std::cout<<"parent: "<<getppid()<<std::endl;
    for (int i=0;i<3;i++){
        std::cout<<argv[i]<<std::endl;
        sleep(1);
    }
    std::cout<<envp[0]<<std::endl;
    std::cout<<"the program 2 has finished\n";
    exit(0);
}


