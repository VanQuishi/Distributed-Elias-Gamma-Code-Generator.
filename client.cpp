// Please note this is a C program
// It compiles without warnings with gcc

/*The socket templates and fireman function are from pofessor Rincon source code.
The toBinary() function is from geeksforgeeks*/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <pthread.h>

using namespace std;

#define NOPER 256
int portno, n;

struct sockaddr_in serv_addr;
struct hostent *server;

int num_code = 5;

struct code
{
    int sockfd;
    int val;
    string binary;
    string eli;
};

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void *sendToServer(void *value_void_ptr)
{
    struct code *value_ptr = (struct code *)value_void_ptr;
    int value = value_ptr->val;
   
    int sockfd;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    n = write(sockfd, &value, sizeof(int));

    if (n < 0)
        error("ERROR writing to socket");

    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);

    bool flag = true;
    for (int i = 0; i < strlen(buffer); i++)
    {
        if (buffer[i] == ' ')
        {
            flag = false;
        }

        if (flag)
        {   
            value_ptr->binary += buffer[i];
        }
        else
        {
            value_ptr->eli += buffer[i];
        }
    }

    if (n < 0)
        error("ERROR reading from socket");
    
    
    close(sockfd);

    return NULL;
}



int main(int argc, char *argv[])
{
    int sockfd;

    char buffer[256];
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    //==========

    cin >> num_code;

    int *values_arr = new int[num_code];

    for (int i = 0; i < num_code; i++)
    {
        cin >> values_arr[i];
    }   

    
    static struct code operations[NOPER];
    pthread_t tid[num_code];

    for (int i = 0; i < num_code; i++)
    {
        operations[i].sockfd = sockfd;
        operations[i].val = values_arr[i];
       
        if(pthread_create(&tid[i], NULL, sendToServer, &operations[i]))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for (int i = 0; i < num_code; i++)
    {
        pthread_join(tid[i], NULL);
    }


    for (int i = 0; i < num_code; i++)
    {
        cout << "Value: " << operations[i].val << ", Binary Code: " << operations[i].binary
        << ", Elias-Gamma code:" << operations[i].eli << endl;
    }

    delete[] values_arr;

    close(sockfd);
    return 0;
}
