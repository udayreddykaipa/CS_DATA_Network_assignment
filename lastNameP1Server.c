//server code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

struct userdict
{
    char unameArr[20][100], passArr[20][100];
} auth;

int loadAuthData()
{
    FILE *authfile;
    char s[30], n[30];
    authfile = fopen("userList.txt", "r");
    for (int i = 0; fscanf(authfile, "%s %s", s, n) != EOF; i++)
    {
        strcpy(auth.unameArr[i], s);
        strcpy(auth.passArr[i], n);
    }

    fclose(authfile);
    // for (int i = 0; i < 6; i++) //testing
    // {
    //     /* code */
    //     puts(auth.unameArr[i]);
    //     puts(auth.passArr[i]);
    // }

    return 1;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    //connect to sender
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc != 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    puts("Welcome to SERVER socket C program");
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

    //load userList to memory
    loadAuthData();

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // verify uname and pass
    int flag = 1;
    while (flag == 1)
    {

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);

        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the User name: %s\n", buffer);

        for (int i = 0; i < 6; i++)
        {
            if (strcmp(auth.unameArr[i], buffer) == 0)
            {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            puts("not found"); //send ack 0
            n = write(newsockfd, "0", 1);
            if (n < 0)
                error("ERROR writing to socket");
            
        }
    }
    printf("uname found");
    n = write(newsockfd,"1", 1);
    if (n < 0)
        error("ERROR writing to socket");
    
    // password
    flag = 1;
    while (flag == 1)
    {

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);

        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the Password: %s\n", buffer);

        for (int i = 0; i < 6; i++)
        {
            if (strcmp(auth.passArr[i], buffer) == 0)
            {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            puts("not found"); //send ack 0
            n = write(newsockfd, "0", 1);
            if (n < 0)
                error("ERROR writing to socket");
            
        }
    }
    printf("uname found");
    n = write(newsockfd,"1", 1);
    if (n < 0)
        error("ERROR writing to socket");
}
