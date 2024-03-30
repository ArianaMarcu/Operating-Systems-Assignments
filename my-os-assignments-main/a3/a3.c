#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_NAME1 "RESP_PIPE_25012"
#define FIFO_NAME2 "REQ_PIPE_25012"

int main()
{
    int fd1 = -1, fd2 = -1;
    
    //creeaza un pipe denumit “RESP_PIPE_25012”
    if(mkfifo(FIFO_NAME1, 0600) != 0)
    {
    	perror("ERROR\ncannot create the response pipe");
    	return 1;
    }
    
    //deschide in citire un pipe denumit “REQ_PIPE_25012”, care este creat automat de catre tester
    fd1 = open(FIFO_NAME2, O_RDONLY);
    if(fd1 == -1)
    {
    	perror("ERROR\ncannot open the request pipe");
    	return 1;
    }
    
    //deschide in scriere pipe-ul “RESP_PIPE_25012”, care a fost creat la pasul 1
    fd2 = open(FIFO_NAME1, O_WRONLY);
    if(fd2 == -1)
    {
    	return 1;
    }
    
    //scrie mesajul request in pipe-ul “RESP_PIPE_25012”
    write(fd2, "CONNECT#", 8);
    printf("SUCCESS\n");
    
    while(1)
    {
    	char* string_fields = (char*)calloc(250, sizeof(char));
    	int i = 0;
    	char c = ' ';
    	while(c != '#')
    	{
    	    read(fd1, &c, 1);
    	    string_fields[i] = c;
    	    i++;
    	}
    	string_fields[i-1] = '\0';
    	
    	if(strcmp(string_fields, "PING") == 0)
    	{
    	     unsigned int varianta = 25012;
    	     write(fd2, "PING#", 5);
    	     write(fd2, &varianta, 4);
    	     write(fd2, "PONG#", 5);
    	}
    	else if(strcmp(string_fields, "EXIT") == 0)
    	{
    	    unlink(FIFO_NAME2);
            unlink(FIFO_NAME1);
            return 0;
    	}
    	else 
    	    break;
    }
    unlink(FIFO_NAME2);
    unlink(FIFO_NAME1);
    return 0;
}
