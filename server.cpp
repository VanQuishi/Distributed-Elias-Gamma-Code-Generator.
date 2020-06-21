// A simple server in the internet domain using TCP
// The port nu1mber is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc

/*How to compile client.c and server.c in terminal:
    gcc server.c -o server
    gcc client.c -o client -pthread
    ./server <port_no>

    Then open another tab in theb terminal:
    ./client localhost <port_no>
*/

/*The socket templates and fireman function are from pofessor Rincon source code.
The toBinary() function is from geeksforgeeks*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/wait.h>
#include <string>
#include <cmath>

using namespace std;

static int num_code;

string toBinary(int n)
{
    string binary = "";
    int binaryCode[32];

    if (n == 0) 
        binary = "0";

    else
    {
        int i = 0;

        while (n > 0) { 
  
            // storing remainder in binary array 
            binaryCode[i] = n % 2; 
            n = n / 2; 
            i++; 
        }

        for (int j = i-1; j >= 0; j--)
        {
            binary += to_string(binaryCode[j]);
        } 
    }

    return binary;
}

string toEli(int n, string binary)
{
    int bits = log2(n);

    string result;

    for (int i = 0; i < bits; i++)
    {
        result += "0";
    }

    return result + binary;
}

void fireman(int)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
        //cout << "A child process ended" << endl;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 1000);
    clilen = sizeof(cli_addr);

    //===============
    signal(SIGCHLD, fireman);
    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);

        if (fork() == 0)
        {
            if (newsockfd < 0)
                error("ERROR on accept");

            n = read(newsockfd, &num_code, sizeof(int));

            string binary = toBinary(num_code);
            string eli = " " + toEli(num_code, binary);
            
            bzero(buffer,256);
            
            strcpy(buffer,binary.c_str());
            strcat(buffer,eli.c_str());

            if (n < 0)
                error("ERROR reading from socket");

            n = write(newsockfd, buffer, strlen(buffer));
            
            if (n < 0)
                error("ERROR writing to socket");
            close(newsockfd);
            _exit(0);
        }
        close(newsockfd);
    }

    close(sockfd);
    
    return 0;
}
