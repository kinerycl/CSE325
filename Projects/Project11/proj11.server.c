//Project 11 Server

//Author: Clare Kinery

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;
#define BSIZE 256

int main()
{
  char hostname[256];
  gethostname(hostname, 256);

  struct hostent* h;
  h = gethostbyname(hostname);
  //print domain name
  printf("%s", h->h_name);
  printf(" ");

  int listen_sd = socket( AF_INET, SOCK_STREAM, 0 );
  if (listen_sd < 0) 
  {
    perror( "socket" );
    exit( 2 );
  }

  struct sockaddr_in saddr;

  bzero( &saddr, sizeof(saddr) );
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;
  saddr.sin_port = htons(0);

  int bstat = bind( listen_sd, (struct sockaddr *) &saddr, sizeof(saddr) );
  if (bstat < 0)
  {
    perror( "bind" );
    exit( 3 );
  }

  socklen_t len = sizeof(saddr);
  int soc_name = getsockname(listen_sd, (struct sockaddr *) &saddr, &len);
  //print port number
  printf("%d\n",ntohs(saddr.sin_port));

  int lstat = listen( listen_sd, 5 );
  if (lstat < 0)
  {
    perror( "listen" );
    exit( 4 );
  }

  while (1)
  {
    struct sockaddr_in caddr;
    unsigned int clen = sizeof(caddr);

    int comm_sd = accept( listen_sd, (struct sockaddr *) &caddr, &clen );
    if (comm_sd < 0)
    {
      perror( "accept" );
      exit( 5 );
    }

    struct hostent *host_info = gethostbyaddr(
      (const char *) &caddr.sin_addr.s_addr,
      sizeof(caddr.sin_addr.s_addr), AF_INET);

    if (host_info == NULL)
    {
      perror( "gethostbyaddr" );
      exit( 6 );
    }

    char * host = inet_ntoa( caddr.sin_addr );
    if (host == NULL)
    {
      perror( "inet_ntoa" );
      exit( 7 );
    }

    char buffer[BSIZE];
    char send_buff[BSIZE];
    char file_buff[BSIZE];

    //get command
    bzero( buffer, BSIZE );
    int nrecv = recv( comm_sd, buffer, BSIZE, 0 );
    if (nrecv < 0) 
    {
      perror( "recv" );
      exit( 8 );
    }
    
    //if buffer is quit - close out both sockets and break loop
    if(strcmp(buffer, "quit") == 0)
    {
      close(listen_sd);
      close(comm_sd);
      exit(0);
    }
    else // recieve again to get the file name
    {
/*      bzero( buffer, BSIZE );
      int nrecv = recv( comm_sd, buffer, BSIZE, 0 );
      if (nrecv < 0) 
      {
        perror( "recv" );
        exit( 8 );
      }*/
      

      //file
      int source = open(buffer, O_RDONLY, S_IRUSR | S_IWUSR);
      if (source == -1)
      {
        // send a signal that the file does not exits - "error"
        const char * send_buff = "error";
        int nsend = send( comm_sd, send_buff, strlen(send_buff), 0 );
        if (nsend < 0) 
        {
          perror( "send" );
          exit( 9 );
        }
      }
      else //write from file to buffer
      {
        //zero your buffer
        
        int b_size = 1;
        while (b_size >0)
        {
          bzero(buffer,BSIZE);
          b_size = read(source, &buffer, BSIZE);
          // send the buffer
          send(comm_sd, buffer, b_size, 0);
        }
        // close file
        close(source);
      }
      close( comm_sd );
    }
  }
}

