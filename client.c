#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024

extern void customers(int );
extern void police(int );
extern void admin(int );

// void error(char *msg)
// {
//     perror(msg);
//     exit(0);
// }

int main(int argc, char *argv[])
{
    int sock_fd, port, n;
    char temp[MAX];
    
    struct sockaddr_in server_addr;
    struct hostent *server;

    if (argc < 3) {
       fprintf(stderr,"Usage: %s hostname port\n", argv[0]);
       exit(0);
    }

    port = atoi(argv[2]);
    
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0))== -1)
    {
        perror("ERROR in opening socket");
        exit(1);
    } 
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR: Host not found\n");
        exit(0);
    }
    
    memset((char *) &server_addr,0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);
    
    // connecting to server
    if (connect(sock_fd,(struct sockaddr *)&server_addr,sizeof(server_addr))== -1)
    {
        perror("ERROR in connecting");
        exit(1);
    }   
    char username[MAX];
    char password[MAX];
    char user_type;
    while(1)
    {
        // taking credentials from user   
        printf("Enter the credentials.\n");
        memset(username,0,MAX);
        printf("Username: ");
        fgets(username, MAX, stdin);
            
        memset(password,0,MAX);
        printf("Password: ");
        fgets (password, MAX, stdin);
        
        memset(temp,0,MAX);
        strcat(temp,username);
        strcat(temp,"$$$");
        strcat(temp,password);

        // sending it to server
        n = write(sock_fd,temp,strlen(temp));
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        } 
        // false or exit or success (gives user type)
        memset(temp,0,MAX);
        n = read(sock_fd,temp,MAX-1);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }      
        if(!strcmp(temp,"exit"))
        {
            printf("You entered the invalid credentials 3 times. Exiting...\n");
            return 0;
        }
        
        if(strcmp(temp,"false"))
        {
            user_type = temp[0];
            break;
        }
    }
    
    /* welcome to the bank */
    if(user_type=='C')
    {
        printf("Welcome Bank_Customer.\n");
        customers(sock_fd);
    }
    else if(user_type=='A')
    {
        printf("Welcome Bank_Admin.\n");
        admin(sock_fd);
    }
    else if(user_type=='P')
    {
        printf("Welcome Police.\n");
        police(sock_fd);
    }

    // close the socket
    close(sock_fd);  
    return 0;
}