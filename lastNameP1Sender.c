// sender code
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

//takes input and send it to Relay server for validation and return ACK, If Ack is Neg asks function is called again.
int SendAndReceiveAck(int socket_desc, char Arg[])
{
	char message[20], server_reply[2000];
	printf("Enter %s :", Arg);
	scanf("%s", message);
	if (send(socket_desc, message, strlen(message), 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	else
	{
		printf("sent %s, waiting for ACK ", message);
		//wait for ack
		if (read(socket_desc, server_reply, 255) < 0)
		{
			puts("Ack Not received ,failed\n");
		}

		else if (strcmp(server_reply, "Received") == 0)
		{
			// puts(server_reply);
			printf("ACK Received for %s\n", Arg);
			return 0;
		}
		else
		{
			puts(server_reply);
			printf("ACK, Incorrect %s OR no ACK\n", Arg);
			SendAndReceiveAck(socket_desc, Arg);
		}
	}
}

void error(char *msg)
{
	perror(msg);
	exit(1);
}

int connectToServer(int argc, char *argv[])
{

	// this function creates a client socket and connects to server
	if (argc != 3)
	{
		error("usage %s hostname port,Incorrect Run-time arguments, HOSTIP PORT - format.\n");
	}
	puts("Welcome to sender socket C program");
	int socket_desc;
	struct sockaddr_in server;
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		error("Could not create socket");
	}
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(atol(argv[2]));

	//Connect to remote server
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		error("connect error");
	}
	puts("Connected to Relay server\n");
	return socket_desc;
}

int ReadAndCompareMsg(int socket_desc, char Com[])
{
	char msg[256];

	if (read(socket_desc, msg, 256) < 0)
	{
		puts("recv failed\n");
	}
	printf("%ld , %s\n", strlen(Com), Com);
	printf("%ld , %s\n", strlen(msg), msg);
	if (strcmp(msg, Com) == 0)
	{
		puts("Authentication Success");
		return 1;
	}
	puts(msg);
	return 0;
}

void sendMsgAndReceiveHammingAndChecksum(int socket_desc)
{
	char buffer[256], server_reply[256];
	printf("Enter Message:");
	
	scanf("%s", buffer);
	if (strcmp(buffer, "q") == 0 || strcmp(buffer, "Q") == 0 || strcmp(buffer, "x") == 0 || strcmp(buffer, "X") == 0)
	{
		//send terminate signal and break
		// SendAndReceiveAck(socket_desc, "terminate");
		if (send(socket_desc, buffer, strlen(buffer), 0) < 0)
			error("message send failed, to Relay server, exit ");
		puts("sent, waiting for Reply");
		error("terminate signal sent, exited");
	}
	if (send(socket_desc, buffer, strlen(buffer), 0) < 0)
		error("message send failed, to Relay server, exit ");
	puts("sent, waiting for Reply");
	//wait for ack, with Hamming and checkSum code
	bzero(server_reply, 256);
	if (read(socket_desc, server_reply, 256) < 0)
	{
		puts("recv failed\n");
	}

	// ignore, logic to decode recived message and print hamming if even and checksum
	printf("%s for %s\n", server_reply, buffer);
}

int main(int argc, char *argv[])
{
	char *message, server_reply[2000], buffer[256];

	int socket_desc = connectToServer(argc, argv);

	//Send some username
	int flag = 1;
	while (flag)
	{
		SendAndReceiveAck(socket_desc, "User name");
		SendAndReceiveAck(socket_desc, "Password");
		if (ReadAndCompareMsg(socket_desc, "Success"))
			break;

		puts("Incorrect Credentials");
	}

	//Send reciver Name and port
	flag = 1;
	while (flag)
	{
		SendAndReceiveAck(socket_desc, " Receiver server");
		SendAndReceiveAck(socket_desc, "Port");
		if (ReadAndCompareMsg(socket_desc, "Success"))
			break;
		puts("Incorrect Credentials\n");
	}

	//need to validate ack about receiver connection,  wait for ack
	if (read(socket_desc, server_reply, 256) < 0)
		error("Reciver Connection,Ack read failed, Exited");
	puts(server_reply);
	puts("here after ack from reciver");

	flag = 1;
	while (flag == 1)
	{
		sendMsgAndReceiveHammingAndChecksum(socket_desc);
	}

	return 0;
}

//Receive a reply from the server
// if (recv(socket_desc, server_reply, 2, 0) < 0)
// {
// 	puts("recv failed");
// }
// puts("Reply received\n");
// puts(server_reply);
// SendAndReceiveAck(socket_desc, "Receiver server");
// validateData(socket_desc, "Port");
//receiver connection status
// if (recv(socket_desc, server_reply, 2, 0) < 0)
// {
// 	puts("recv failed");
// }
// puts("Reply received: ");
// puts(server_reply);
