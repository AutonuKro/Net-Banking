#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 1024

// extern void error(char *);

void customers(int sock_fd)
{
    char temp[MAX];
    char operation[MAX];
    int n;
    char flag;
    
    printf("Do u want to continue (y/n): ");
    scanf("%c",&flag);
    getchar();
    
    while(flag=='y')
    {
        memset(temp,0,MAX);
        temp[0] = flag;
        temp[1] = '\0';
        n = write(sock_fd,temp,strlen(temp));
        if (n < 0)
	{
		perror("ERROR writing to socket");
		exit(1);
	} 
            

        printf("Operation to perform: ");
        memset(operation,0,MAX);
        fgets (operation, MAX, stdin);
        
        // sending command
        n = write(sock_fd,operation,strlen(operation));
        if (n < 0)
	{
		perror("ERROR writing to socket");
		exit(1);

	} 
                    
        operation[strlen(operation)-1] = '\0';

        // true or false
        memset(temp,0,MAX);
        n = read(sock_fd,temp,MAX-1);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }     
        
        if(!strcmp(temp,"false"))
        {
            printf("Invalid Operation.\n\n");
        }    
        else if(!strcmp(temp,"true"))
        {
            if(!strcmp(operation,"balance"))
            {
                // delimiter string
                memset(temp,0,MAX);
                strcpy(temp,"content");
                n = write(sock_fd,temp,strlen(temp));
                if (n < 0) 
                {
                    perror("ERROR reading from socket");
                    exit(1);
                }
                // balance
                memset(temp,0,MAX);
                n = read(sock_fd, temp, MAX-1);
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
                printf("BALANCE: %s\n\n", temp);   
            }
            else if(!strcmp(operation,"mini_statement"))
            {
                // delimeter string
                memset(temp,0,MAX);
                strcpy(temp,"size");
                n = write(sock_fd,temp,strlen(temp));
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
    
                
                // file size 
                memset(temp,0,MAX);
                n = read(sock_fd, temp, MAX-1);
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
                                    
                int file_size = atoi(temp);
                int remain_data = file_size;
                
                // delimeter string
                memset(temp,0,MAX);
                strcpy(temp,"content");
                n = write(sock_fd,temp,strlen(temp));
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
                // mini statement
                printf("MINI STATEMENT: \n");
                memset(temp,0,MAX);
                while ((remain_data > 0) && ((n = read(sock_fd, temp, MAX)) > 0))
                {
                    printf("%s", temp);
                    remain_data -= n;
                    memset(temp,0,MAX);
                }
                printf("\n\n");
            }
        }
        printf("Do u want to continue (y/n): ");
        scanf("%c",&flag);
        getchar();
    }
    // sending flag
    memset(temp,0,MAX);
    temp[0] = flag;
    temp[1] = '\0';
    n = write(sock_fd,temp,strlen(temp));  
}
