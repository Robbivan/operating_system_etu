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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <netinet/in.h>

int listen_socket, server_socket, flag_wait=0, flag_pro_req=0, flag_rec_request=0;
pthread_t thread_rec_req,thread_pro_req, thread_wait_req;
pthread_mutex_t mutex;//use mutex for conflict part of vector
std::vector <std::string> msglist;
sockaddr_in listen_socket_addr;


void sig_handler (int sign){//for cor work after end of prog
    printf ("\nget SIGPIPE\n");
}


void* rec_request (void* arg){
    printf("thread rec_request has started working\n");
    char rec_buf[128];
    int count=0;
    while (flag_rec_request == 0){
        int rec_count = recv(server_socket, rec_buf, sizeof(rec_buf), 0);
        if (rec_count == -1){
            perror ("recv");
            sleep (1);
        }
        else if (rec_count == 0) {
            sleep (1);
        }
        else {
            pthread_mutex_lock(&mutex);
            msglist.push_back(std::string(rec_buf));
            pthread_mutex_unlock(&mutex);
            fflush (stdout);
            printf ("\nNumb rec %d", count);
            count++;

        }
    }
    pthread_exit (NULL);
}


void* pro_request (void* arg){
    printf("thread pro_request has started working\n");
    int count=0;
    char path[]="/home";
    struct statfs buf;
    int fileStat;
    long bufGoing;
    while (flag_pro_req == 0){
        pthread_mutex_lock(&mutex);
        if (!msglist.empty()){
            std::string s = msglist.back();
            msglist.pop_back();
            pthread_mutex_unlock(&mutex);

            char buffer [45];
            fileStat=statfs(path,&buf);
            if (fileStat==-1){
                perror("statfs");
            }
            bufGoing=buf.f_namelen;
            const std::string str=std::to_string(bufGoing);
            strcpy(buffer, str.c_str());
            int len=(int)strlen(str.c_str());

            int send_count = send(server_socket, buffer, len, 0);
            if (send_count == -1){
                perror ("send");
            }
            else {
                fflush (stdout);
                printf("\nMsg %d send!\n", count);
                count++;
            }
        } else {
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    pthread_exit (NULL);
}


void* wait (void* arg){
    printf("thread wait has started working\n");
    sockaddr_in server_socket_addr;
    socklen_t addr_len = (socklen_t)sizeof(server_socket_addr);

    while (flag_wait==0){
        server_socket = accept(listen_socket, (struct sockaddr*)&server_socket_addr, &addr_len);
        if (server_socket==-1) {
            perror("accept");
            sleep(1);
        }
        else{
            fflush (stdout);
            printf("\nConnection is est\n");
            printf("client port number %d\n",(int) ntohs(server_socket_addr.sin_port));
            pthread_create(&thread_rec_req, NULL, rec_request, NULL);
            pthread_create(&thread_pro_req, NULL, pro_request, NULL);
            pthread_exit (NULL);
        }
    }
    pthread_exit (NULL);
}


int main (){
    signal(SIGPIPE, sig_handler);
    pthread_mutex_init (&mutex, NULL);
    printf("the program SERVER has started working\n");
    listen_socket = socket (AF_INET, SOCK_STREAM, 0);
    if (listen_socket==-1){
        perror("socket");
    }
    int res_fcntl=fcntl(listen_socket, F_SETFL, O_NONBLOCK);
    if (res_fcntl==-1){
        perror("fcntl");
    }

    listen_socket_addr.sin_family = AF_INET;
    listen_socket_addr.sin_port = htons(2000);
    listen_socket_addr.sin_addr.s_addr = htonl (INADDR_ANY);

    bind (listen_socket, (struct sockaddr*)&listen_socket_addr, sizeof(listen_socket_addr));
    int optval =1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));

    int res_listen=listen (listen_socket, SOMAXCONN);
    if (res_listen==-1){
        perror("listen");
    }
    int ret_val= pthread_create(&thread_wait_req, NULL, wait, NULL);
    if (ret_val == 0){
        printf("pthread_create req func OK\n");
    }
    else{
        printf("pthread_create error: %s\n",strerror(ret_val));
        return 0;
    }
    printf("the program is waiting for the key to be pressed\n");
    getchar();
    printf("the key is pressed\n");
    flag_wait=1;
    flag_pro_req=1;
    flag_rec_request=1;

    int ret_join_wait=pthread_join(thread_wait_req, NULL);
    if (ret_join_wait == -1){
        printf("join error: %s\n",strerror(ret_join_wait));
    }
    int ret_join_pro=pthread_join(thread_pro_req, NULL);
    if (ret_join_pro == -1){
        printf("join error: %s\n",strerror(ret_join_pro));
    }
    int ret_join_rec=pthread_join(thread_rec_req, NULL);
    if (ret_join_rec == -1){
        printf("join error: %s\n",strerror(ret_join_rec));
    }

    shutdown (server_socket, 2);
    close (listen_socket);
    close (server_socket);

    return 0;
}


