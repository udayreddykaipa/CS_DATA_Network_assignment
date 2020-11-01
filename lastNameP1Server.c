//server code file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> //inet_addr

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int CreateServerAndConnectToClient(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen; //socket and conn var
    struct sockaddr_in serv_addr, cli_addr;
    int socket_desc; //can del
    struct sockaddr_in server;
    char server_reply[255];

    //check arguents
    if (argc != 2)
        error("expected 1 Argument,Exited.\n");
    // create a socket and bind
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    printf("Welcome to SERVER socket C program, Server Started at port : %s\n", argv[1]);
    // bind
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    return newsockfd;
}

void checkAuthentication(int newsockfd)
{

    FILE *authfile;
    char s[30], p[30], k[30];       //used to store received info comparision
    char buffer[256], buffer1[256]; //rece buffer common
    int n;
    char server_reply[2000];

    int flag = 1;
    while (flag == 1)
    {
        puts("Waiting for Credentials");
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the User name : %s\n", buffer);
        n = write(newsockfd, "Received", 9);
        if (n < 0)
            error("ERROR writing to socket");

        bzero(buffer1, 256);
        n = read(newsockfd, buffer1, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the Pass: %s\n", buffer1);
        n = write(newsockfd, "Received", 9);
        if (n < 0)
            error("ERROR writing to socket");

        printf("%s %s ", buffer, buffer1);
        puts("Validating credentials\n");
        char a[8] = "failed";
        authfile = fopen("userList.txt", "r");
        for (int i = 0; fscanf(authfile, "%s %s", s, p) != EOF; i++)
        {
            if (strcmp(s, buffer) == 0 && strcmp(p, buffer1) == 0)
            {
                flag = 0;
                strcpy(a, "Success");
                break;
            }
        }
        fclose(authfile);
        sleep(1);
        n = write(newsockfd, a, strlen(a));
        if (n < 0)
            error("ERROR writing to socket");

        puts(a);
    }
}

//called after authentication
int newConn(char ip[], char port[], int senderSocket)
{
    //connect to rec
    puts("Starting a connection to Receiver server");
    int socket_desc, n;
    struct sockaddr_in server;
    char server_reply[2000];
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(atol(port));
    //Connect to remote server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("connect error");
        return 1;
    }

    puts("Connected to Receiver server\n");
    n = read(socket_desc, server_reply, 256);
    if (n < 0)
        error("ERROR writing to socket");
    puts("ser");
    puts(server_reply);
    sleep(0.1);
    n = write(senderSocket, server_reply, strlen(server_reply));
    if (n < 0)
        error("ERROR writing to socket");
    // receive and send data
    int flag = 1;
    while (flag == 1)
    {
        bzero(server_reply, 255);
        n = read(senderSocket, server_reply, 255);
        if (n < 0)
            error("ERROR reading from sender socket");

        puts(server_reply);
        if (strcmp(server_reply, "q") == 0 || strcmp(server_reply, "Q") == 0 || strcmp(server_reply, "x") == 0 || strcmp(server_reply, "X") == 0)
        {
            n = write(socket_desc, server_reply, strlen(server_reply));
            if (n < 0)
                error("ERROR writing to Exit signal to socket");
            error("Terminate signal, Exit");
        }
        n = write(socket_desc, server_reply, strlen(server_reply));
        if (n < 0)
            error("ERROR writing to socket");
        bzero(server_reply, 255);
        n = read(socket_desc, server_reply, 255);
        if (n < 0)
            error("ERROR read to socket");
        puts(server_reply);
        n = write(senderSocket, server_reply, strlen(server_reply));
        if (n < 0)
            error("ERROR writing to socket");
    }
}

