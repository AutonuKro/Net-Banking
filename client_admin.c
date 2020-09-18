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

void admin(int sock_fd)
{
	char temp[MAX];
    char id[MAX],trans[MAX],amount[MAX];
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
        printf("User ID of Customer: ");
        memset(id,0,MAX);
        fgets (id, MAX, stdin);
        
        printf("Transaction Type: ");
        memset(trans,0,MAX);
        fgets (trans, MAX, stdin);

        printf("Amount: ");
        memset(amount,0,MAX);
        fgets (amount, MAX, stdin);

        memset(temp,0,MAX);
        strcat(temp,id);
        strcat(temp,"$$$");
		strcat(temp,trans);
        strcat(temp,"$$$");
        strcat(temp,amount);
        strcat(temp,"$$$");

        // sending command
        n = write(sock_fd,temp,strlen(temp));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        } 
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
            printf("Transaction denied.\n\n");
        }    
        else if(!strcmp(temp,"true"))
        {
            printf("Transaction successful.\n\n");
        }
        else if(!strcmp(temp,"deficit"))
        {
        	printf("Insufficient Amount.\n\n");
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