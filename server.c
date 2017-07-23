/*
    	C socket server
	Name : Ashish Pawar
	Date : 6/6/2017	 	 
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<time.h>
#define MAX 999999
#define MAX_NUM 500
void dispArray(int *,int);
int cmpFun(const void *,const void *);
int randBet(int, int);
void *connection_handler(void *);
int fibonecci(int n);
void genFibonacciNum(int ,char*);

pthread_mutex_t lock;
FILE *fp;

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	fp = fopen("Command.txt","a+"); 
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	//Listen
	listen(socket_desc , 5);
	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);
	//Accept and incoming connectionp
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		if(pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
	//Now join the thread 
	//pthread_join( sniffer_thread , NULL);
	puts("Handler assigned");
	}
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	fclose(fp);
	pthread_mutex_destroy(&lock);
	return 0;
}

/*
 This will handle connection for each client
 */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	int i = 0, j = 0,k = 0,len;
	
	char *message , client_message[MAX], buffer[MAX],*reply_message;
	reply_message = (char *)malloc(MAX);
	char *tokens[MAX];
	char *token;
	time_t timer;
	char time_buff[26];
	struct tm* tm_info;
	
	if(fp == NULL)
	{
		printf("error opening file");
		exit(1);
	}
	
	bzero(reply_message,sizeof(reply_message));	
	while( (read_size = recv(sock , client_message , MAX,0)) > 0 )
	{
		//fp = fopen("record.txt","a+"); 
		time(&timer);
		tm_info = localtime(&timer);
		strftime(time_buff, 26, "[%Y-%m-%d %H:%M:%S]\t", tm_info);

		pthread_mutex_lock(&lock);
		fputs(time_buff,fp);
		fputs(client_message,fp);	
		//pthread_mutex_unlock(&lock);

		token = strtok(client_message," ");
		len = 0;
		while(token != NULL)
		{
			tokens[len++] = token;
			token = strtok(NULL," ");
		}
		
		if(len == 1)
		{
			char *c = strstr(tokens[0],"\n");
			*c = '\0';		
		}	
	
		if(strcmp(tokens[0],"help") == 0)
		{
			if(len == 1)
			{
				strcpy(reply_message,"you have following commands available \n\t- help \n\t- fibonecci [number]\n\t- sort [space seperated array numbers]\n\t- random [num1] [num2]\n\t- Show History\n\t- exit");
				//printf("- %s\n",reply_message);
			}
			else
			{
				//bzero(reply_message,sizeof(reply_message));
				strcpy(reply_message,"please enter valid input");
			}
		}
		else if(strcmp(tokens[0],"fibonacci") == 0)
		{			
			if(len == 2)
			{
				//sprintf(reply_message,"%d",fibonecci(atoi(tokens[1])));
				genFibonacciNum(atoi(tokens[1]),reply_message);
				//printf("fibo - %s\n",reply_message);
			}
			else
			{
				//bzero(reply_message,sizeof(reply_message));
				strcpy(reply_message,"please enter valid input");
			}
		}
		else if(strcmp(tokens[0],"sort") == 0)
		{
			int arr[len];
			//bzero(reply_message,sizeof(reply_message));
			k = 1;
			for(j = 0; j < len - 1; j++)
			{
				arr[j] = atoi(tokens[k]);
				k++;
			}
			//dispArray(arr,i - 1);
			qsort(arr, len - 1, sizeof(int),cmpFun);
			int index = 0;
			 for(j = 0; j < len - 1; j++)
			{
				index += sprintf(&reply_message[index],"%d",arr[j]);
				index += sprintf(&reply_message[index],"%c",' ');
			}
			//dispArray(arr,i - 1);
			//printf("fibo - %s\n",reply_message);
		}
		else if(strcmp(tokens[0],"random") == 0)
		{
			if(len == 3)
			{	
				srand(time(NULL));
				//bzero(reply_message,sizeof(reply_message));
				//int num = randBet(atoi(tokens[1]),atoi(tokens[2]))
				int lower, higher;
				lower = atoi(tokens[1])<atoi(tokens[2])? atoi(tokens[1]) : atoi(tokens[2]);
				higher = atoi(tokens[1])>atoi(tokens[2])? atoi(tokens[1]) : atoi(tokens[2]);
				sprintf(reply_message,"%d",randBet(lower,higher));
				//printf("rando - %s\n",reply_message);
			}
			else
			{
				//bzero(reply_message,sizeof(reply_message));
				strcpy(reply_message,"please enter valid input");
			}

		}
		else if(strcmp(tokens[0],"Show") == 0)
		{
			//bzero(reply_message,sizeof(reply_message));
			char *c = strstr(tokens[1],"\n");
			*c = '\0';		

			if(len == 2 && strcmp(tokens[1],"History") == 0)
			{
				//pthread_mutex_lock(&lock);
				fseek(fp,0,SEEK_END);
				long fsize = ftell(fp);
				fseek(fp,0,SEEK_SET);
				fread(reply_message,1,fsize,fp);	
				reply_message[fsize] = '\0';
				fflush(fp);	
				//pthread_mutex_unlock(&lock);
			}
			else
			{
				strcpy(reply_message,"please enter valid input");
			}	
		}
		else	if(strcmp(tokens[0],"exit") == 0)
		{
			//bzero(reply_message,sizeof(reply_message));
			if(len == 1)
			{
				strcpy(reply_message,"exit");
			}	
			else		
			{
				//bzero(reply_message,sizeof(reply_message));
				strcpy(reply_message,"please enter valid input");
			}
		}
		else
		{
			//bzero(reply_message,sizeof(reply_message));
			strcpy(reply_message,"please enter valid input");
			//printf("please enter valid input");
		}	
		
		//printf("-%d",strlen(reply_message) );					
		write(sock , reply_message , strlen(reply_message));

		//pthread_mutex_lock(&lock);
		fputs(reply_message,fp);
		fputs("\n",fp);
		//fclose(fp);
		fflush(fp);	

		bzero(reply_message,sizeof(reply_message));
		bzero(client_message, sizeof(client_message));
		pthread_mutex_unlock(&lock);
	}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	//Free the socket pointer
	
	
	free(socket_desc);
	return 0;
}

