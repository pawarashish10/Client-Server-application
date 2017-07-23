/*
    	C socket client
	Name : Ashish Pawar
	Date : 6/6/2017	 	 
*/
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#define SERVER_PORT "8888"
#define MAX 999999
int main(int argc , char *argv[])
{
	int sock,s;
	struct addrinfo hints, *result;
	struct sockaddr_in server;
	char message[MAX] , server_reply[MAX];
	char *host;

	if (argc==2)
	{
		host = argv[1];
    	}
	else
	{
		fprintf(stderr, "usage: %s host\n", argv[0]);
		exit(1);
	}
	
	    /* Translate host name into peer's IP address */
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_INET;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = 0;
	    hints.ai_protocol = 0;
	    
	    if ((s = getaddrinfo(host, SERVER_PORT, &hints, &result)) != 0 )
	    {
		fprintf(stderr, "%s: getaddrinfo: %s\n", argv[0], gai_strerror(s));
		exit(1);
	    }
	  //create socket
	    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	     if (sock == -1)
		{
		    printf("Could not create socket");
		}
	    else
	    {
		    //puts("Socket created");
	    }
	    
	    //connect to port 
	    if (connect(sock, result->ai_addr, result->ai_addrlen) < 0)
	    {
		    perror("connect failed. Error");
		    return 1;
	    }
	    else
	    {
	    	//puts("connection success");
	    } 
	    
	//keep communicating with server
	while(1)
	{
		bzero(server_reply, sizeof(server_reply));
		printf(">>");
		//scanf("%s" , message);
		fgets(message,100,stdin);		
		//printf("len : %d", strlen(message));
		//message[strlen(message)] = '\0';
		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
	//Receive a reply from the server
		if( recv(sock , server_reply , MAX , 0) < 0)
		{
			puts("recv failed");
			break;
		}
		puts("Server reply :");
		if(strcmp(server_reply,"exit") == 0)
		{
			exit(0);
		}
		puts(server_reply);
		bzero(server_reply, sizeof(server_reply));
	}
	close(sock);
	return 0;
}
