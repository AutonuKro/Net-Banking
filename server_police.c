#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/sendfile.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1024

extern void mini_statement(int, char *);
extern void available_balance(int, char *);
extern char *client_ip;

void police(int sock_fd)
{
    int n;
    char temp[MAX];
    char filename[100],op[MAX],id[MAX];

    // reading flag
    memset(temp,0,MAX);
    n = read(sock_fd,temp,MAX-1);
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
        
    
    while(temp[0]=='y')
    {
    	int check = 0;
        // reading command
        memset(temp,0,MAX);
        n = read(sock_fd,temp,MAX-1);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        } 
            
        
        // breaking command into operation and user_id
        char *ptr = strtok(temp,"$$$");
        strcpy(op,ptr);
        ptr = strtok(NULL,"$$$");
        strcpy(id,ptr);

        op[strlen(op)-1] = '\0';
        id[strlen(id)-1] = '\0';
        

        // checking for validity of user_id
        char *cred = NULL;
        size_t len = 0;
        
        FILE *fp = fopen("login_file.txt","r");
        if(fp == NULL)
        {
            perror("Error in opening login_file.");
            exit(1);
        }
            
        
        while(getline(&cred,&len,fp)!=-1)
        {
            char *username = strtok(cred," ");
            strtok(NULL," ");
            char *usertype = strtok(NULL, " ");
            
            if(!strcmp(username,id))
            {
                check=1;
                if(usertype[0]=='C')
                {
                    check=2;
                }
                break;
            } 
        }
        free(cred);
        fclose(fp);
        
        // sending false
        if(check==0 || check==1 || (strcmp(op,"balance") && strcmp(op,"mini_statement")))
        {
            fprintf(stdout, "Request from client with ip '%s' declined. \n", client_ip);    
            memset(temp,0,MAX);
            strcpy(temp,"false");
            n = write(sock_fd,temp,strlen(temp));  
            if (n < 0)
            {
                perror("ERROR writing to socket");
                exit(1);
            } 
                
        }
        else
        {
            if(!strcmp(op,"balance"))
            {
            	// sending true
                memset(temp,0,MAX);
                strcpy(temp,"true");
                n = write(sock_fd,temp,strlen(temp));  
                if (n < 0)
                {
                    perror("ERROR writing to socket");
                    exit(1);
                } 
                    
                
                // delimeter string
                memset(temp,0,MAX);
                n = read(sock_fd,temp,MAX-1);
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
                      
                
                available_balance(sock_fd,id);
            }
            else if(!strcmp(op,"mini_statement"))
            {
                // sending true
                memset(temp,0,MAX);
                strcpy(temp,"true");
                n = write(sock_fd,temp,strlen(temp));  
                if (n < 0)
                {
                    perror("ERROR writing to socket");
                    exit(1);
                } 
                    
                
                // delimeter string
                memset(temp,0,MAX);
                n = read(sock_fd,temp,MAX-1);
                if (n < 0)
                {
                    perror("ERROR reading from socket");
                    exit(1);
                } 
                      
                
                mini_statement(sock_fd,id); 
            }
        }
        /* Reading flag */
        memset(temp,0,MAX);
        n = read(sock_fd,temp,MAX-1);
        if (n < 0) 
        {
            perror("ERROR reading from socket");
            exit(1);
        }
            
    }
    
}
