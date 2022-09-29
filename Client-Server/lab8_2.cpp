#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <cstdio>
#include <cstring>
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

int client_socket, flag_conn=0,flag_pro_request=0, flag_send_request=0;;
sockaddr_in client_socket_addr;
pthread_t thread_pro_req,thread_send_req, thread_conn;

void sig_handler (int sign){//for cor work after end of prog
    printf ("\nSIGPIPE\n");
}

void* send_request(void*arg) {
    printf("thread send_request has started working\n");
    int count=0;
    while (flag_send_request == 0){
        char send_buf[128];
        int len = sprintf(send_buf, "request%d", count);
        int send_count = send (client_socket, send_buf, len, 0);
        if (send_count==-1){
            perror("send");
        }
        else{
            fflush (stdout);
            printf ("Send from cl req %d \n", count);
            count++;
        }
        sleep(1);
    }
    pthread_exit (NULL);
}
void* pro_request(void*arg) {
    printf("thread pro_request has started working\n");
    char rec_buf [128];
    int count=0;
    while(flag_pro_request==0){
        memset(rec_buf, 0, sizeof(rec_buf));
        int rec_count = recv (client_socket, rec_buf, sizeof(rec_buf), 0);
        if (rec_count == -1){
            perror ("recv");
            sleep(1);
        }
        else if (rec_count==0){
            sleep (1);
        }
        else {
            fflush (stdout);
            printf ("Res answer %d: \n%s\n", count, rec_buf);
            count++;
        }
    }
    pthread_exit (NULL);
}

void* connection(void*arg) {
    printf("thread connection has started working\n");

    while (flag_conn == 0){
        int res_conn = connect(client_socket, (struct sockaddr*) &client_socket_addr, sizeof (client_socket_addr));
        if (res_conn==-1){
            perror("connect");
            sleep(1);
        }
        else{
            fflush (stdout);
            printf("Connection is est\n");
            socklen_t clientsz = sizeof(client_socket_addr);
            getsockname(client_socket, (struct sockaddr *) &client_socket_addr, &clientsz);
            printf("client port number %u\n",ntohs(client_socket_addr.sin_port));

            pthread_create(&thread_send_req, NULL, send_request, NULL);
            pthread_create(&thread_pro_req, NULL, pro_request, NULL);
            pthread_exit (NULL);
        }
    }
    pthread_exit (NULL);
}


int main() {
    signal(SIGPIPE, sig_handler);

    printf("the program CLIENT has started working\n");

    client_socket = socket (AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1){
        perror("socket");
    }
    int res_fcntl=fcntl(client_socket, F_SETFL, O_NONBLOCK);
    if (res_fcntl==-1){
        perror("fcntl");
    }
    client_socket_addr.sin_family=AF_INET;
    client_socket_addr.sin_port= htons(2000);
    client_socket_addr.sin_addr.s_addr = inet_addr ("127.0.0.1");


    int ret_val= pthread_create(&thread_conn, NULL, connection, NULL);
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
    flag_conn=1;
    flag_pro_request=1;
    flag_send_request=1;

    int ret_join_wait=pthread_join(thread_conn, NULL);
    if (ret_join_wait == -1){
        printf("join error: %s\n",strerror(ret_join_wait));
    }
    int ret_join_pro=pthread_join(thread_pro_req, NULL);
    if (ret_join_pro == -1){
        printf("join error: %s\n",strerror(ret_join_pro));
    }
    int ret_join_rec=pthread_join(thread_send_req, NULL);
    if (ret_join_rec == -1){
        printf("join error: %s\n",strerror(ret_join_rec));
    }

    shutdown (client_socket, 2);
    close (client_socket);

    return 0;
}


