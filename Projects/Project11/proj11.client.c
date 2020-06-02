//Project 11 Client

//Author: Clare Kinery

//Thanks for being a great TA Robert!

#include <fcntl.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <netdb.h> 
using namespace std;

#define BSIZE 256

/** Intakes a string splits by space or tab
* returns a vector of strings 
*/
vector<string> array_make(string line)
{
  vector<string> splitString;
  string word = "";
  
  //check see if tab or space
  string check = "";
  for (unsigned int i=0; i<line.length(); i++)
  {
    check.push_back(line[i]);
    if(check == " " || check == "\t" || check == "\n")
    {
      if(word != "")
      {
        splitString.push_back(word);
        //reset word
        word = "";
      }
    }
    else
    {
      word += line[i];
    }

    //reset check
    check = "";
  } 

  //append final word if present
  if (word.length() > 0)
  {
    splitString.push_back(word);
  }
  return splitString;
}


int main(int argc, char* argv[] )
{
  if (argc != 3)
  {
    printf("Usage:  % <host> <port>\n", argv[0]);
    exit(1);
  }
    char * hostname = argv[1];
    int port = atoi( argv[2] );

    char buffer[BSIZE];
    char send_buff[BSIZE];
    char file_buff[BSIZE];

  while(1)
  {

    //prompt for command and filename
    printf( "Enter Command: ");
    bzero( buffer, BSIZE );
    bzero(send_buff, BSIZE);
    string user_input = fgets( buffer, BSIZE, stdin );
    printf( "\n" );

    //display input line
    printf(user_input.c_str());
    printf("\n");
    vector<string> command = array_make(user_input); 

    if(command.size() == 0)
    {
      continue;
    }
    //if files the same infinite loop
    else if(command[1] == command[2])
    {
      printf("different files needed\n\n");
    }

    //only vaild if array is 3 in length
    else if((command[0] == "get")&& (command.size() == 3))
    {
      int sd = socket( AF_INET, SOCK_STREAM, 0 );
      if (sd < 0)
      {
        perror( "socket" );
        exit( 2 );
      }

      struct hostent * server = gethostbyname( hostname );
      if (server == NULL)
      {
        printf( "Error: no such host %s\n", hostname );
        exit( 3 );
      }

      struct sockaddr_in saddr;

      bzero( &saddr, sizeof(saddr) );
      saddr.sin_family = AF_INET;
      bcopy( server->h_addr, &saddr.sin_addr.s_addr, server->h_length );
      saddr.sin_port = htons( port );

      int stat = connect( sd, (struct sockaddr *) &saddr, sizeof(saddr) );
      if (stat < 0)
      {
        perror( "connect" );
        exit( 4 );
      }
      const char * status = command[1].c_str();

     int nsend = send( sd, status, strlen(status), 0 );
          if (nsend < 0) 
        {
          perror( "send" );
          exit( 5 );
        }

      bzero( buffer, BSIZE );
      int nrecv = recv( sd, buffer, BSIZE, 0 );
      if (nrecv < 0) 
      {
        perror( "recv" );
        exit( 4 );
      }
      //check file recieve status
      if(strcmp(buffer, "error") == 0)
      {
        printf("file error \n");
        printf("\n");
      }
      else //recieve again and read to new file
      {
        //open the destination file
        int dest;
        FILE * out_fp;
        dest = open(command[2].c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        
        //read file context
        while(nrecv > 0)
        {
          write(dest, &buffer, nrecv);
          bzero(buffer,BSIZE);
          nrecv = recv(sd, buffer, BSIZE,0);
        }
        close(dest);
      }
      close(sd);
    }

    else if (command[0] == "quit")
    {

      int sd = socket( AF_INET, SOCK_STREAM, 0 );
      if (sd < 0)
      {
        perror( "socket" );
        exit( 2 );
      }

      struct hostent * server = gethostbyname( hostname );
      if (server == NULL)
      {
        printf( "Error: no such host %s\n", hostname );
        exit( 3 );
      }

      struct sockaddr_in saddr;

      bzero( &saddr, sizeof(saddr) );
      saddr.sin_family = AF_INET;
      bcopy( server->h_addr, &saddr.sin_addr.s_addr, server->h_length );
      saddr.sin_port = htons( port );

      int stat = connect( sd, (struct sockaddr *) &saddr, sizeof(saddr) );
      if (stat < 0)
      {
        perror( "connect" );
        exit( 4 );
      }

      const char * send_buff = command[0].c_str();
      int nsend = send( sd, send_buff, strlen(send_buff), 0 );
      if (nsend < 0) 
      {
        perror( "send" );
        exit( 5 );
      }
      close(sd);
      exit(1);
    }
    else if (command.size() != 3)
    {
      printf("Incorrect number of arguments \n");
      printf("\n");
    }
    else
    {
      printf("Invaild command\n");
      printf("\n");
    }
  }
}

