#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

int validateData(int socket_desc, char Arg[])
{
	char message[20], server_reply[2000];
	printf("Enter %s :", Arg);
	scanf("%s", &message);
	if (send(socket_desc, message, strlen(message), 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	else
	{
		puts("sent, waiting for ACK");
		//wait for ack
		if (recv(socket_desc, server_reply, 1, 0) < 0)
		{
			puts("recv failed\n");
		}
		else if (server_reply[0] == '1')
		{
			printf("ACK, correct %s\n", Arg);
			return 0;
		}
		else
		{
			printf("ACK, Incorrect %s OR no ACK\n", Arg);
			validateData(socket_desc, Arg);
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "usage %s hostname port,Incorrect Run-time arguments, HOSTIP PORT - format.\n", argv[0]);
		exit(0);
	}

	int socket_desc;
	struct sockaddr_in server;
	char *message, server_reply[2000];

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(atol(argv[2]));

	//Connect to remote server
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected to Relay server\n");

	//Send some username
	int flag = 1;
	//while (flag)
	//{

	validateData(socket_desc, "User name");
	validateData(socket_desc, "Password");
	//}

	//Receive a reply from the server
	if (recv(socket_desc, server_reply, 2, 0) < 0)
	{
		puts("recv failed");
	}
	puts("Reply received\n");
	puts(server_reply);

	return 0;
}