//calculate fibonecci
int fibonecci(int n)
{
	//printf("%d", n);
	if(n == 0)
		return 0;
	else if(n == 1)
		return 1;
	return (fibonecci(n-1) + fibonecci(n-2)); 
}

//display array
void dispArray(int *arr,int len)
{
	int i;
	for(i = 0; i < len; i++ )
		printf("%d ",arr[i]);
	printf("\n");
}

//cmp function for qsort
int cmpFun(const void * a,const void * b)
{
	return (*(int *)b - *(int *)a);
}

//generate random no
int randBet(int low,int high)
{
	return (low + rand()%(high-low+1));
}

void genFibonacciNum(int n, char *msg)
{
	int i, j,k;
	int carry = 0, add;
	//char msg[MAX];
	int fib_value[MAX_NUM] = {0};
	int fib_prev[MAX_NUM] = { 0 };
	int fib_next[MAX_NUM] = { 0 };
	fib_prev[MAX_NUM - 1] = 0;
	fib_next[MAX_NUM - 1] = 1;
	//bzero(msg,sizeof(msg));

/*	if (n < 0)
	{
		while (n < 0)
		{
			strcpy(msg,"you have entered negative number");
			return msg; 
		}
	}*/
	if (n == 0)
	{
		fib_value[MAX_NUM - 1] = fib_prev[MAX_NUM - 1];
		j = 1;
	}
	else if (n == 1)
	{
		fib_value[MAX_NUM - 1] = fib_next[MAX_NUM - 1];
		j = 1;
	}
	else
	{
		for (i = 2; i <= n; i++)
		{
			for (j = 1; j <= MAX_NUM; j++)
			{
				add = fib_prev[MAX_NUM - j] + fib_next[MAX_NUM - j] + carry;
				fib_value[MAX_NUM - j] = (add % 10);
				carry = add / 10;
				fib_prev[MAX_NUM - j] = fib_next[MAX_NUM - j];
				fib_next[MAX_NUM - j] = fib_value[MAX_NUM - j];
            }
        }
    }
        for(j = 0; j < MAX_NUM; j++)
        if(fib_value[j]!= 0)
            break;
        j = MAX_NUM - j;

	int index = 0;
	 for (k = j; k >= 1; k--)
	{
		index += sprintf(&msg[index],"%d",fib_value[MAX_NUM-k]);
		//index += sprintf(&reply_message[index],"%c",' ');
	}
	sprintf(&msg[index],"%c",'\0');
    //return msg;
}
