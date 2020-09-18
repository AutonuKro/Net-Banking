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

// extern void error(char *);
extern char *client_ip;

void available_balance(int sock_fd,char *cust_id)
{
    int n;
    char filename[MAX];
    sprintf(filename,"%s",cust_id);
    strcat(filename,".txt");
    
    FILE *fp = fopen(filename,"r");
    if(fp == NULL)
    {
        perror("ERROR reading from socket");
        exit(1);
    }
    
    
    char *transaction = NULL;
    size_t len = 0;
    char *balance;

    while(getline(&transaction,&len,fp)!=-1)
    {
        strtok(transaction," ");
        strtok(NULL, " ");
        balance = strtok(NULL, " ");
    }
    
    fprintf(stdout, "Sending balance of customer '%s' to client with ip '%s'. \n", cust_id, client_ip);
    // balance
    n = write(sock_fd,balance,strlen(balance));
    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(1);
    } 
    free(transaction);
    fclose(fp);
}

void mini_statement(int sock_fd, char *cust_id)
{
    int n;
    char filename[MAX];
    sprintf(filename,"%s",cust_id);
    strcat(filename,".txt");
    struct stat file_stat;
    char temp[MAX];

    
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        perror("Error in opening user file for mini_statement");
        exit(1);
    }
    
    // finding stats of file
    if (fstat(fd, &file_stat) < 0)
    {
        perror("Error in getting statistics of file.");
        exit(1);
    }

    // writing size of file
    memset(temp,0,MAX);
    sprintf(temp, "%d", (int)file_stat.st_size);
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
        perror("ERROR writing to socket");
        exit(1);
    } 
    
    
    // sending mini statement        
    fprintf(stdout, "Sending mini statement of customer '%s' to client with ip '%s'. \n", cust_id, client_ip);
    while (1) 
    {
        memset(temp,0,MAX);
        int bytes_read = read(fd, temp, sizeof(temp));
        if (bytes_read == 0) 
            break;
        if (bytes_read < 0)
        {
            perror("ERROR reading from file.");
            exit(1);
        } 
        
        void *ptr = temp;
        while (bytes_read > 0) {
            int bytes_written = write(sock_fd, ptr, bytes_read);
            if (bytes_written <= 0)
            {
                perror("ERROR writing to socket");
                exit(1);
            } 
            bytes_read -= bytes_written;
            ptr += bytes_written;
        }
    }
    close(fd);         
}


void customer(int sock_fd,int cust_id)
{
    int n;
    char temp[MAX];
    char id[MAX];
    sprintf(id,"%d",cust_id);

    /* Reading flag */
    memset(temp,0,MAX);
    n = read(sock_fd,temp,MAX-1);
    if (n < 0) 
    {
        perror("ERROR reading to socket");
        exit(1);
    }
    
    while(temp[0]=='y')
    {   
        // reading command
        memset(temp,0,MAX);
        n = read(sock_fd,temp,MAX-1);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        } 
        
        temp[strlen(temp)-1] = '\0';
        
        if(!strcmp(temp,"balance"))
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
        else if(!strcmp(temp,"mini_statement"))
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
        else
        {
            fprintf(stdout, "Request from client with ip '%s' declined. \n", client_ip);    
            // sending false
            memset(temp,0,MAX);
            strcpy(temp,"false");
            n = write(sock_fd,temp,strlen(temp));  
            if (n < 0)
            {
                perror("ERROR writing to socket");
                exit(1);
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