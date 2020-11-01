//Receiver code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int hammingDist(char str1[], char str2[])
{
    int i = 0, count = 0;
    while (str1[i] != '\0')
    {
        if (str1[i] != str2[i])
            count++;
        i++;
    }
    return count;
}

int checkSum(char str1[])
{
    int checksum = 0; // could be an int if preferred
    int SizeOfArray = strlen(str1);
    int re;

    for (int x = 0; x < SizeOfArray; x++)
    {
        checksum += str1[x];
    }
    //Perform bitwise inversion
    checksum = ~checksum;
    //Increment
    checksum++;
    // printf("Sum of the bytes for %s is: %d\n", str1, checksum);
    re=checksum;
    return re;
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
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // after connection, receivnig started
    sleep(0.1);
    n = write(newsockfd, "sent from Connected receiver server", 36);
    if (n < 0)
    {
        error("ERROR read to socket");
    }
    puts("A Client connected and ACK sent\n");

    int flag = 1, count = 0, distance = 0, Csum = 0;
    char c[10], di[10];
    char buffer2[255] = "";
    char Response[255];
    char d[3];
    while (flag == 1)
    {
        count += 1;

        // printf("%d", count);
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
        {
            error("ERROR read to socket");
        }
        puts(buffer);
        if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0 || strcmp(buffer, "x") == 0 || strcmp(buffer, "X") == 0)
        {
            // terminate
            close(newsockfd);
            error("Terminate Code Received");
            break;
        }
        Csum = checkSum(buffer);

        sprintf(c, "%d", Csum);
        strcpy(Response, "");
        if (count % 2 == 0)
        {
            strcpy(Response, "The Check Sum and Hammming Distance are ");
            strcat(Response, c);
            strcat(Response, " and ");
            distance = hammingDist(buffer, buffer2);
            sprintf(di, "%d", distance);
            strcat(Response, di);
        }
        else
        {
            strcpy(Response, "The Check Sum is : ");
            strcat(Response, c);
        }

        // printf("%s %s are buffer and buffer2", buffer, buffer2);

        n = write(newsockfd, Response, strlen(Response));
        if (n < 0)
        {
            error("ERROR read to socket");
        }
        puts(Response);
        strcpy(buffer2, buffer);

        // sent data
    }

    return 0;
}
