#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "string.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <unistd.h>


using namespace std;
int dest = 1 ;
bool modify= false;
bool trun= true;
bool optionb= false;
string buff = "";
string fileA = "";
string fileB = "";
int file_cnt = 0;
int buffd = 256;
ssize_t ret_in, ret_out;
 
int main(int argc, char* argv[])
{
    int f_count = 0;
    for (int i = 2 ; i < argc; i++)
    {
        int cmd_cnt = 0;
        if (argv[i][0] == '-')
        {
            if (strcmp(argv[i],"-m") == 0) 
            {
                modify=true;
                cmd_cnt++;
            }
            else if (strcmp(argv[i],"-nm") == 0) 
            {
                modify=false;
                cmd_cnt++;
            }

            if (strcmp(argv[i],"-t") == 0) 
            {
                trun=true;
                cmd_cnt++;
            }
            else if (strcmp(argv[i],"-nt") == 0) 
            {
                trun=false;
                cmd_cnt++;
            }

            if (strcmp(argv[i],"-b") == 0) //seg fault if -b end and no num
            {
                optionb=true;
                if (i+1 >= argc)
                {
                    cout<< "invaild command" << endl;
                    return 0;
                }
                buff = argv[i+1];
                i++;
                cmd_cnt++;
            }

            if (cmd_cnt == 0)
            {
                cout << "invaild command" << endl;
                return 0;
            }
        }
        else if (argv[i][0] != '-' && file_cnt == 0)
        {
            fileA = argv[i];
            file_cnt++;
        }
        else if (argv[i][0] != '-' && file_cnt == 1)
        {
            fileB = argv[i];
            file_cnt++;
        }
        else
        {
            cout << "files incorrect" << endl;
            return 0;
        }

    }

    if (file_cnt < 2)
    {
        cout << "files incorrect" << endl;
        return 0;
    }

    //buffer error
    if ( buff.length() > 0)
    {
        int buffcon = stoi(buff);
        buffd = pow(2, buffcon);
    }
    
    //read only source
    int source = open( fileA.c_str(), O_RDONLY, S_IRUSR | S_IWUSR); 
    dest = open( fileB.c_str(), O_WRONLY, S_IRUSR | S_IWUSR); 

    if (source == -1)
    {
        cout << "file doesn't exist" << endl;
        return 0;
    } 

    if (dest == -1)
    {
        dest = open( fileB.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); 
        close(dest);
    }

    if (modify == false)
    {
        cout << "the default is not modify" << endl;
        return 0;
    }
    
    char buffer[buffd]; //buffer ptr
    
    if (dest == -1)
    {
        dest = open( fileB.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); 
        while((ret_in = read (source, &buffer, buffd)) > 0)
        {
            ret_out = write (dest, &buffer, (ssize_t) ret_in);
        }
        close(dest);
    }
    else
    {
        if (trun == false)
        {
            //write only destination 
            dest = open( fileB.c_str(), O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
            while((ret_in = read (source, &buffer, buffd)) > 0)
            {
                ret_out = write (dest, &buffer, (ssize_t) ret_in);
            }
            close(dest);
        }
        else
        {
            dest = open( fileB.c_str(), O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
            while((ret_in = read (source, &buffer, buffd)) > 0)
            {
                ret_out = write (dest, &buffer, (ssize_t) ret_in);
            }
            close(dest);
        }
    }
    close(source);
    return 0;
}