void checkReceiverDetails(int newsockfd)
{
    FILE *recfile;
    char s[30], p[30], k[30];       //used to store received info comparision
    char buffer[256], buffer1[256]; //rece buffer common
    int n;
    int flag = 1;
    while (flag == 1)
    {
        puts("Waiting for Receiver server name and Port");
        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the Receiver name : %s\n", buffer);
        n = write(newsockfd, "Received", 9);
        if (n < 0)
            error("ERROR writing to socket");

        bzero(buffer1, 256);
        n = read(newsockfd, buffer1, 255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the Port: %s\n", buffer1);
        n = write(newsockfd, "Received", 9);
        if (n < 0)
            error("ERROR writing to socket");

        printf("%s %s ", buffer, buffer1);
        char a[8] = "failed";
        recfile = fopen("receiverList.txt", "r");
        for (int i = 0; fscanf(recfile, "%s %s %s", s, k, p) != EOF; i++)
        {
            // puts(k);
            if (strcmp(s, buffer) == 0 && strcmp(p, buffer1) == 0)
            {
                fclose(recfile);
                flag = 0;
                strcpy(a, "Success");
                break;
                sleep(1);
            }
        }

        sleep(1);
        n = write(newsockfd, a, strlen(a));
        if (n < 0)
            error("ERROR writing to socket");

        puts(a);
    }
    printf("%s %s is ip and Port of receiver", k, p);
    newConn(k, p, newsockfd);
    return;
}

int main(int argc, char *argv[])
{
    int newsockfd = CreateServerAndConnectToClient(argc, argv); //server connected to sender after return

    //vars
    FILE *authfile, *recfile;       //file pointers
    char s[30], p[30], k[30];       //used to store received info comparision
    char buffer[256], buffer1[256]; //rece buffer common
    int n;

    //authentication - tested
    checkAuthentication(newsockfd);

    //Receiver server Name,
    checkReceiverDetails(newsockfd);

    //  also connect function called by above function
}

// newConn(k, buffer1, newsockfd); //new connection to receiver

//connect to rec
// puts("Starting a connection to Receiver server");
// //Create socket
// socket_desc = socket(AF_INET, SOCK_STREAM, 0);
// if (socket_desc == -1)
// {
//     printf("Could not create socket");
// }
// server.sin_addr.s_addr = inet_addr(k);
// server.sin_family = AF_INET;
// server.sin_port = htons(atol(buffer1));
// //Connect to remote server
// if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
// {
//     puts("connect error");
//     return 1;
// }
// bzero(buffer1, 256);
// n = read(newsockfd, buffer1, 255);
// if (n < 0)
//     error("ERROR reading from socket");
// printf("Here is the Port: %s\n", buffer1);

// n = write(newsockfd, "Connected to Receiver Server", 29);
// if (n < 0)
//     error("ERROR writing to socket");
// puts("Connected to Receiver server");

// // receive and send data
// int flag = 1;
// while (flag == 1)
// {
//     bzero(buffer1, 255);
//     n = read(newsockfd, buffer1, 255);
//     if (n < 0)
//         error("ERROR reading from sender socket");

//     puts(buffer1);
//     if (strcmp(buffer1, "q") == 0 || strcmp(buffer1, "Q") == 0 || strcmp(buffer1, "x") == 0 || strcmp(buffer1, "X") == 0)
//     {
//         return 0;
//     }
//     n = write(socket_desc, buffer1, strlen(buffer1));
//     if (n < 0)
//         error("ERROR writing to socket");

//     bzero(buffer, 255);
//     n = read(socket_desc, buffer, 255);
//     if (n < 0)
//         error("ERROR read to socket");
//     puts(buffer);
//     n = write(newsockfd, buffer, strlen(buffer));
//     if (n < 0)
//         error("ERROR writing to socket");
// }

// after connection with receiver by server , server sends ack
// verify uname and pass
// int flag = 1;
// while (flag == 1)
// {
//     bzero(buffer, 256);
//     n = read(newsockfd, buffer, 255);
//     if (n < 0)
//         error("ERROR reading from socket");
//     printf("Here is the User name: %s\n", buffer);
//     for (int i = 0; i < 6; i++)
//     {
//         if (strcmp(auth.unameArr[i], buffer) == 0)
//         {
//             flag = 0;
//             printf("User name found");
//             n = write(newsockfd, "1", 1);
//             if (n < 0)
//                 error("ERROR writing to socket");
//             //read Pass
//                 bzero(buffer, 256);
//                 n = read(newsockfd, buffer, 255);
//                 if (n < 0)
//                     error("ERROR reading from socket");
//                 printf("Here is the Pass: %s %s\n", buffer,auth.passArr[i]);
//                 if (strcmp(auth.passArr[i],buffer)==0)
//                 {
//                     inflag = 0;
//                     printf("Correct Password\n");
//                     n = write(newsockfd, "1", 1);
//                     if (n < 0)
//                         error("ERROR writing to socket");
//                     puts("Authenticated-> ");
//                 }else
//                 {
//                     inflag=0;
//                     flag=1;
//                 }
//         }
//     }
//     if (flag == 1)
//     {
//         puts(" User not found"); //send ack 0
//         n = write(newsockfd, "0", 1);
//         if (n < 0)
//             error("ERROR writing to socket");
//     }
// }
// //reciver name
// loadRecData();
// flag = 1;
// while (flag == 1)
// {
// bzero(buffer, 256);
// n = read(newsockfd, buffer, 255);
// if (n < 0)
//     error("ERROR reading from socket");
// printf("Here is the User name: %s\n", buffer);
//     for (int i = 0; i < 6; i++)
//     {
//         if (strcmp(rec.SerNameArr[i], buffer) == 0)
//         {
//             flag = 0;
//             printf("RECEIVER Server Found in list\n");
//             n = write(newsockfd, "1", 1);
//             if (n < 0)
//                 error("ERROR writing to socket");
//             //read port
//             int inflag = 1;
//             while (inflag == 1)
//             {
//                 bzero(buffer, 256);
//                 n = read(newsockfd, buffer, 255);
//                 if (n < 0)
//                     error("ERROR reading from socket");
//                 printf("Here is the User name: %s\n", buffer);
//                 if (strcmp(rec.PortArr[i],buffer)==0)
//                 {
//                     inflag = 0;
//                     printf("RECEIVER Server Port is valid, connecting to RECEIVER\n");
// n = write(newsockfd, "1", 1);
// if (n < 0)
//     error("ERROR writing to socket");
//                     //connect to receiver
//                     // newConn(rec.IPArr[i], "5002"); //hard coded port
//                     newConn("127.0.0.1", rec.PortArr[i]); //testing only
//                 }
//             }
//         }
//     }
//     if (flag == 1)
//     {
//         puts("not found"); //send ack 0
//         n = write(newsockfd, "0", 1);
//         if (n < 0)
//             error("ERROR writing to socket");
//     }
// }
// struct userdict
// {
//     char unameArr[20][100], passArr[20][100];
// } auth;
// struct recDict
// {
//     char SerNameArr[20][100], IPArr[20][100], PortArr[20][100];
// } rec;
// int loadAuthData()
// {
//     FILE *authfile;
//     char s[30], n[30];
//     authfile = fopen("userList.txt", "r");
//     for (int i = 0; fscanf(authfile, "%s %s", s, n) != EOF; i++)
//     {
//         strcpy(auth.unameArr[i], s);
//         strcpy(auth.passArr[i], n);
//     }
//     fclose(authfile);
//     return 1;
// }
// int loadRecData()
// {
//     FILE *authfile;
//     char s[30], n[30], p[30];
//     authfile = fopen("receiverList.txt", "r");
//     for (int i = 0; fscanf(authfile, "%s %s %s", s, n, p) != EOF; i++)
//     {
//         strcpy(rec.SerNameArr[i], s);
//         strcpy(rec.IPArr[i], n);
//     }
//     fclose(authfile);
//     // for (int i = 0; i < 6; i++) //testing
//     // {
//     //     /* code */
//     //     puts(auth.unameArr[i]);
//     //     puts(auth.passArr[i]);
//     // }
//     return 1;
// }
